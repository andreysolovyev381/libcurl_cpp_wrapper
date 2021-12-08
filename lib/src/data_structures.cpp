//
// Created by Andrey Solovyev on 27/09/2021.
//

#include "const_values.h"
#include "data_structures.h"
#include "utils.h"


namespace curl_client {

	  bool HttpHeader::isEmpty () const {
	  	return name.empty() && value.empty();
	  }
	  std::string HttpHeader::operator () () const {
		  std::string output;
		  output.reserve(name.size() + value.size() + 1);
		  output += name;
		  output += ':';
		  output += value;
		  return output;
	  }


  std::ostream& operator<<(std::ostream& output, const HttpHeader& h) {
	  return output << h.name << ": " << h.value;
  }

  bool operator==(const HttpHeader& lhs, const HttpHeader& rhs) {
	  return lhs.name == rhs.name && lhs.value == rhs.value;
  }


  QueryData::QueryData (std::string&& q)
		  : sizeleft(q.size())
		  , curr_offset(0)
		  , curr_chunk_size(0)
		  , query_str (std::move(q))
  {}

  PathData::PathData() : is_update_required(false) {
	  elems.reserve(EXPECTED_PATH_ELEMENTS_COUNT);
  }
  PathData::PathData(std::string &&path) : is_update_required(false) {
	  elems.reserve(EXPECTED_PATH_ELEMENTS_COUNT);
	  elems = std::move(utils::splitIntoWords(path, delim)); //todo: check how NRVO works
  }

  void PathData::Stringify(){
	  full_str.reserve(EXPECTED_PATH_ELEMENTS_COUNT);
	  for (const auto &elem : elems){
		  full_str += elem;
		  full_str += delim;
	  }
//  	full_str.pop_back(); //remove last '/' - delim
	  is_update_required = false;
  }


}//!namespace
