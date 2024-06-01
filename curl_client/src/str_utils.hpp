//
// Created by Andrey Solovyev on 06.11.2023.
//

#pragma once

#include <concepts>
#include <type_traits>

#include <vector>
#include <string>
#include <string_view>
#include <array>

#include <optional>
#include <cstddef>
#include <charconv>

#include <stdexcept>
#include <system_error>

namespace utility {

	namespace const_values {
		static constexpr std::size_t expectedWordCount {100u};
	}

	//see appropriate cppref
	inline bool my_ispunct(char ch){
		return (std::ispunct(static_cast<unsigned char>(ch)));
	}

	inline bool my_isalpha(char ch){
		return std::isalpha(static_cast<unsigned char>(ch));
	}

	inline bool my_isspace(char ch){
		return std::isspace(static_cast<unsigned char>(ch));
	}

	inline char my_tolower(char ch){
		return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
	}

	inline bool my_isalnum(char ch){
		return std::isalnum(static_cast<unsigned char>(ch));
	}

	inline bool my_isdigit(char ch) {
		return std::isdigit(static_cast<unsigned char>(ch));
	}

	template <typename Func>
	auto modify_string(const std::string &source, Func f) {
		std::string result;
		size_t size = source.size();
		result.resize(size);
		for (size_t i = 0; i != size; ++i) {
			result[i] = f(source[i]);
		}
		return result;
	}

	template <typename Func>
	auto modify_string(std::string &&source, Func f) {
		size_t size = source.size();
		for (size_t i = 0; i != size; ++i) {
			source[i] = f(source[i]);
		}
		return source;
	}


	inline
	std::string_view unquote(std::string_view value) {
		if (!value.empty() && value.front() == '\"') {
			value.remove_prefix(1);
		}
		if (!value.empty() && value.back() == '\"') {
			value.remove_suffix(1);
		}
		return value;
	}

	inline
	std::string_view strip(std::string_view s) {
		while (!s.empty() && my_isspace(s.front())) {
			s.remove_prefix(1);
		}
		while (!s.empty() && my_isspace(s.back())) {
			s.remove_suffix(1);
		}
		return s;
	}

	namespace split_mode {
		struct IncludeEmpties {};
		struct SkipEmpties {};

		template <typename SplitMode>
		concept IsMode = std::same_as<SplitMode, IncludeEmpties> || std::same_as<SplitMode, SkipEmpties>;

		template <IsMode SplitMode>
		static constexpr inline bool is_include_empties_v{requires {requires IsMode<IncludeEmpties>;}};
	}//!namespace split_mode


	template <split_mode::IsMode SplitMode>
	std::vector<std::string_view> split_str_to_views(std::string const& line, char delim) {
		constexpr bool include_empty {split_mode::is_include_empties_v<SplitMode>};
		std::string_view str(line);
		std::vector<std::string_view> result;
		result.reserve(const_values::expectedWordCount); //arbitrary figure
		std::size_t found_delim{0};
		while (true) {
			found_delim = str.find(delim);
			auto substr = str.substr(0, found_delim);
			if (include_empty || !substr.empty()) {
				result.push_back(substr);
			}
			if (found_delim == str.npos) break;
			else str.remove_prefix(found_delim + 1);
		} //!while
		return result;
	}

	template <split_mode::IsMode SplitMode>
	std::vector<std::string> split_str_to_strings(const std::string &line, char delim) {
		constexpr bool include_empty {split_mode::is_include_empties_v<SplitMode>};
		if (line.empty()) {
			return {};
		}
		std::vector<std::string> result;
		result.reserve(const_values::expectedWordCount); //arbitrary figure
		std::size_t
				current_pos = 0,
				line_size = line.size();
		while (true) {
			std::size_t delim_pos = line.find(delim, current_pos);

			if (delim_pos == line.npos) {
				if (line.back() == '\r') { --line_size; } //fucking windows
				delim_pos = line_size;
			}
			int substr_length = delim_pos - current_pos;
			if (substr_length < 0) {
				throw std::runtime_error("substr length is less than zero: " + std::string(__PRETTY_FUNCTION__));
			}
			auto sub = line.substr(current_pos, substr_length);
			if (include_empty || !sub.empty()) {
				result.push_back(std::move(sub));
			}
			if (delim_pos == line_size) {
				break;
			} else {
				current_pos = ++delim_pos;
			}
		} //!while
		return result;
	} //!func

	template <split_mode::IsMode SplitMode>
	std::vector<std::string_view> split_view_to_views(std::string_view s, char sep) {
		constexpr bool include_empty {split_mode::is_include_empties_v<SplitMode>};
		std::vector<std::string_view> result;
		while (!s.empty()) {
			size_t pos = s.find(sep);
			auto substr = s.substr(0, pos);
			if (include_empty || !substr.empty()) {
				result.push_back(substr);
			}
			s.remove_prefix(pos != s.npos ? pos + 1 : s.size());
		}
		return result;
	}

	inline
	std::string replace_substr (const std::string &src, const std::string &what, const std::string & with) {
		if (src.empty() || what.empty()) return src;

		std::size_t
				size {src.size()},
				cur_pos {0u};

		std::string output;
		output.reserve(size);

		while (true) {
			std::size_t pos = src.find(what, cur_pos);
			if (pos == src.npos) {
				pos = size;
			}
			int substr_length = pos - cur_pos;
			if (substr_length < 0){
				throw std::runtime_error("negative length of a substr; " + std::string(__PRETTY_FUNCTION__ ));
			}
			output += src.substr(cur_pos, substr_length);
			if (pos != size) {
				output += with;
			}
			cur_pos = pos + what.size();
			if (pos == size || cur_pos >= size){
				break;
			}
		}
		return output;
	}

	template <typename... Sources>
	requires (std::convertible_to<Sources, std::string> && ...)
	auto concat(char delim, Sources&&... sources){
		std::string result;
		((result += (
				std::is_same_v<std::decay_t<decltype(sources)>, std::string> ?
		        std::forward<Sources>(sources) :
				std::string(std::forward<Sources>(sources))) + delim)
						, ...);
		result.pop_back();
		return result;
	}

	template <typename Number>
	requires std::integral<Number> || std::floating_point<Number>
	std::optional<Number> from_chars(std::string_view str) {
		Number local_result;
		auto const last = str.data() + str.size();

		auto [ptr, ec]{std::from_chars(str.data(), last, local_result)};

		if (ec == std::errc::invalid_argument ||
		    ec == std::errc::result_out_of_range ||
		    ptr != last) {
			return std::nullopt;
		}
		return {local_result};
		//	  throw std::invalid_argument("Attempt to convert not a number; ");
		//	  throw std::invalid_argument("Out of bound; ");
	}

	template <typename T>
	std::string to_string(const T &value) {
		std::array<char, 64> data;
		auto [p, ec] = std::to_chars(data.data(), data.data() + data.size(), value);
		return std::string(data.data(), p - data.data());
	}

	template <>
	inline std::string to_string(const bool &value) {
		if (value) {
			return "true";
		}
		return "false";
	}

	template <>
	inline std::string to_string(const std::string_view &value) {
		return std::string(value.data(), value.size());
	}


}//!namespace