#include <curl/curl.h>
#include <string>
#include <vector>
#include <algorithm>
#include "raylib.h"
#include <sstream>
#include "../json-parser-c/parser.hpp"
#include "drawLogo.hpp"
class Leaderboard {
    public :

    void loadLeaderboard(void) const;
    void renderLeaderboard(const std::vector<std::pair<std::string, size_t>> & leaderboard) const;

    void updateLeaderboard(const std::string & username, const size_t xp);
};