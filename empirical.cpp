#include "empirical.hpp"
#include <iostream>
#include <string>
#include <curl/curl.h>

namespace Empirical {

void post(std::string message) {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://google.com");
	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
	    std::cout << "Error: " << curl_easy_strerror(res) << std::endl;
	}
	curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

} // namespace Empirical
