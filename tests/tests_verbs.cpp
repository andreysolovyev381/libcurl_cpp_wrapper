//
// Created by Andrey Solovyev on 26/09/2021.
//

#include <gtest/gtest.h>
#include "curl_client/curl_client.h"

TEST(verbs, get) {
	std::string const test_url {"https://httpbin.org/get"};
	curl_client::request request;
	curl_client::response response = request.
			setURL(test_url)->
			implement(curl_client::Method::Get);

	ASSERT_TRUE(response->response_code == "CURLE_OK, ");
	bool contains_OK {false};
	for (auto const& h: response->headers){
		if (h.find("200") != std::string::npos) {
			contains_OK = true;
			break;
		}
	}
	ASSERT_TRUE(contains_OK);
}

TEST(verbs, put_) {
	std::string const test_url {"https://httpbin.org/put"};
	curl_client::request request;
	curl_client::response response = request.
			setURL(test_url)->
			implement(curl_client::Method::Put);

	ASSERT_TRUE(response->response_code == "CURLE_OK, ");
	bool contains_OK {false};
	for (auto const& h: response->headers){
		if (h.find("200") != std::string::npos) {
			contains_OK = true;
			break;
		}
	}
	ASSERT_TRUE(contains_OK);
}

TEST(verbs, post) {
	std::string const test_url {"https://httpbin.org/post"};
	curl_client::request request;
	curl_client::response response = request.
			setURL(test_url)->
			implement(curl_client::Method::Post);

	ASSERT_TRUE(response->response_code == "CURLE_OK, ");
	bool contains_OK {false};
	for (auto const& h: response->headers){
		if (h.find("200") != std::string::npos) {
			contains_OK = true;
			break;
		}
	}
	ASSERT_TRUE(contains_OK);
}

TEST(verbs, delete_) {
	std::string const test_url {"https://httpbin.org/delete"};
	curl_client::request request;
	curl_client::response response = request.
			setURL(test_url)->
			implement(curl_client::Method::Delete);

	ASSERT_TRUE(response->response_code == "CURLE_OK, ");
	bool contains_OK {false};
	for (auto const& h: response->headers){
		if (h.find("200") != std::string::npos) {
			contains_OK = true;
			break;
		}
	}
	ASSERT_TRUE(contains_OK);
}

TEST(verbs, patch) {
	std::string const test_url {"https://httpbin.org/patch"};
	curl_client::request request;
	curl_client::response response = request.
			setURL(test_url)->
			implement(curl_client::Method::Patch);

	ASSERT_TRUE(response->response_code == "CURLE_OK, ");
	bool contains_OK {false};
	for (auto const& h: response->headers){
		if (h.find("200") != std::string::npos) {
			contains_OK = true;
			break;
		}
	}
	ASSERT_TRUE(contains_OK);
}
