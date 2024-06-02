//
// Created by Andrey Solovyev on 26/09/2021.
//

#include <gtest/gtest.h>
#include "curl_client/curl_client.h"

TEST(example, basic_one) {
	std::string const test_url {"https://httpbin.org/anything"};
	std::string query_data {"Some arbitrary payload data"};

	curl_client::request request;
	curl_client::response response = request.
			set_query(std::move(query_data))->
			setURL(test_url)->
			set_header(curl_client::http::header("Accept", "text"))->
			implement(curl_client::Method::Post);

	bool const print_headers {true};
	curl_client::Print(response, std::cout, print_headers);

	ASSERT_TRUE(true);
}

TEST(example, manage_url) {
	std::string const test_url {"https://httpbin.org"};

	curl_client::request request;
	request.setURL(test_url);
	auto url = request.manageURL();
	url->push_back("anything");

	ASSERT_EQ(request.getURL(), "https://httpbin.org/anything");

	curl_client::response response = request.
			implement(curl_client::Method::Get);

	bool ok = response.operator bool();
	ok = ok && std::holds_alternative<std::string>(response->body);
	ok = ok && !std::get<std::string>(response->body).empty();

	ASSERT_TRUE(ok);
}