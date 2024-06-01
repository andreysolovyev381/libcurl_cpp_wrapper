//
// Created by Andrey Solovyev on 22/09/2021.
//

#pragma once

#include "query.hpp"
#include "response.hpp"
#include "url.h"
#include "json.h"
#include "curl_handler.h"

namespace curl_client {

	class request final {
	public:
		request();
		response implement (Method method, http::query_data_ptr&& query = nullptr);
		request* set_verbose (bool verbose);

		request* setURL(std::string path_);
		std::string const& getURL() const;
		URLptr manageURL() const;

		request* set_headers (http::headers headers);
		request* set_header (http::header header);
		http::headers const& get_headers () const;
		http::header const& get_header(http::header::field_type name) const;

		request* set_query (std::string input) ;

	private:
		//curl
		Curl curl;

		//url
		URLptr url_;

		response response_;
		//handlers
		static const std::unordered_map <ResponseType, void (request::*)()> set_response_body;
		void returnNone();
		void returnText();
		void returnJson();
		void returnError();
	};

	void Print (const response &response, std::ostream &os, bool print_headers = false);

}//!namespace

