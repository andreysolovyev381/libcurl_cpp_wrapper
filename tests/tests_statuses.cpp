//
// Created by Andrey Solovyev on 26/09/2021.
//

#include <gtest/gtest.h>
#include "curl_client/curl_client.h"

std::string const test_url {"https://httpbin.org/status/200"};

TEST(put, basic_payload) {
	curl_client::request request;
	curl_client::response response = request.
			setURL(test_url)->
			set_query("Some Payload Data")->
			implement(curl_client::Method::Post);

	curl_client::Print(response, std::cout);

	ASSERT_TRUE(true);
}

TEST(put, auth_header) {
}

TEST(put, auth_data) {
}
