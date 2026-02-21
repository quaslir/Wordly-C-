#include "leaderboard.hpp"
size_t leaderboardCallback(void * contents, size_t size, size_t number, std::string * result) {
    size_t totalSize = size * number;
    result->append((char *) contents, totalSize);
    return totalSize;
}


void Leaderboard::loadLeaderboard(void) const {
    CURL * curl = curl_easy_init();
    if(!curl) {
        throw std::runtime_error("HTTP request failed");
    }
    std::string buffer;
    CURLcode result;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, leaderboardCallback);
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/leaderboard");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, & buffer);

    result = curl_easy_perform(curl);

    if(result != CURLE_OK) {
        throw std::runtime_error(std::string("Error: " + std::to_string((int) result)));
    }
    ParserJSON ps;
    std::stringstream ss (buffer);

    ps.parse(ss);
            
    size_t size = ps.getSize();
    std::vector<std::pair<std::string, size_t>> leaderboard;
    for(size_t i = 0; i < size; i++) {
        auto x = ps.getObject(std::to_string(i)).value();
        std::string username = x.getValue<std::string>("username").value();
        size_t xp = x.getValue<size_t>("xp").value();
        leaderboard.push_back({username, xp});
    }
    std::sort(leaderboard.begin(), leaderboard.end(), [](const std::pair<std::string, size_t> & first, 
    const std::pair<std::string, size_t> & second) {
        return first.second > second.second;
    });
    renderLeaderboard(leaderboard);
    curl_easy_cleanup(curl);
}

void Leaderboard::renderLeaderboard(const std::vector<std::pair<std::string, size_t>> & leaderboard) const {
    DrawRectangle(0,0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
    drawLogo();

int x = 10;
int y = 100;
    for(int i = 0; i < leaderboard.size(); i++) {
        DrawText(std::to_string(i + 1).c_str(), x, y, 20, RAYWHITE);
        x += 30;
        DrawText(leaderboard[i].first.c_str(), x, y, 20, RAYWHITE);

        x = GetScreenWidth() - 100;

        DrawText(std::to_string((int) leaderboard[i].second).c_str(), x, y, 20, RAYWHITE);

        y += 30;
        x = 10;
    }
}


void Leaderboard::updateLeaderboard(const std::string & username, const size_t xp) {
CURL * curl = curl_easy_init();

if(!curl) {
        throw std::runtime_error("HTTP request failed");
}
ParserJSON data;
data.insert<std::string>("username", username);

data.insert<std::string>("xp", std::to_string((int) xp));

std::string dataFormatted = data.toString();
struct curl_slist * headers = NULL;
headers = curl_slist_append(headers, "Content-Type: application/json");
curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/update-leaderboard");
curl_easy_setopt(curl, CURLOPT_HTTPHEADER,headers);
curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dataFormatted.c_str());

curl_easy_perform(curl);

curl_easy_cleanup(curl);

}