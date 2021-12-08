//
// Created by Andrey Solovyev on 27/09/2021.
//

#pragma once

#include <curl/curl.h>

#include "data_structures.h"
#include "json.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <variant>
#include <memory>

#ifndef CURL_HANDLER_H
#define CURL_HANDLER_H

namespace curl_client {

  class CurlHandler {
  public:
	  CurlHandler();
	  ~CurlHandler();
	  void setHeaders(const HttpHeaders &headers = {});
	  void setHeader (HttpHeader header);
	  void removeHeader (HeaderName name);
	  void setUrl (const std::string &url);
	  void setMethod(Method method);
	  void setQuery(Query&& query);
	  void setVerbose(bool verbose);
	  void performRequest ();

	  CURL* getCurlHandler() const;
	  const HttpHeaders& getRequestHeaders() const;
	  const HttpHeader& getRequestHeaderAt(HeaderName name) const;
	  std::string getResponseBody ();
	  ResponseType getResponseType() const;
	  std::vector<std::string> getResponseHeaders ();
	  const std::string& getResponseCode () const;
	  bool checkRequestOk () const;
	  std::string getError ();

  private:
	  //curl data
	  CURL* curl_handle;
	  struct curl_slist *headers_list = NULL;
	  HttpHeaders headers;
	  HttpHeader empty_header;

	  std::string buf_response, buf_error;
	  std::vector<std::string> buf_headers;
	  CURLcode request_return_code;

	  void Init();
	  void clearBuffers();
	  void clearQuery();

	  //curl callbacks, must be statics because of curl
	  static size_t read_callback(char *buffer, size_t size, size_t nmemb, void *userdata);
	  static size_t curl_write_func(char *data, size_t size, size_t nmemb, std::string *buffer);
	  static size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata);

	  //method
	  Method method;
	  [[nodiscard]] bool isSetMethod(Method new_method) const;
	  void updateMethod(const Method method);
	  static const std::unordered_map <Method, void (CurlHandler::*)()> set_method;
	  void setNone();
	  void setGet();
	  void setPost();
	  void setPut();
	  void setDelete();
	  void setPatch();

	  //query
	  Query buf_query;
	  void setNewQuery(std::string&& q);
	  static bool hasDataToCopy(QueryData *p_q);
	  static size_t getCurrQuerySize(QueryData *p_q);
	  static void copyDataToBuffer(char* buffer, size_t buffer_size, QueryData *q);
	  static size_t getCopiedCount(QueryData *p_q);

	  //response
	  ResponseType response_mime_type;
	  void setResponseType ();
	  [[nodiscard]] bool isSetResponseType () const;
  };

	using Curl = std::unique_ptr<CurlHandler>;
}

#endif //CURL_HANDLER_H
