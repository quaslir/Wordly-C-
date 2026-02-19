#include "wordle.hpp"

int Wordly::centerTextByX(const std::string & text, int fontSize, int width, int marginX = 0) const {
int textWidth = MeasureText(text.c_str(), fontSize);

return marginX + (width - textWidth) / 2;
}

void Wordly::drawLogo(void) const {
    const std::string text = "WORDLE";
    int space = 10;
    int squareSize = 50;
        int x = (GetScreenWidth() / 2) - (((6 * squareSize) + (5 * space)) / 2);
        int y = 30;
        int fontSize = 40;
    static std::string buffer;
    Color squareColor = {83, 141, 78, 255};
    for(int i = 0; i < text.size(); i++) {
        buffer.clear();
        buffer += text[i];
        int currentX = x + (i * (squareSize + space));
   DrawRectangle(currentX, 30, squareSize, squareSize, squareColor);
   int textWidth = MeasureText(buffer.c_str(), fontSize);

   int textX  = currentX + (squareSize - textWidth) / 2;
   int textY = y + (squareSize - fontSize) / 2;
      DrawText(buffer.c_str(), textX + 2, textY + 2, fontSize, BLACK);
    DrawText(buffer.c_str(), textX, textY, fontSize, RAYWHITE);
    } 
    
}

void Wordly::drawFrontScreen(void) {
    ClearBackground({18, 19, 19, 255});
    drawLogo();
    const int numButtons = 4;
    static const std::vector<std::string> buttons = {"Daily challenge", "Pratice mode", "Autoplay showcase", "Exit"};
    float btnW = 280.f;
    float btnH  = 40.f;
    float startX = (GetScreenWidth() - (float) btnW) / 2;
    float startY = 200;
    float spacing = 15;
    for(int i = 0; i < buttons.size(); i++) {
        Rectangle rec = {startX, startY + i * (btnH + spacing), btnW, btnH};
        bool isHovered = (CheckCollisionPointRec(GetMousePosition(), rec));
        Color color = isHovered ? Color{106,170,100,255} : Color{83, 141, 78, 255};
        if(!activeDailyChallenge && i == 0) {
            color = DARKGREEN;
        }
        Button btn = drawBtn(rec, buttons[i], color);

        if(btn.checkClick(GetMousePosition())) {

            switch (i) {
            case 0 :    
            if(activeDailyChallenge) {
                state = DAILY_CHALLENGE;
                getRandomWordDayChallenge();
            }
                break;
            case 1 :
                state = PRACTICE;
                mainTimer.start();
                getRandomWord();
                break;
            case 2 :
            state = AUTOPLAY;
            getRandomWord();
            this->config.autoplay = true;
            break;

            case 3: 
            std::exit(0);
        }
    }
}
}

void Wordly::drawError(const std::string & msg) const {
        DrawText(msg.c_str(), 40, 680, 20, RED);
    }
void Wordly::drawTimer(void) const {
        std::string text = mainTimer.getCurrentTime();
        DrawText(text.c_str(), 465, 25, 23, GREEN);
    }
    void Wordly::drawGuessDistribution(const Rectangle & rec) const {
        int startY = rec.y + 200;
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
 void Wordly::draw(void) {
    if(state == EMPTY_USERNAME) {
        drawLogo();
        setUsername();
    }
   else if(state == DAILY_CHALLENGE || state == PRACTICE || state == AUTOPLAY) {
    mainTimer.update();
    drawTimer();
    drawLogo();
        drawUsername();
    if(!gameOver) {
    std::string buf;
    float offset = 0.0f;
    if(pendingGameOver) {
        timer -= GetFrameTime();

        if(timer <= 0) {
            pendingGameOver = false;
            gameOver = true;
            timer  =0;
        }
    }
    else {
        if(this->config.autoplay) {
            autoBotPlay();
        }
    }
    if(shakeTimer > 0) {
        shakeTimer -= GetFrameTime();
        offset = sinf(shakeTimer * 60.f) * shakeIntensity * (shakeTimer / 0.5f);
    }
       for(size_t i = 0; i < 6; i++) {
        float currentRowOffset = i == activeY ? offset : 0.0f;
        for(size_t j = 0; j < 5; j++) {
            buf.clear();
            auto c = this->history[i][j];
            buf += c.c;
        float calculateX = (float) ((j * SQUARE_SIZE * 1.1) + 75) + currentRowOffset;
        float calculateY =  (float) ((i * SQUARE_SIZE * 1.1) + 90);
        Rectangle box = {(float) calculateX,calculateY, SQUARE_SIZE, SQUARE_SIZE};
            Vector2 textSize = MeasureTextEx(GetFontDefault(), buf.c_str(), 40.f, 2);
            float textX = box.x + (box.width / 2) - (textSize.x / 2);
            float textY = box.y + (box.height / 2) - (textSize.y / 2);
            DrawText(buf.c_str(), (int) textX, (int) textY, 40, getColor(c.type));
        float thickness = 3.0f;

        DrawRectangleLinesEx(box, thickness, this->config.grid_color);
       }
       }

       renderKeyBoard();
       writeKey();
    } else {
        gameOverScreenRenderer();
    }

    if(renderErrorMessage) {
        drawError(errorMessage);
    }
}
else {
    drawFrontScreen();
    drawUsername();
}
 }

Button Wordly::drawBtn(const Rectangle & box, const std::string & text, const Color & color) const{
    Button btn (box, color, text);

    DrawRectangle(btn.btn.x, btn.btn.y, btn.btn.width, btn.btn.height, btn.color);
    DrawText(btn.text.c_str(),btn.btn.x + 10, btn.btn.y + 5, 20, BLACK);
    return btn;
}
void Wordly::gameOverScreenRenderer(void) {
    mainTimer.stop();
    
    DrawRectangle(0,0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, 0.5f));
    Rectangle panel = {40, 80, (float) GetScreenWidth() - 80, (float) GetScreenHeight() - 120};

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
    

    
    int statY = panel.y + 110;
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
   
    Rectangle box = {145, 500, 120, 30};
    std::string text = "Play again";
    Button playAgain = drawBtn(box, text, PINK);
        if(playAgain.checkClick(GetMousePosition())) {
        gameOver = false;
        clearHistory();
        activeX = 0;
        activeY = 0;
        attempts = 0;
        errorMessage = "";
        renderErrorMessage = false;
        mustUsedChars.clear();
        initKeyboard();
        getRandomWord();
        mainTimer.start();
    }

    Rectangle box2 =  {280, 500, 120, 30};
    std::string text2 = "Exit";
    Button exit = drawBtn(box2, text2, PINK);

    if(exit.checkClick(GetMousePosition())) {
                this->config.autoplay = false;
                state = MAIN_MENU;
                gameOver = false;
                 gameOver = false;
                clearHistory();
                activeX = 0;
                activeY = 0;
                attempts = 0;
                errorMessage = "";
                renderErrorMessage = false;
                mustUsedChars.clear();
                initKeyboard();
    }
}

void Wordly::renderKeyBoard(void) {
int posY = 8 * SQUARE_SIZE + SQUARE_SIZE / 3;
int posX = 60;
std::string character;
for(auto & x : keyboard) {
    character.clear();
    character += x.c;
    Color color = x.status == NOT_CHECKED ? LIGHTGRAY : x.status == CORRECT ? GREEN : x.status == INCORRECT ? YELLOW : DARKGRAY;
    Rectangle box = {(float) posX, (float) posY, (x.c == "ENT" || x.c == "DEL") ? (float) (CELL_SIZE * 1.5)
         : (float) CELL_SIZE, (float) CELL_SIZE};
    Vector2 textSize = MeasureTextEx(GetFontDefault(), character.c_str(), 18.f, 2);
    float textX = box.x + (box.width / 2) - (textSize.x / 2);
    float textY = box.y + (box.height / 2) - (textSize.y / 2);
    bool specialButton  = x.c == "ENT" || x.c == "DEL" ? 1 : 0;
    int size = 0;
    if(specialButton) {
         x.box = Rectangle(posX, posY, CELL_SIZE * 1.5, CELL_SIZE);
         size = CELL_SIZE * 1.5;
    }
    else {
        size = CELL_SIZE;
         x.box = Rectangle(posX, posY, size,size);
    }
   
    DrawRectangle(posX, posY, size, CELL_SIZE, color);
    DrawText(character.c_str(), (int) textX, (int) textY, 18, BLACK);

    posX += size + 6;

    if(x.c == "p" || x.c == "l") {
        posY += CELL_SIZE + 6;
        posX = 60;
    }

}
}

void Wordly::drawUsername(void) const {
    DrawText(username.c_str(), 10, 15, 25, DARKGREEN);
}