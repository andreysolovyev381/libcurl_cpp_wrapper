//
// Created by Andrey Solovyev on 27/09/2021.
//

#include "utils.h"
#include "const_values.h"

namespace utils {

  std::vector<std::string> splitIntoWords(const std::string &line, char delim) {
	  if (line.empty()) return {};
	  std::vector<std::string> result;
	  result.reserve(curl_client::EXPECTED_WORDS_COUNT);
	  size_t
			  current_pos = 0,
			  line_size = line.size();
	  while (true) {
		  size_t delim_pos = line.find(delim, current_pos);
		  if (delim_pos == line.npos) delim_pos = line_size;
		  int substr_length = delim_pos - current_pos;
		  if (substr_length < 0)
			  throw std::runtime_error("substr length is less than zero: " + std::string(__PRETTY_FUNCTION__));
		  auto sub = line.substr(current_pos, substr_length);
//		  if (!sub.empty()) result.push_back(std::move(sub));
		  result.push_back(std::move(sub)); //alternated this line because of http://
		  if (delim_pos == line_size) break;
		  else current_pos = ++delim_pos;
	  } //!while
	  return result;
  } //!func


}//!namespace
