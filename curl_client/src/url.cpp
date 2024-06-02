//
// Created by Andrey Solovyev on 22/09/2021.
//

#include "url.h"
#include "const_values.h"

namespace curl_client {

	url::url() {
		elems.reserve(curl_client::const_values::url_elem_count);
	}

	url::url(std::string path) {
		using namespace utility;
		elems = split_str_to_strings<split_mode::IncludeEmpties>(path, curl_client::const_values::url_delim);
	}

	void url::push_back(std::string elem) {
		using namespace utility;
		auto new_elem {split_str_to_strings<split_mode::SkipEmpties>(elem, curl_client::const_values::url_delim)};
		elems.insert(elems.end(), std::move_iterator(new_elem.begin()), std::move_iterator(new_elem.end()));
	}

	void url::pop_back() {
		elems.pop_back();
	}

	void url::clear() {
		elems.clear();
		full_str.clear();
	}

	std::string_view url::back() const {
		return elems.back();
	}

	std::string_view url::front() const {
		return elems.back();
	}

	std::string_view url::at(size_t idx) const {
		if (idx > elems.size()) {
			throw std::invalid_argument(
					"Out of boundaries, required idx is " + std::to_string(idx) +
					",\nwhile path elem count is " + std::to_string(elems.size()) +
					std::string(__PRETTY_FUNCTION__));
		}
		return elems.at(idx);
	}

	std::string& url::operator[](std::size_t idx){
		return elems[idx];
	}

	std::string const& url::to_string() {
		full_str.clear();
		full_str.reserve(std::max(full_str.capacity(), curl_client::const_values::url_elem_count));
		for (const auto &elem : elems){
			full_str += elem;
			full_str += curl_client::const_values::url_delim;
		}
		if (!full_str.empty() && full_str.back() == '/') {
			full_str.pop_back(); //remove last '/' - delim
		}
		return full_str;
	}

	bool url::empty() const {
		return elems.empty();
	}

	std::size_t url::size() const {
		return elems.size();
	}

}//!namespace

