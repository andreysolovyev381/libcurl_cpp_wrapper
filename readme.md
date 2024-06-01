## Bare minimum C++ libcurl wrapper

### Rational
This is a C++ MINI wrapper for well known C-library "libcurl". Wrapper makes a query and gets a text or the simplest json as a result. No performance but extreme simplicity - basically makes GET, PUT queries and gets back the chars. 

See the tests for the details and examples.

### Kudos
To [magnificent Vinnie Falco](https://github.com/vinniefalco), his contribution to Boost for broader subject. 

### Dependency
* C++20
* libcurl

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
* Copy-paste ./curl_client into your source dir. ```add_subdirectory(curl_client)``` is expected
* Then link curl_client lib into your project, see ./tests/CMakeLists.txt as an example:
```CMake
target_link_libraries(${TESTS_NAME}
        ${GTEST_LIBRARIES}
        curl_client
)
```

### License
MIT