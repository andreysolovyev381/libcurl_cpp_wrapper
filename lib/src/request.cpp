//
// Created by Andrey Solovyev on 22/09/2021.
//

#include "request.h"
#include "utils.h"
#include <stdexcept>
#include <type_traits>
#include <sstream>
#include <utility>
#include <iterator>

namespace curl_client {

  Request::Request()
		  : curl (std::make_unique<CurlHandler>())
		  , path (std::make_unique<PathData>())
  {}

  Response Request::Implement(Method method, Query&& query){
	  response = std::make_unique<ResponseData>();
	  if (pathIsEmpty()){
		  response->body = NO_PATH_MSG;
		  return std::move(response);
	  }
	  if (not curl) {
		  response->body = NO_CURL_HANDLE_MSG;
		  return std::move(response);
	  }

	  curl->setUrl(pathGetStringified());
	  curl->setMethod(method);
	  if (query) curl->setQuery(std::move(query));

	  curl->performRequest();

	  response->request_ok = curl->checkRequestOk();
	  response->response_code = curl->getResponseCode();
	  if (not response->request_ok){
		  return std::move(response);
	  }
	  response->headers = std::move(curl->getResponseHeaders());
	  ResponseType type = curl->getResponseType();
	  (this->*set_response_body.at(type))();

	  return std::move(response);
  }

  Request* Request::SetVerbose (bool verbose) {
	  if (curl) curl->setVerbose(verbose);
	  return this;
  }

  Request* Request::pathSetNew(std::string&& path) {
	  auto new_path = std::make_unique<PathData>(std::move(path));
	  this->path.swap(new_path);
	  this->path->is_update_required = true;
	  return this;
  }

  bool Request::pathIsEmpty() const {
	  return path->elems.empty();
  }
  void Request::pathSetEmpty() {
	  path->elems.clear();
	  path->full_str.clear();
  }
  const std::string& Request::pathGetStringified () {
	  if (path->is_update_required) path->Stringify();
	  return path->full_str;
  }
  size_t Request::pathGetElemCount() const {
	  return path->elems.size();
  }

  Request* Request::pathPushBackElement(std::string &&elem) {
	  auto new_elem = std::move(utils::splitIntoWords(elem, path->delim)); //todo: check how NRVO works
	  path->elems.insert(path->elems.end(), std::move_iterator(new_elem.begin()), std::move_iterator(new_elem.end()));
	  path->is_update_required = true;
	  return this;
  }
  Request* Request::pathPopBackElement() {
	  path->elems.pop_back();
	  path->is_update_required = true;
	  return this;
  }
  const std::string& Request::pathGetBackElement() const {
	  return path->elems.back();
  }
  const std::string& Request::pathAt(size_t idx) const {
	  if (idx > path->elems.size()) throw std::invalid_argument(
				  "Out of boundaries, required idx is " + std::to_string(idx) +
						  ",\nwhile path vector size is " + std::to_string(path->elems.size()) +
						  std::string (__PRETTY_FUNCTION__ ));
	  return path->elems.at(idx);
  }

  std::string& Request::pathAt(size_t idx){
	  if (idx > path->elems.size()) throw std::invalid_argument(
				  "Out of boundaries, required idx is " + std::to_string(idx) +
						  ",\nwhile path vector size is " + std::to_string(path->elems.size()) +
						  std::string (__PRETTY_FUNCTION__ ));
	  path->is_update_required = true;
	  return path->elems.at(idx);
  }


  const std::unordered_map <ResponseType, void (Request::*)()> Request::set_response_body {
		  {ResponseType::None, &Request::returnNone},
		  {ResponseType::Text, &Request::returnText},
		  {ResponseType::Json, &Request::returnJson},
		  {ResponseType::CurlError, &Request::returnError},
  };

  void Request::returnNone() {
	  throw std::runtime_error("Unable to SET RETURN TYPE "+std::string(__PRETTY_FUNCTION__));
  }
  void Request::returnText() {
	  response->body = std::move(curl->getResponseBody());
  }
  void Request::returnError() {
	  response->body = std::move(curl->getError());
  }
  void Request::returnJson() {
	  //buffer
	  std::stringstream json_to_str;
	  auto _ = std::move(curl->getResponseBody());
	  json_to_str.write(_.data(), _.size());
	  response->body = std::move(Json::Load(json_to_str));
  }


  Request* Request::headersSetAll (HttpHeaders headers) {
	  curl->setHeaders(headers);
	  return this;
  }

  Request* Request::headerSetSingle (HttpHeader header) {
  	curl->setHeader(header);
  	return this;
  }

  const HttpHeaders& Request::headersGetAll () const {
  	return curl->getRequestHeaders();
  }

  const HttpHeader& Request::headerGetOne(HeaderName name) const {
	  return curl->getRequestHeaderAt(name);
  }


  void Print (const Response &response, std::ostream &os, bool print_headers) {
	  if (not response) {
		  os << "ERROR: Response is nullptr\n";
		  return;
	  }

	  if (not response->request_ok){
		  os << "ERROR: " << response->response_code << '\n';
//		  return;
	  }

	  if (print_headers){
		  for (const auto &header : response->headers){
			  os << header;
		  }
	  }

	  std::visit(overloaded {
			  [&os](std::monostate arg) { os << "monostate\n"; },
			  [&os](const std::string &arg) { os << arg << '\n'; },
			  [&os](const Json::Document& arg) { Json::Print(arg, os); }
	  }, response->body);
  }

}//!namespace

