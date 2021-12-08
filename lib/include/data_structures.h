//
// Created by Andrey Solovyev on 26/09/2021.
//

#pragma once

#include "json.h"

#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <utility>
#include <ostream>
#include <map>

#ifndef CURL_CLIENT_DATA_STRUCTURES_H
#define CURL_CLIENT_DATA_STRUCTURES_H

namespace curl_client {
  enum class Method {
	  None,
	  Get,
	  Post,
	  Put,
	  Delete,
	  Patch
  };

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

  struct HttpHeader {
	  std::string name, value;
	  bool isEmpty () const;
	  std::string operator () () const;
  };

  /*
HTTP/1.1 200 OK
Age: 348634
Cache-Control: max-age=604800
Content-Type: text/html; charset=UTF-8
Date: Mon, 27 Sep 2021 22:28:55 GMT
Etag: "3147526947+ident"
Expires: Mon, 04 Oct 2021 22:28:55 GMT
Last-Modified: Thu, 17 Oct 2019 07:18:26 GMT
Server: ECS (dcb/7F16)
Vary: Accept-Encoding
X-Cache: HIT
Content-Length: 1256
   */

  using HeaderName = std::string_view;
  using HttpHeaders = std::map<HeaderName, HttpHeader>; //map to avoid invalidation as a result of rehashing

  std::ostream& operator<<(std::ostream& output, const HttpHeader& h);
  bool operator==(const HttpHeader& lhs, const HttpHeader& rhs);

  struct ResponseData {
	  using Headers = std::vector<std::string>;
	  using Body = std::variant<std::monostate, Json::Document, std::string>;

	  bool request_ok;
	  std::string response_code;
	  Body body;
	  Headers headers;
  };

  using Response = std::unique_ptr<ResponseData>;

  struct QueryData {
	  explicit QueryData(std::string&& q);
	  QueryData() = delete;
	  size_t sizeleft, curr_offset, curr_chunk_size;
	  std::string query_str;
  };

  using Query = std::unique_ptr<QueryData>;

  struct PathData {
	  PathData();
	  PathData(std::string &&path);

	  const char delim = '/';
	  std::vector<std::string> elems;
	  bool is_update_required;
	  std::string full_str;

	  void Stringify();
  };

  using Path = std::unique_ptr<PathData>;



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

#endif //CURL_CLIENT_DATA_STRUCTURES_H
