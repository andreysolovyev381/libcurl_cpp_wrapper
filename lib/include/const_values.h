//
// Created by Andrey Solovyev on 26/09/2021.
//

#pragma once

#include <map>
#include <string>

#ifndef CURL_CLIENT_CONST_VALUES_H
#define CURL_CLIENT_CONST_VALUES_H

namespace curl_client {

  static const size_t EXPECTED_REQUEST_SIZE = 1048576; //1 Mb
  static const size_t EXPECTED_HEADERS_RESPONSE_COUNT = 10;
  static const size_t EXPECTED_PATH_ELEMENTS_COUNT = 5;
  static const size_t EXPECTED_WORDS_COUNT = 5;
  static const size_t EXPECTED_FULL_PATH_LENGTH = 50;

  static const std::string NO_PATH_MSG {"Path hasn't been set"};
  static const std::string NO_CURL_HANDLE_MSG {"Curl handle hasn't been set"};

  static const std::map<const std::string, std::string> headers_by_default {
		  {"Accept", "application/json"},
		  {"Connection", "Keep-Alive"},
		  {"Keep-Alive", "timeout=5, max=1000"},
		  {"Content-Encoding", "gzip"},
		  {"Content-Type", "text/html; charset=utf-8"},
  	};

}

#endif //CURL_CLIENT_CONST_VALUES_H
