#include "lib/curl_client.h"

using namespace curl_client;

int main(){

//	std::vector<std::string> headers {
//		"Accept: application/json",
//		"Connection: Keep-Alive",
//		"Keep-Alive: timeout=5, max=1000",
//		"Content-Encoding: gzip",
//		"Content-Type: text/html; charset=utf-8",
//	};
//
//	curl->setHeaders(headers);


	Response response;
	Request request;
/*
	for (const auto& [_, header]: request.headersGet())
		std::cerr << header() << '\n';

	HttpHeaders headers {
			{"Accept", {"Accept", "application/json"}},
			{"Connection",{"Connection", "Keep-Alive"}},
			{"Keep-Alive",{"Keep-Alive", "timeout=5, max=1000"}},
			{"Content-Encoding",{"Content-Encoding", "gzip"}},
			{"Content-Type",{"Content-Type", "text/html; charset=utf-8"}},
			{"FUCK",{"FUCK", "some crazy shit"}},
	};

	request.headersSetNew(std::move(headers));
	for (const auto& [_, header]: request.headersGet())
		std::cerr << header() << '\n';
*/

	response = request.
							  SetVerbose(true)->
							  pathSetNew("https://www.example.com")->
							  headerSetSingle(HttpHeader{"CB-ACCESS-SIGN", "some_signature"})->
							  headerSetSingle(HttpHeader{"CB-ACCESS-TIMESTAMP", "some_signature"})->
							  headerSetSingle(HttpHeader{"CB-ACCESS-KEY", "some_signature"})->
							  Implement(Method::Get);


	Print (response, std::cout, true);

	//todo: test POST and PUT - heavy refactoring

	return 0;
}
