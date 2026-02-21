#include "wordle.hpp"

void Wordly::gameOverScreenRenderer(void) {

    
    DrawRectangle(0,0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, 0.5f));
    Rectangle panel = {40, 100, (float) GetScreenWidth() - 80, (float) GetScreenHeight() - 120};

    DrawRectangleRec(panel, ColorAlpha(DARKGRAY, 0.9f));
    DrawRectangleLinesEx(panel,2 , config.grid_color);
    if(userWon) {
    DrawText("WELL DONE!", panel.x + 20, panel.y + 20, 32, GREEN); 
    std::string usersTime = this->mainTimer.getCurrentTime();
    std::string text = std::string("You guessed the word ") + "\"" + this->word + "\"\n" + "in ";
    float minutes = this->mainTimer.getMins();
    float seconds = this->mainTimer.getSeconds();
    if(minutes) {
        
        
        std::string params = seconds ? ",\nand " + std::to_string((int) seconds) + " seconds" : "";
        text += std::to_string((int) minutes) + " minutes" + params;
    }  
    else {
        text += std::to_string((int) seconds) + " seconds";
    }

    DrawText(text.c_str(), panel.x + 20, panel.y + 60, 20, GREEN);
    } else {
        DrawText("NEXT TIME...", panel.x + 20, panel.y + 20, 32, RED);
        std::string str = std::string("WORD WAS: ") + "\"" + this->word + "\"";
        DrawText(str.c_str(), panel.x + 20, panel.y + 60, 20, LIGHTGRAY);  
    }
    

    
    int statY = panel.y + 130;
    auto drawStatRow = [&](const std::string & label, const std::string & key, int x) {
        auto val = usersHistory.getValue<int>(key);
        if(val.has_value()) {
            DrawText(std::to_string(val.value()).c_str(), x, statY, 30, WHITE);
            DrawText(label.c_str(), x, statY + 35, 12, LIGHTGRAY);
        }
    };
    drawGuessDistribution(panel);
    drawStatRow("Total games", "total_games", panel.x + 30);
    drawStatRow("Wins", "wins", panel.x + 125);
    drawStatRow("Losses", "losses", panel.x + 190);
    drawStatRow("Current streak", "current_streak", panel.x + 270);
    drawStatRow("Best streak", "best_streak", panel.x + 370);
   drawTotalXp(panel);
    Rectangle box = {145, 600, 120, 30};
    std::string text = "Play again";
    Button playAgain;
    playAgain = playAgain.drawBtn(box, text, PINK);
        if(playAgain.checkClick(GetMousePosition())) {
        clearVariables();
    getRandomWord();
        mainTimer.start();
    }

    Rectangle box2 =  {280, 600, 120, 30};
    std::string text2 = "Exit";
    Button exit;
    exit = exit.drawBtn(box2, text2, PINK);

    if(exit.checkClick(GetMousePosition())) {
                clearVariables();
                this->config.autoplay = false;
                state = MAIN_MENU;
    }
}
void Wordly::drawTotalXp(const Rectangle & panel) const {
    if(usersHistory.exists("total_xp")) {
        auto x = usersHistory.getValue<std::string>("total_xp");
        if(x.has_value()) {
            std::string text = std::string("Total XP: ") + x.value() + std::string(" (+ ")  + std::to_string(this->totalXp) + std::string(" this game)");
            DrawText(text.c_str(), panel.x + 10, panel.y + 400, 20, LIGHTGRAY);
        }
    }
}

    void Wordly::drawGuessDistribution(const Rectangle & rec) const {
        int startY = rec.y + 180;
        int maxWidth = GetScreenWidth();
        int maxWins = 0;
        auto distribution = usersHistory.getObject("guess_distribution");

        if(distribution.has_value()) {
            for(int i = 1; i <= 6; i++) {
                auto current = distribution->getValue<int>(std::to_string(i));
                if(current.has_value()) {
                    if(current.value() > maxWins) maxWins = current.value();
                }
            }

            for(int i = 1; i <= 6; i++) {
                 auto current = distribution->getValue<int>(std::to_string(i));
                if(current.has_value()) {
                    int val = current.value();
                    float barW = 20;
                    if(current.value() != 0) {
                      barW = 25.0f + ((float) val /  maxWins) * (rec.width - 100.0f);
                    }
                    Color barC = (attempts == i) ? GREEN : DARKGRAY;

                    DrawText(std::to_string(i).c_str(), rec.x + 20, startY + (i * 30),20,WHITE);
                    DrawRectangle(rec.x + 50, startY +(i * 30), (int) barW, 25, barC);
                    DrawText(std::to_string(val).c_str(), rec.x + 50 + (int) barW - 20,
                startY + (i * 30) + 4, 18, WHITE);

                }
            }
        }

    }