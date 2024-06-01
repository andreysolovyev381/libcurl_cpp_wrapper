//
// Created by Andrey Solovyev on 27/09/2021.
//

#pragma once

#include "curl_handler.h"
#include <string>
#include <optional>

#ifndef CONNECTION_DATA_H
#define CONNECTION_DATA_H

namespace curl_client {

  static
  std::unique_ptr<CurlHandler> curl = std::make_unique<CurlHandler>();

  class ConnectionData {
  public:
  	double getConnectionTime (const std::string &url, int iterations_qty = 10);
  private:
  	std::optional<double> Ping (const std::string &url);
  };


}//!namespace

#endif //CONNECTION_DATA_H
