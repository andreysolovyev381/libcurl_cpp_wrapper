## Bare minimum C++ libcurl wrapper

### Rational
This is a C-library "libcurl" C++ MINI wrapper, that makes a query and gets a text or the simplest json as a result. No performance but extreme simplicity - basically makes GET, PUT queries, gets back the chars. See the tests for the details and examples.
Zero dependencies except curl itself.

### Details
Allows to manage:
* url, ie insert something in the middle - in case it deals with some kind of REST API
* request headers 
* request methods

```cpp
#include "curl_client.h"
...
	std::string const test_url {"https://httpbin.org/get"};
	curl_client::request request;
	curl_client::response response = request.
			setURL(test_url)->
			implement(curl_client::Method::Get);
	
	curl_client::Print(response, std::cout);
...
```
For more examples see the ./test folder

### Testing 
Done using this special purpose and very nice [website](https://httpbin.org/). Indeed, there is no guarantee it stays online forever. 

### Usage
* Copy-paste ./curl_client into your source dir
* link curl_client lib into your project, see ./tests/CMakeLists.txt as an example:
```CMake
target_link_libraries(${TESTS_NAME}
        ${GTEST_LIBRARIES}
        curl_client
)
```

### License
MIT