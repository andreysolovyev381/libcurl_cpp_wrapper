//
// Created by Andrey Solovyev on 26/09/2021.
//

#pragma once

#include "json.h"
#include "header.hpp"

#include <vector>

namespace curl_client {

	enum class ResponseType {
		None,
		Text,
		Json,
		Html,
		Xml,
		Pdf,
		Jpeg,
		Jpg,
		Gif,
		CurlError,
	};

	struct response_data final {

		using Headers = std::vector<std::string>;

		using Body = std::variant<std::monostate, Json::Document, std::string>;

		bool request_ok;

		std::string response_code;

		Body body;

		Headers headers;
	};

	using response = std::unique_ptr<response_data>;


	/*

	struct ParsedResponse {
		int code;
		vector<HttpHeader> headers;
		string content;
	};

	istream& operator >>(istream& input, ParsedResponse& r) {
		string line;
		getline(input, line);

		{
			istringstream code_input(line);
			string dummy;
			code_input >> dummy >> r.code;
		}

		size_t content_length = 0;

		r.headers.clear();
		while (getline(input, line) && !line.empty()) {
			if (auto [name, value] = SplitBy(line, ": "); name == "Content-Length") {
				istringstream length_input(value);
				length_input >> content_length;
			} else {
				r.headers.push_back( {std::move(name), std::move(value)});
			}
		}

		r.content.resize(content_length);
		input.read(r.content.data(), r.content.size());
		return input;
	}
  */

}
