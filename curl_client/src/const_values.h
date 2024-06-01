//
// Created by Andrey Solovyev on 28.02.2024.
//

#pragma once

#include <string>
#include <cstddef>
#include <map>

namespace curl_client::const_values {

	using namespace std::string_literals;

	static inline constexpr std::size_t EXPECTED_REQUEST_SIZE {1 << 20};
	static inline constexpr std::size_t EXPECTED_HEADERS_RESPONSE_COUNT {10};
	static inline constexpr std::size_t EXPECTED_WORDS_COUNT {5};
	static inline constexpr std::size_t EXPECTED_FULL_PATH_LENGTH {50};

	static inline const std::string NO_PATH_MSG {"Path hasn't been set"};
	static inline const std::string NO_CURL_HANDLE_MSG {"Curl handle hasn't been set"};
	static inline const std::string NO_REQUEST_MADE {"NO_CURL"};

	static inline const std::map<std::string, std::string> headers_by_default {
			{"Accept", "application/json"},
			{"Connection", "Keep-Alive"},
			{"Keep-Alive", "timeout=5, max=1000"},
			{"Content-Encoding", "gzip"},
			{"Content-Type", "text/html; charset=utf-8"},
	};

	static inline constexpr char url_delim {'/'};
	static inline constexpr char header_delim {':'};

	static inline constexpr std::size_t url_elem_count {10u};

}//!namespace
