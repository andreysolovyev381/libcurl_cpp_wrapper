//
// Created by Andrey Solovyev on 26/09/2021.
//

#include <gtest/gtest.h>
#include "curl_client/curl_client.h"

std::string const test_url {"https://httpbin.org/anything"};

auto make_request_payload = [](curl_client::Method method, std::string const& query_data){
	curl_client::request request;
	curl_client::response response = request.
			set_query(query_data)->
			setURL(test_url)->
			implement(method);

	return response;
};

TEST(payload, get) {
	std::string const query_data {"Using method GET with payload data"};
	curl_client::response response {make_request_payload(curl_client::Method::Get, query_data)};
	std::string response_part_1 {R"("data": ")"};
	response_part_1 += query_data;
	response_part_1 += std::string{R"(",)"};
	std::string const response_part_2 {R"("method": "GET")"};

	auto const& txt {std::get<std::string>(response->body)};
	ASSERT_TRUE(txt.find(response_part_1) != std::string::npos);
	ASSERT_TRUE(txt.find(response_part_2) != std::string::npos);
}

TEST(payload, post) {
	std::string const query_data {"Using method POST with payload data"};
	curl_client::response response {make_request_payload(curl_client::Method::Post, query_data)};

	std::string response_part_1 {R"("data": ")"};
	response_part_1 += query_data;
	response_part_1 += std::string{R"(",)"};
	std::string const response_part_2 {R"("method": "POST")"};

	auto const& txt {std::get<std::string>(response->body)};
	ASSERT_TRUE(txt.find(response_part_1) != std::string::npos);
	ASSERT_TRUE(txt.find(response_part_2) != std::string::npos);
}

TEST(payload, put) {
	std::string const query_data {"Using method PUT with payload data"};
	curl_client::response response {make_request_payload(curl_client::Method::Put, query_data)};

	std::string response_part_1 {R"("data": ")"};
	response_part_1 += query_data;
	response_part_1 += std::string{R"(",)"};
	std::string const response_part_2 {R"("method": "PUT")"};

	auto const& txt {std::get<std::string>(response->body)};
	ASSERT_TRUE(txt.find(response_part_1) != std::string::npos);
	ASSERT_TRUE(txt.find(response_part_2) != std::string::npos);
}

TEST(payload, delete_) {
	std::string const query_data {"Using method DELETE with payload data"};
	curl_client::response response {make_request_payload(curl_client::Method::Delete, query_data)};

	std::string response_part_1 {R"("data": ")"};
	response_part_1 += query_data;
	response_part_1 += std::string{R"(",)"};
	std::string const response_part_2 {R"("method": "DELETE")"};

	auto const& txt {std::get<std::string>(response->body)};
	ASSERT_TRUE(txt.find(response_part_1) != std::string::npos);
	ASSERT_TRUE(txt.find(response_part_2) != std::string::npos);
}

TEST(payload, patch) {
	std::string const query_data {"Using method PATCH with payload data"};
	curl_client::response response {make_request_payload(curl_client::Method::Patch, query_data)};

	std::string response_part_1 {R"("data": ")"};
	response_part_1 += query_data;
	response_part_1 += std::string{R"(",)"};
	std::string const response_part_2 {R"("method": "PATCH")"};

	auto const& txt {std::get<std::string>(response->body)};
	ASSERT_TRUE(txt.find(response_part_1) != std::string::npos);
	ASSERT_TRUE(txt.find(response_part_2) != std::string::npos);
}
