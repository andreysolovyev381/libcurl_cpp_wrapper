//
// Created by Andrey Solovyev on 26/09/2021.
//

#include <gtest/gtest.h>
#include "curl_client/curl_client.h"

TEST(headers, default_values) {
	curl_client::request request;

	auto default_headers_copy = curl_client::const_values::headers_by_default;

	for (auto const&[name, header] : request.get_headers()) {
		auto found = default_headers_copy.find(std::string(name));
		ASSERT_TRUE(found != default_headers_copy.end());
		default_headers_copy.erase(found);
	}
	ASSERT_TRUE(default_headers_copy.empty());
}

TEST(headers, headers_sent) {
	//this url echoes request headers as a response body
	std::string const test_url {"https://httpbin.org/headers"};
	curl_client::request request;

	curl_client::response response = request.
			setURL(test_url)->
			implement(curl_client::Method::Get);

	auto const& txt {std::get<std::string>(response->body)};

	for (auto const&[_, header] : request.get_headers()) {
		if (header.field() == "Connection" || header.field() == "Keep-Alive") { //todo somehow it doesn't come back
			continue;
		}
		ASSERT_TRUE(txt.find(header.field()) != std::string::npos);
		ASSERT_TRUE(txt.find(header.value()) != std::string::npos);
	}
}


TEST(headers, user_defined_header) {
	//this url echoes request headers as a response body
	std::string const test_url {"https://httpbin.org/headers"};
	curl_client::request request;

	curl_client::response response = request.
			setURL(test_url)->
			set_header(curl_client::http::header("Some Arbitrary Field", "Some Arbitrary Value of 42"))->
			implement(curl_client::Method::Get);

	auto const& txt {std::get<std::string>(response->body)}; std::cout << txt << '\n';

	ASSERT_TRUE(txt.find("Some Arbitrary Field") != std::string::npos);
	ASSERT_TRUE(txt.find("Some Arbitrary Value of 42") != std::string::npos);
}
