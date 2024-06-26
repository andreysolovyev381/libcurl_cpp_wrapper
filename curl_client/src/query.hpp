//
// Created by Andrey Solovyev on 26/09/2021.
//

#pragma once

#include <string>
#include <memory>
#include <utility>
#include <cstdint>

namespace curl_client {

	enum class Method : std::uint8_t {
		None =          0,
		Get =           1 << 1,
		Post =          1 << 2,
		Put =           1 << 3,
		Delete =        1 << 4,
		Patch =         1 << 5,
		FTPListOnly =   1 << 6,
	};

	namespace http {

		struct query_data final {

			std::size_t sizeleft;

			std::size_t curr_offset;

			std::size_t curr_chunk_size;

			std::string query_str;

			explicit query_data(std::string q)
					: sizeleft(q.size())
					, curr_offset(0)
					, curr_chunk_size(0)
					, query_str(std::move(q)){}

			query_data() = delete;
		};

		using query_data_ptr = std::unique_ptr<query_data>;

	}//!namespace curl_client::http

}//!namespace curl_client
