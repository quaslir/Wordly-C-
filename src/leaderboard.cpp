#include "leaderboard.hpp"
#include "button.hpp"
size_t leaderboardCallback(void * contents, size_t size, size_t number, std::string * result) {
    size_t totalSize = size * number;
    result->append((char *) contents, totalSize);
    return totalSize;
}


void Leaderboard::loadLeaderboard(void) {
    std::cout << "Sending request" << std::endl;
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
    leaderboardLoaded = true;

    curl_easy_cleanup(curl);
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

void Leaderboard::renderLeaderboard(void) {
    DrawRectangle(0,0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
    drawLogo();
int screenWidth = GetScreenWidth();
int panelWidth = 450;
int rowHeight = 45;
int x = (screenWidth - panelWidth) / 2;
int y = 150;
DrawRectangle(x, y - 40, panelWidth, 35, Fade(GRAY, 0.3f));
DrawText("RANK", x + 10, y - 33, 20, GOLD);
DrawText("PLAYER", x + 100, y - 33, 20, GOLD);
DrawText("TOTAL XP", x + panelWidth - 110, y - 33, 20, GOLD);

    for(int i = 0; i < leaderboard.size(); i++) {
        Color rowColor  = (leaderboard[i].first == getUsername()) ? GREEN : RAYWHITE;
        int currentY = y + (i * rowHeight);

        if(i % 2 == 0) {
            DrawRectangle(x, currentY - 5, panelWidth, rowHeight - 5, Fade(WHITE, 0.05f));
        }

        Color rankColor = i == 0 ? GOLD : i == 1 ? LIGHTGRAY : i == 2 ? ORANGE : WHITE;
        DrawText(TextFormat("%02d", i + 1), x + 15, currentY, 22, i == 0 || i == 1 || i == 2 ? rankColor : rowColor);  

        DrawText(leaderboard[i].first.c_str(), x + 100, currentY, 22, rowColor);
        std::string xpStr = std::to_string((int) leaderboard[i].second);
        int textWidth = MeasureText(xpStr.c_str(), 22);

        DrawText(xpStr.c_str(), x + panelWidth  - 15 - textWidth, currentY, 22, rowColor);

    }

    Button btn;
    Rectangle rec = {(float) screenWidth / 2 - 60, (float) GetScreenHeight() - 80, 120, 40};
     Color colorBtn = DARKGRAY;
    if(CheckCollisionPointRec(GetMousePosition(), rec)) {
        colorBtn = LIGHTGRAY;
    }
    btn = btn.drawBtn(rec, "Back", colorBtn);
    if(btn.checkClick(GetMousePosition())) {
        leaderboardLoaded = false;
        leaderboard.clear();
        changeState();
    }

}