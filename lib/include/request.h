//
// Created by Andrey Solovyev on 22/09/2021.
//

#pragma once

#include "data_structures.h"
#include "const_values.h"
#include "json.h"
#include "curl_handler.h"

#include <unordered_map>
#include <string>
#include <ostream>
#include <memory>

#ifndef CURL_REQUEST_H
#define CURL_REQUEST_H

namespace curl_client {

  class Request {
  public:
	  Request();
	  Response Implement (Method method, Query&& query = nullptr);
	  Request* SetVerbose (bool verbose);

	  Request* pathSetNew(std::string&& path);
	  bool pathIsEmpty() const;
	  void pathSetEmpty();
	  const std::string& pathGetStringified();
	  size_t pathGetElemCount() const;
	  Request* pathPushBackElement(std::string&& elem);
	  Request* pathPopBackElement();
	  const std::string& pathGetBackElement() const;
	  const std::string& pathAt(size_t idx) const;
	  std::string& pathAt(size_t idx);

	  Request* headersSetAll (HttpHeaders headers);
	  Request* headerSetSingle (HttpHeader header);
	  const HttpHeaders& headersGetAll () const;
	  const HttpHeader& headerGetOne(HeaderName name) const;

	  static Query makeQuery (std::string &&input) ;

  private:
	  //curl
	  Curl curl;

	  //path
	  Path path;

	  Response response;
	  //handlers
	  static const std::unordered_map <ResponseType, void (Request::*)()> set_response_body;
	  void returnNone();
	  void returnText();
	  void returnJson();
	  void returnError();
  };


  //todo: move it from here to utils

  // helper type for the visitor #4
  template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
  // explicit deduction guide (not needed as of C++20)
  template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

  void Print (const Response &response, std::ostream &os, bool print_headers = false);

}//!namespace


#endif //CURL_REQUEST_H
