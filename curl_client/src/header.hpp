//
// Created by Andrey Solovyev on 26/09/2021.
//

#pragma once

#include "str_utils.hpp"
#include "const_values.h"

#include <memory>
#include <ostream>

namespace curl_client {
	namespace http {
	/*
  HTTP/1.1 200 OK
  Age: 348634
  Cache-Control: max-age=604800
  Content-Type: text/html; charset=UTF-8
  Date: Mon, 27 Sep 2021 22:28:55 GMT
  Etag: "3147526947+ident"
  Expires: Mon, 04 Oct 2021 22:28:55 GMT
  Last-Modified: Thu, 17 Oct 2019 07:18:26 GMT
  Server: ECS (dcb/7F16)
  Vary: Accept-Encoding
  X-Cache: HIT
  Content-Length: 1256
	 */

	class header final {
	public:
		using field_type = std::string;
		using value_type = std::string;

		header() = default;

		header(field_type f, value_type v)
		: field_(std::move(f))
		, value_(std::move(v))
		, curl_header_(utility::concat(const_values::header_delim, field_, value_))
		{}

		header(std::string const& curl_format)
		{
			auto split {utility::split_str_to_strings<utility::split_mode::SkipEmpties>(curl_format, const_values::header_delim)};
			if (split.size() != 2u) {
				return;
			}
			field_ = std::move(split[0]);
			value_ = std::move(split[1]);
			curl_header_ = utility::concat(const_values::header_delim, field_, value_);
		}

		std::string const& field() const {
			return field_;
		}

		std::string const& value() const {
			return value_;
		}

		const char* curl_header() const {
			return curl_header_.c_str();
		}

	private:

		field_type field_;
		value_type value_;
		std::string curl_header_;

		bool empty () const {
			return curl_header_.empty();
		}
	};

	static inline const header empty_header;

	inline
	bool operator==(header const& lhs, header const& rhs){
		return lhs.field() == rhs.field() && lhs.value() == rhs.value();
	}

	inline
	std::ostream& operator<<(std::ostream& output, const header& h) {
		return output << h.curl_header();
	}
	
	//it must be a map, as curl headers_slist uses char* for "data" field
	using headers = std::map<header::field_type, header>;

	}//!namespace curl_client::http
}//!namespace curl_client

