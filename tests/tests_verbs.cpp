//
// Created by Andrey Solovyev on 26/09/2021.
//

#include <gtest/gtest.h>
#include "curl_client/curl_client.h"

auto make_request_verbs = [](curl_client::Method method, std::string const& test_url){
	curl_client::request request;
	curl_client::response response = request.
			setURL(test_url)->
			implement(method);

	return response;
};

inline
auto check_response_ok = [](curl_client::response&& response){
	ASSERT_TRUE(response->response_code == "CURLE_OK, ");
	bool contains_OK {false};
	for (auto const& h: response->headers){
		if (h.find("200") != std::string::npos) {
			contains_OK = true;
			break;
		}
	}
	ASSERT_TRUE(contains_OK);
};

TEST(verbs, get) {
	std::string const test_url {"https://httpbin.org/get"};
	curl_client::response response {make_request_verbs(curl_client::Method::Get, test_url)};
	check_response_ok(std::move(response));
}

TEST(verbs, post) {
	std::string const test_url {"https://httpbin.org/post"};
	curl_client::response response {make_request_verbs(curl_client::Method::Post, test_url)};
	check_response_ok(std::move(response));
}

TEST(verbs, put) {
	std::string const test_url {"https://httpbin.org/put"};
	curl_client::response response {make_request_verbs(curl_client::Method::Put, test_url)};
	check_response_ok(std::move(response));
}

TEST(verbs, delete_) {
	std::string const test_url {"https://httpbin.org/delete"};
	curl_client::response response {make_request_verbs(curl_client::Method::Delete, test_url)};
	check_response_ok(std::move(response));
}

TEST(verbs, patch) {
	std::string const test_url {"https://httpbin.org/patch"};
	curl_client::response response {make_request_verbs(curl_client::Method::Patch, test_url)};
	check_response_ok(std::move(response));
}
