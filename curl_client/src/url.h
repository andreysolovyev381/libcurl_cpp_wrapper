//
// Created by Andrey Solovyev on 22/09/2021.
//

#pragma once

#include "str_utils.hpp"
#include "shared_from_this.hpp"

#include <vector>
#include <string>
#include <string_view>
#include <memory>

namespace curl_client {

	class url final : public ::utility::shared_from_this<url> {
	public:
		url();

		url(std::string path);

		void push_back(std::string elem);

		void pop_back();

		void clear();

		std::string_view back() const;

		std::string_view front() const;

		std::string_view at(std::size_t idx) const;

		std::string& operator[](std::size_t idx);

		std::string const& to_string();

		bool empty() const;

		std::size_t size() const;

	private:
		std::vector<std::string> elems;

		std::string full_str;

	};

	using URLptr = std::shared_ptr<url>;

}//!namespace

