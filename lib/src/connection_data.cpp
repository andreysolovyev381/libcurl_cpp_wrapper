//
// Created by Andrey Solovyev on 27/09/2021.
//

#include "connection_data.h"
#include <stdexcept>

namespace curl_client {

  double ConnectionData::getConnectionTime (const std::string &url, int iterations_qty) {
  	curl->setUrl(url);

  	double total_time = 0.0;
  	int attempt_count = 0;
  	for (int i = 0; i != iterations_qty; ++i) {
  		auto attemp = Ping(url);
  		if (attemp.has_value()) {
  			total_time += attemp.value();
  			++attempt_count;
  		}
  	}
  	return total_time / attempt_count;
  }

  std::optional<double> ConnectionData::Ping (const std::string &url) {
  	double total;
  	curl->performRequest();
  	if (curl->checkRequestOk()) {
  		auto time = curl_easy_getinfo(curl->getCurlHandler(), CURLINFO_TOTAL_TIME, &total);
  		if(CURLE_OK == time) {
  			return total;
  		}
  	}
  	return std::nullopt;
  }

}//!namespace