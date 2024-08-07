//
// Created by Andrey Solovyev on 27/09/2021.
//

#include "curl_handler.h"
#include <stdexcept>
#include <utility>

namespace curl_client {

#define THROW_CURL throw std::runtime_error("Unable to access curl "+std::string(__PRETTY_FUNCTION__));
#define DEBUG_OUPUT

  CurlHandler::CurlHandler()
    : method(Method::None)
  {
	  buf_response.reserve(curl_client::const_values::EXPECTED_REQUEST_SIZE);
	  buf_error.fill('\0');
	  buf_headers.reserve(curl_client::const_values::EXPECTED_HEADERS_RESPONSE_COUNT);
	  Init();
  }

  CurlHandler::~CurlHandler() {
	  /**
	   * @dev
	   * this is supposed to free headers in curl slist,
	   * but char* that is a data for each node is a pointer
	   * to header in the map, that is another class member;
	   * map will be cleaned by its dtor, therefore slist should be cleaned
	   * only from the pointers, so here we solve an issue of leaking 16 bytes
	   * per each headers slist node along with avoiding to delete
	   * a real string underneath of a list node.
	   */
	  if (headers_list) {
#if 0
		  curl_slist_free_all(headers_list);
#endif
		  while(headers_list) {
			  auto tmp {headers_list->next};
			  headers_list->data = nullptr;
			  headers_list->next = nullptr;
			  delete headers_list;
			  std::swap(headers_list, tmp);
		  }
	  }

	  if (curl_handle) {
		  curl_easy_cleanup(curl_handle);
		  curl_handle = nullptr;
	  }

	  curl_global_cleanup();
  }

  void CurlHandler::setHeaders(http::headers headers){
	  if (!curl_handle) {
		  THROW_CURL
	  }

	  if (!headers.empty()) {
		  for (auto &[name, header] : headers) {
			  setHeader(std::move(header));
		  }
	  }
	  else {
		  for (auto const& [field, value] : curl_client::const_values::headers_by_default) {
			  setHeader(http::header(field, value));
		  }
	  }

	  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers_list);

	  if (not headers_list) {
		  throw std::runtime_error("Unable to SET HEADERS\n" + std::string(__PRETTY_FUNCTION__));
	  }
  }

  void CurlHandler::setHeader (http::header header) {
	  auto *h = headers_list;
	  while (h) {
		  std::string_view h_view (h->data);
		  auto pos = h_view.find(':');
		  if (pos == h_view.npos) {
			  continue;
		  }
		  if (h_view.substr(0, pos) == header.field()) {
			  auto& header_in_place {headers[header.field()]};
			  header_in_place = std::move(header);
			  h->data = const_cast<char*>(header_in_place.curl_header());

			  return;
		  }
		  h = h->next;
	  }
	  auto [inserted, ok] = headers.insert({header.field(), std::move(header)});
	  headers_list = curl_slist_append(headers_list, const_cast<char*>(inserted->second.curl_header()) );
  }

	void CurlHandler::removeHeader (http::header::field_type const& field) {
		auto found = headers.find(field);
		if (found == headers.end()) {
			return;
		}

		headers.erase(found);

		auto *curr = headers_list, *prev = headers_list;
		while (curr) {
			std::string_view h_view (curr->data);
			auto pos = h_view.find(':');
			if (pos == h_view.npos) {
				continue;
			}
			if (h_view.substr(0, pos) == field) {
				prev->next = curr->next;
				delete curr;
				return;
			}
			prev = curr;
			curr = curr->next;
		}
	}

  void CurlHandler::setUrl (const std::string &url){
	  if(curl_handle) {
		  curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	  }
	  else THROW_CURL
  }

  void CurlHandler::setMethod (Method method) {
	  if (not isSetMethod(method)) {
		  updateMethod(method);
	  }
  }

  void CurlHandler::setQuery(http::query_data_ptr&& query) {
	  if (not query) {
		  throw std::runtime_error("No query\n"+std::string(__PRETTY_FUNCTION__));
	  }
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
	  clean_up(); //must be BEFORE
	  if(curl_handle) {
		  request_return_code = curl_easy_perform(curl_handle);
		  setResponseType();
		  clear_query(); //must be AFTER
	  }
	  else THROW_CURL
  }

  const http::headers& CurlHandler::getRequestHeaders() const {
	  return headers;
  }

  const http::header& CurlHandler::getRequestHeaderAt(http::header::field_type const& name) const {
	  if (auto it = headers.find(name); it != headers.end()) {
		  return it->second;
	  }
	  return http::empty_header;
  }

  std::string CurlHandler::getResponseBody () {
	  return std::move(buf_response);
  }

  ResponseType CurlHandler::getResponseType() const {
	  return response_mime_type;
  }

  std::string CurlHandler::getError(){
	  return std::string(buf_error.begin(), buf_error.end());
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
	  curl_global_init(CURL_GLOBAL_DEFAULT);
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

		  /* we want to use our own read function */
		  curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, &CurlHandler::curl_read); //static

		  // callback params
		  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &buf_response);
		  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &CurlHandler::curl_write); //static

		  //received headers
		  curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, &CurlHandler::curl_header); //static
		  /* pass in custom data to the callback */
		  curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &buf_headers);

		  setHeaders();
	  }
	  else THROW_CURL
  }

  size_t CurlHandler::curl_read(char *buffer, size_t size, size_t nmemb, void *userdata){
	  if (not userdata) return 0; //avoid falling

	  auto *q = static_cast<http::query_data*>(userdata);
	  size_t buffer_size = size*nmemb;

	  if (q->sizeleft > 0) { //we still have some data to copy
		  q->curr_chunk_size = q->sizeleft > buffer_size ? buffer_size : q->sizeleft;
		  std::memcpy(buffer, q->query_str.data() + q->curr_offset, q->curr_chunk_size);
		  q->curr_offset += q->curr_chunk_size;
		  q->sizeleft -= q->curr_chunk_size; //see two lines above, it is safe to subsract from size_t
		  return q->curr_chunk_size; /* we copied this many bytes */
	  }
	  return 0; /* no more data left to deliver */
  }

  size_t CurlHandler::curl_write(char *data, size_t size, size_t nmemb, std::string *buffer) {
	  size_t result = 0;
	  if (buffer) {
		  buffer->append(data, size*nmemb);
		  result = size*nmemb;
	  }
	  return result;
  }

  size_t CurlHandler::curl_header(char *buffer, size_t size, size_t nitems, void *userdata){
	  /* received header is nitems * size long in 'buffer' NOT ZERO TERMINATED */
	  /* 'userdata' is set with CURLOPT_HEADERDATA */
	  auto* headers = (std::vector<std::string> *)userdata;
	  headers->emplace_back(std::string(buffer, nitems * size));
	  return nitems * size;
  }

  void CurlHandler::clean_up(){
	  method = Method::None;
	  buf_response.clear();
	  buf_error.fill('\0');
	  buf_headers.clear();
	  response_mime_type = ResponseType::None;
  }

  void CurlHandler::clear_query() {
	  if (buf_query){
		  buf_query.reset();
		  if (curl_handle) {
			  curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 0L);
			  curl_easy_setopt(curl_handle, CURLOPT_READDATA, nullptr);
		  }
		  else THROW_CURL
	  }
  }

  bool CurlHandler::isSetMethod(Method new_method) const {
	  return method==new_method; //it will work in the new environment
  }

  void CurlHandler::updateMethod(Method method){
	  (this->*set_method.at(method))();
	  method = method;
  }

  const std::unordered_map<Method, void (CurlHandler::*)()> CurlHandler::set_method{
		  {Method::None, &CurlHandler::setNone},
		  {Method::Get, &CurlHandler::setGet},
		  {Method::Post, &CurlHandler::setPost},
		  {Method::Put, &CurlHandler::setPut},
		  {Method::Delete, &CurlHandler::setDelete},
		  {Method::Patch, &CurlHandler::setPatch},
		  {Method::FTPListOnly, &CurlHandler::setFTPListOnly},
  };

  void CurlHandler::setNone() {
	  throw std::runtime_error("Unable to SET CURL METHOD\n"+std::string(__PRETTY_FUNCTION__));
  }
  void CurlHandler::setGet(){
	  if (curl_handle) {
		  if (buf_query){
			  curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1L);
			  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
			  curl_easy_setopt(curl_handle, CURLOPT_READDATA, buf_query.get());
		  }
		  else {
			  curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
		  }
	  }
	  else {
		  THROW_CURL
	  }
  }
  void CurlHandler::setPost(){
	  if (curl_handle) {
		  if (buf_query){
			  curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1L);
			  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");
			  curl_easy_setopt(curl_handle, CURLOPT_READDATA, buf_query.get());
		  }
		  else {
			  curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
		  }
	  }
	  else {
		  THROW_CURL
	  }
  }
  void CurlHandler::setPut(){
	  if (curl_handle) {
		  if (buf_query){
			  curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1L);
			  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
			  curl_easy_setopt(curl_handle, CURLOPT_READDATA, buf_query.get());
		  }
		  else {
			  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
		  }
	  }
	  else {
		  THROW_CURL
	  }
  }
  void CurlHandler::setDelete(){
	  if (curl_handle) {
		  if (buf_query){
			  curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1L);
			  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
			  curl_easy_setopt(curl_handle, CURLOPT_READDATA, buf_query.get());
		  }
		  else {
			  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
		  }
	  }
	  else {
		  THROW_CURL
	  }
  }
  void CurlHandler::setPatch(){
	  if (curl_handle) {
		  if (buf_query){
			  curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1L);
			  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PATCH");
			  curl_easy_setopt(curl_handle, CURLOPT_READDATA, buf_query.get());
		  }
		  else {
			  curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PATCH");
		  }
	  }
	  else {
		  THROW_CURL
	  }
  }
  void CurlHandler::setFTPListOnly(){
	  if (curl_handle) {
		  curl_easy_setopt(curl_handle, CURLOPT_DIRLISTONLY, 1L);
	  }
	  else {
		  THROW_CURL
	  }
  }

  void CurlHandler::setNewQuery (std::string&& q) {
	  auto new_query = std::make_unique<http::query_data>(std::move(q));
	  buf_query.reset(new_query.get());
  }

  size_t CurlHandler::getCurrQuerySize (http::query_data *p_q) {
	  return p_q->sizeleft;
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