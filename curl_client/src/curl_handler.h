//
// Created by Andrey Solovyev on 27/09/2021.
//

#pragma once

#include <curl/curl.h>

#include "query.hpp"
#include "header.hpp"
#include "response.hpp"
#include "json.h"
#include "const_curl_codes.h"
#include "const_values.h"

#include <array>
#include <cstring>

namespace curl_client {

  class CurlHandler {
  public:
	  CurlHandler();
	  ~CurlHandler();
	  void setHeaders(http::headers headers = {});
	  void setHeader (http::header header);
	  void removeHeader (http::header::field_type const& name);
	  void setUrl (const std::string &url);
	  void setMethod(Method method);
	  void setQuery(http::query_data_ptr&& query);
	  void setVerbose(bool verbose);
	  void performRequest ();

	  http::headers const& getRequestHeaders() const;
	  http::header const& getRequestHeaderAt(http::header::field_type const& name) const;
	  std::string getResponseBody ();
	  ResponseType getResponseType() const;
	  std::vector<std::string> getResponseHeaders ();
	  const std::string& getResponseCode () const;
	  bool checkRequestOk () const;
	  std::string getError ();

  private:
	  //curl data
	  CURL* curl_handle;
	  struct curl_slist *headers_list = nullptr;

	  http::headers headers;

	  std::array<char, CURL_ERROR_SIZE> buf_error;
	  std::string buf_response;
	  std::vector<std::string> buf_headers;
	  CURLcode request_return_code;

	  void Init();
	  void clean_up();
	  void clear_query();

	  //curl callbacks, must be statics because of curl
	  static size_t curl_read(char *buffer, size_t size, size_t nmemb, void *userdata);
	  static size_t curl_write(char *data, size_t size, size_t nmemb, std::string *buffer);
	  static size_t curl_header(char *buffer, size_t size, size_t nitems, void *userdata);

	  //method
	  Method method;
	  [[nodiscard]] bool isSetMethod(Method new_method) const;
	  void updateMethod(Method method);
	  static const std::unordered_map <Method, void (CurlHandler::*)()> set_method;
	  void setNone();
	  void setGet();
	  void setPost();
	  void setPut();
	  void setDelete();
	  void setPatch();
	  void setFTPListOnly();

	  //query
	  http::query_data_ptr buf_query;
	  void setNewQuery(std::string&& q);
	  static size_t getCurrQuerySize(http::query_data *p_q);

	  //response
	  ResponseType response_mime_type;
	  void setResponseType ();
	  [[nodiscard]] bool isSetResponseType () const;
  };

	using Curl = std::unique_ptr<CurlHandler>;
}
