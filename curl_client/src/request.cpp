//
// Created by Andrey Solovyev on 22/09/2021.
//

#include "request.h"
#include "const_values.h"

#include <stdexcept>
#include <type_traits>
#include <sstream>
#include <utility>
#include <iterator>

namespace curl_client {

	request::request()
		: curl (std::make_unique<CurlHandler>())
		, url_ (url::create())
	{}

	response request::implement(Method method, http::query_data_ptr&& query){
		response_ = std::make_unique<response_data>();
		if (!url_ || url_->empty()){
			response_->body = curl_client::const_values::NO_PATH_MSG;
			response_->response_code = curl_client::const_values::NO_REQUEST_MADE;
			return std::move(response_);
		}
		if (not curl) {
			response_->body = curl_client::const_values::NO_CURL_HANDLE_MSG;
			response_->response_code = curl_client::const_values::NO_REQUEST_MADE;
			return std::move(response_);
		}

		//todo this is shameful, but I am under time pressure
		std::string urlStr {url_->to_string()};
		if (method == Method::FTPListOnly) {
			urlStr += '/';
		}

		curl->setUrl(urlStr);
		curl->setMethod(method);
		if (query) {
			curl->setQuery(std::move(query));
		}

		curl->performRequest();

		response_->request_ok = curl->checkRequestOk();
		response_->response_code = curl->getResponseCode();
		if (not response_->request_ok){
			return std::move(response_);
		}
		response_->headers = curl->getResponseHeaders();
		ResponseType type = curl->getResponseType();
		(this->*set_response_body.at(type))();

		return std::move(response_);
	}

	request* request::set_verbose (bool verbose) {
		if (curl) curl->setVerbose(verbose);
		return this;
	}

	request* request::setURL(std::string path_) {
		auto new_url_ptr {url::create(std::move(path_))};
		std::swap(url_, new_url_ptr);
		new_url_ptr.reset();
		return this;
	}

	std::string const& request::getURL() const{
		return url_->to_string();
	}

	URLptr request::manageURL() const {
		return url_->get_shared();
	}

	const std::unordered_map <ResponseType, void (request::*)()> request::set_response_body {
			{ResponseType::None, &request::returnNone},
			{ResponseType::Text, &request::returnText},
			{ResponseType::Json, &request::returnJson},
			{ResponseType::CurlError, &request::returnError},
	};

	void request::returnNone() {
		throw std::runtime_error("Unable to SET RETURN TYPE "+std::string(__PRETTY_FUNCTION__));
	}

	void request::returnText() {
		response_->body = curl->getResponseBody();
	}

	void request::returnError() {
		response_->body = curl->getError();
	}

	void request::returnJson() {
		//buffer
		std::stringstream json_to_str;
		auto _ = curl->getResponseBody();
		json_to_str.write(_.data(), _.size());
		response_->body = Json::Load(json_to_str);
	}


	request* request::set_headers (http::headers headers) {
		curl->setHeaders(headers);
		return this;
	}

	request* request::set_header (http::header header) {
		curl->setHeader(header);
		return this;
	}

	http::headers const& request::get_headers () const {
		return curl->getRequestHeaders();
	}

	http::header const& request::get_header(http::header::field_type name) const {
		return curl->getRequestHeaderAt(name);
	}

	request* request::set_query (std::string input) {
		http::query_data qd (std::move(input));
		curl->setQuery(std::make_unique<http::query_data>(std::move(qd)));
		return this;
	}


	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

	void Print (const response &response_, std::ostream &os, bool print_headers) {
		if (not response_) {
			os << "ERROR: Response is nullptr\n";
			return;
		}

		if (not response_->request_ok){
			os << "ERROR: " << response_->response_code << '\n';
//		  return;
		}

		if (print_headers){
			for (const auto &header : response_->headers){
				os << header;
			}
		}

		std::visit(overloaded {
				[&os](std::monostate arg) { os << "monostate\n"; },
				[&os](const std::string &arg) { os << arg << '\n'; },
				[&os](const Json::Document& arg) { Json::Print(arg, os); }
		}, response_->body);
	}

}//!namespace

