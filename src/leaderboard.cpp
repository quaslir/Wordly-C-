#include "wordle.hpp"


void Wordly::loadLeaderboard(void) const {
    CURL * curl = curl_easy_init();
    if(!curl) {
        throw std::runtime_error("HTTP request failed");
    }
    CURLcode result;

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.google.com");

    result = curl_easy_perform(curl);

    if(result != CURLE_OK) {
        throw std::runtime_error(std::string("Error: " + std::to_string((int) result)));
    }

    curl_easy_cleanup(curl);
}