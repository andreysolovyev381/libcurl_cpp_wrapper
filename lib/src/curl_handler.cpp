//
// Created by Andrey Solovyev on 27/09/2021.
//

#include "const_curl_codes.h"
#include "const_values.h"

#include "curl_handler.h"
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace curl_client {

#define THROW_CURL throw std::runtime_error("Unable to access curl "+std::string(__PRETTY_FUNCTION__));
#define DEBUG_OUPUT

  CurlHandler::CurlHandler() {
	  buf_response.reserve(EXPECTED_REQUEST_SIZE);
	  buf_error.reserve(CURL_ERROR_SIZE);
	  buf_headers.reserve(EXPECTED_HEADERS_RESPONSE_COUNT);
	  Init();
  }
  CurlHandler::~CurlHandler() {
	  if (curl_handle) curl_easy_cleanup(curl_handle);
	  /* free headers */
	  if (headers_list) curl_slist_free_all(headers_list);
  }

  void CurlHandler::setHeaders(const HttpHeaders &headers){
	  if (curl_handle) {

		  if (not headers.empty()) {
			  for (const auto &[_, header] : headers) {
				  const auto [it, ok] = this->headers.insert({header.name, header});
				  if (ok) headers_list = curl_slist_append(headers_list, it->second().data());
			  }
		  }
		  else {
			  for (const auto &[name, header] : headers_by_default) {
				  HttpHeader h {.name = name, .value = header};
				  const auto [it, ok] = this->headers.insert({h.name, std::move(h)});
				  if (ok) headers_list = curl_slist_append(headers_list, it->second().data());
			  }
		  }

		  /*
  If you use POST to a HTTP 1.1 server, you can send data without knowing
  the size before starting the POST if you use chunked encoding. You
  enable this by adding a header like "Transfer-Encoding: chunked" with
  CURLOPT_HTTPHEADER. With HTTP 1.0 or without chunked transfer, you must
  specify the size in the request.
*/
#ifdef USE_CHUNKED
		  {
  			//		  	struct curl_slist *chunk = NULL;

  			chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
  			//		  	res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
  			/* use curl_slist_free_all() after the *perform() call to free this list again */
}
#else
/* Set the expected POST size. If you want to POST large amounts of data,
   consider CURLOPT_POSTFIELDSIZE_LARGE */
//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) wt.getCurrQuerySize());
#endif

#ifdef DISABLE_EXPECT
		  /*
Using POST with HTTP 1.1 implies the use of a "Expect: 100-continue"
header.  You can disable this header with CURLOPT_HTTPHEADER as usual.
NOTE: if you want chunked transfer too, you need to combine these two
since you can only set one list of headers with CURLOPT_HTTPHEADER. */

/* A less good option would be to enforce HTTP 1.0, but that might also have other implications. */
{
	//	struct curl_slist *chunk = NULL;

	chunk = curl_slist_append(chunk, "Expect:");
	//	res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	/* use curl_slist_free_all() after the *perform() call to free this list again */
}
#endif

		  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers_list);
	  }
	  else THROW_CURL
	  if (not headers_list)
		  throw std::runtime_error("Unable to SET HEADERS\n"+std::string(__PRETTY_FUNCTION__));
  }

  void CurlHandler::setHeader (HttpHeader header) {
	  auto *h = headers_list;
	  while (h) {
		  std::string_view h_view (h->data);
		  auto pos = h_view.find(':');
		  if (pos == h_view.npos) continue;
		  if (h_view.substr(0, pos) == header.name) {
			  const auto [it, res] = this->headers.insert({header.name, std::move(header)}); //todo:: looks like it is going to fall
			  h->data = it->second().data();
			  return;
		  }
		  h = h->next;
	  }
	  const auto [it, res] = this->headers.insert({header.name, std::move(header)}); //todo:: looks like it is going to fall
	  headers_list = curl_slist_append(headers_list, it->second().data());
  }

  void CurlHandler::removeHeader (HeaderName name) {
	  if (auto it = headers.find(name); it != headers.end()) {
		  auto *curr = headers_list, *prev = headers_list;
		  while (curr) {
			  std::string_view h_view (curr->data);
			  auto pos = h_view.find(':');
			  if (pos == h_view.npos) continue;
			  if (h_view.substr(0, pos) == name) {
			  	prev->next = curr->next;
			  	delete curr;
			  	return;
			  }
			  prev = curr;
			  curr = curr->next;
		  }
		  headers.erase(it);
	  }
  }


  void CurlHandler::setUrl (const std::string &url){
	  if(curl_handle) {
		  curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	  }
	  else THROW_CURL
  }
  void CurlHandler::setMethod (Method method) {
	  if (not isSetMethod(method)) updateMethod(method);
  }
  void CurlHandler::setQuery(Query&& query) {
	  if (not query) throw std::runtime_error("No query\n"+std::string(__PRETTY_FUNCTION__));
	  buf_query = std::move(query);
	  if(curl_handle) {
		  /* pointer to pass to our read function */
		  curl_easy_setopt(curl_handle, CURLOPT_READDATA, buf_query.get());
		  /* Set the expected POST size. If you want to POST large amounts of data, consider CURLOPT_POSTFIELDSIZE_LARGE */
		  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, (long) getCurrQuerySize(buf_query.get()));
	  }
	  else THROW_CURL
  }

  void CurlHandler::setVerbose(bool verbose) {
	  if(curl_handle) {
		  /* get verbose debug output please */
		  curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, (long)verbose);
	  }
	  else THROW_CURL
  }

  void CurlHandler::performRequest () {
	  clearBuffers(); //must be BEFORE
	  if(curl_handle) {
		  request_return_code = curl_easy_perform(curl_handle);
		  setResponseType();
		  clearQuery(); //must be AFTER
	  }
	  else THROW_CURL
  }

  CURL* CurlHandler::getCurlHandler() const {
	  return curl_handle;
  }
  const HttpHeaders& CurlHandler::getRequestHeaders() const {
	  return headers;
  }
  const HttpHeader& CurlHandler::getRequestHeaderAt(HeaderName name) const {
	  if (auto it = headers.find(name); it != headers.end()) return it->second;
	  return empty_header;
  }

  std::string CurlHandler::getResponseBody () {
	  return std::move(buf_response);
  }
  ResponseType CurlHandler::getResponseType() const {
	  return response_mime_type;
  }
  std::string CurlHandler::getError(){
	  return std::move(buf_error);
  }
  std::vector<std::string> CurlHandler::getResponseHeaders () {
	  return std::move(buf_headers);
  }
  const std::string& CurlHandler::getResponseCode () const {
	  return curl_codes.at(request_return_code);
  }
  bool CurlHandler::checkRequestOk() const {
	  return request_return_code == CURLE_OK;
  }

  void CurlHandler::Init() {
	  curl_handle = curl_easy_init();
	  if (curl_handle) {

		  //ADDED HTTP/2 support
		  curl_easy_setopt(curl_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

		  //error buffer
		  curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, &buf_error);
		  // follow the "Location:" in HTTP header
		  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
		  // don't check sertificates
		  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
		  curl_easy_setopt(curl_handle, CURLOPT_TCP_KEEPALIVE, 1L);
		  /* some servers don't like requests that are made without a user-agent field, so we provide one */
		  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		  //query, if any
		  /* we want to use our own read function */
		  curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, read_callback);
		  /* pointer to pass to our read function */
		  curl_easy_setopt(curl_handle, CURLOPT_READDATA, buf_query.get()); //initially it is just a nullptr

		  // callback params
		  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &buf_response);
		  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &CurlHandler::curl_write_func);

		  //received headers
		  curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, &CurlHandler::header_callback);
		  /* pass in custom data to the callback */
		  curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &buf_headers);

		  setHeaders(); //todo: unclear why it falls down
	  }
	  else THROW_CURL
  }
  size_t CurlHandler::read_callback(char *buffer, size_t size, size_t nmemb, void *userdata){
	  if (not userdata) return 0; //avoid falling

	  auto *q_data = (class QueryData *)userdata;
	  size_t buffer_size = size*nmemb;

	  if (hasDataToCopy(q_data)) {
		  copyDataToBuffer(buffer, buffer_size, q_data);
		  return getCopiedCount(q_data); /* we copied this many bytes */
	  }
	  return 0; /* no more data left to deliver */
  }
  size_t CurlHandler::curl_write_func(char *data, size_t size, size_t nmemb, std::string *buffer) {
	  size_t result = 0;
	  if (buffer) {
		  buffer->append(data, size*nmemb);
		  result = size*nmemb;
	  }
	  return result;
  }
  size_t CurlHandler::header_callback(char *buffer, size_t size, size_t nitems, void *userdata){
	  /* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
	  /* 'userdata' is set with CURLOPT_HEADERDATA */
	  auto* headers = (std::vector<std::string> *)userdata;
	  headers->emplace_back(std::string(buffer, nitems * size));
	  return nitems * size;
  }

  void CurlHandler::clearBuffers(){
	  buf_response.clear();
	  buf_error.clear();
	  buf_headers.clear();
	  clearQuery();
	  response_mime_type = ResponseType::None;
  }

  void CurlHandler::clearQuery() {
	  if (buf_query){
		  buf_query.reset();
		  if (curl_handle) curl_easy_setopt(curl_handle, CURLOPT_READDATA, nullptr);
		  else THROW_CURL
	  }
  }

  bool CurlHandler::isSetMethod(Method new_method) const
  {
	  /*
  	if (curl) {
  		char *method = NULL;
  		curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_METHOD, &method);
  		if(method)
  			printf("Redirected to method: %s\n", method);
  	}
  	else throw std::runtime_error("Unable to access curl, check dependencies\n"+std::string(__PRETTY_FUNCTION__));
  */
	  return this->method==new_method; //it will work in the new environment
  }
  void CurlHandler::updateMethod(const Method method){
	  (this->*set_method.at(method))();
	  this->method = method;
  }

  const std::unordered_map<Method, void (CurlHandler::*)()> CurlHandler::set_method{
		  {Method::None, &CurlHandler::setNone},
		  {Method::Get, &CurlHandler::setGet},
		  {Method::Post, &CurlHandler::setPost},
		  {Method::Put, &CurlHandler::setPut},
		  {Method::Delete, &CurlHandler::setDelete},
		  {Method::Patch, &CurlHandler::setPatch},
  };

  void CurlHandler::setNone()
  {
	  throw std::runtime_error("Unable to SET CURL METHOD\n"+std::string(__PRETTY_FUNCTION__));
  }
  void CurlHandler::setGet(){
	  if (curl_handle) curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
	  else THROW_CURL
  }
  void CurlHandler::setPost(){
	  if (curl_handle) {
		  // use POST
		  curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
	  }
	  else THROW_CURL
  }
  void CurlHandler::setPut(){
	  if (curl_handle) {
		  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
	  }
	  else THROW_CURL
  }
  void CurlHandler::setDelete(){
	  if (curl_handle) {
		  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
	  }
	  else THROW_CURL
  }
  void CurlHandler::setPatch(){
	  if (curl_handle) {
		  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PATCH");
	  }
	  else THROW_CURL
  }

  void CurlHandler::setNewQuery (std::string&& q) {
	  auto new_query = std::make_unique<QueryData>(std::move(q));
	  buf_query.reset(new_query.get());
  }
  bool CurlHandler::hasDataToCopy (QueryData *p_q) {
	  return p_q->sizeleft > 0;
  }
  size_t CurlHandler::getCurrQuerySize (QueryData *p_q) {
	  return p_q->sizeleft;
  }
  void CurlHandler::copyDataToBuffer (char *buffer, size_t buffer_size, QueryData *q) {
	  q->curr_chunk_size = q->sizeleft > buffer_size ? buffer_size : q->sizeleft;
	  std::memcpy(buffer, q->query_str.data() + q->curr_offset, q->curr_chunk_size);
	  q->curr_offset += q->curr_chunk_size;
	  q->sizeleft -= q->curr_chunk_size; //see two lines above, it is safe to subsract from size_t
  }
  size_t CurlHandler::getCopiedCount (QueryData *p_q) {
	  return p_q->curr_chunk_size;
  }




  void CurlHandler::setResponseType() {

	  if (not checkRequestOk()) return;

	  for (const auto &header : buf_headers){
		  const auto pos_content_type = header.find("Content-Type", 0);
		  if (pos_content_type != header.npos) {
			  const auto pos_json = header.find("json", pos_content_type);
			  if (pos_json != header.npos){
				  response_mime_type = ResponseType::Json;
				  break;
			  }
			  const auto pos_text = header.find("text", pos_content_type);
			  if (pos_text != header.npos){
				  response_mime_type = ResponseType::Text;
			  }
			  break;
		  }
	  }
	  //to set text by default, and if no headers
	  if (not isSetResponseType()) {
		  response_mime_type = ResponseType::Text;
	  }
  }
  bool CurlHandler::isSetResponseType () const {
	  return response_mime_type != ResponseType::None;
  }

}//!namespace