#include "wordle.hpp"

int Wordly::centerTextByX(const std::string & text, int fontSize, int width, int marginX = 0) const {
int textWidth = MeasureText(text.c_str(), fontSize);

return marginX + (width - textWidth) / 2;
}


void Wordly::drawFrontScreen(void) {
    ClearBackground({18, 19, 19, 255});
    drawLogo();
    const int numButtons = 4;
    static const std::vector<std::string> buttons = {"Daily challenge", "Pratice mode", "Autoplay showcase", "Leaderboard", "Exit"};
    float btnW = 280.f;
    float btnH  = 40.f;
    float startX = (GetScreenWidth() - (float) btnW) / 2;
    float startY = 200;
    float spacing = 15;
    for(int i = 0; i < buttons.size(); i++) {
        Rectangle rec = {startX, startY + i * (btnH + spacing), btnW, btnH};
        bool isHovered = (CheckCollisionPointRec(GetMousePosition(), rec));
        Color color = isHovered ? Color{106,170,100,255} : Color{83, 141, 78, 255};
        if(!dailyChallenge.first && i == 0) {
            color = DARKGREEN;
        }
        Button btn;
        btn = btn.drawBtn(rec, buttons[i], color);

        if(btn.checkClick(GetMousePosition())) {

            switch (i) {
            case 0 :    
            if(dailyChallenge.first) {
                state = DAILY_CHALLENGE; 
                mainTimer.start();
                getRandomWordDayChallenge();
                dailyChallenge.second = generateDayId();
            }
                break;
            case 1 :
                state = PRACTICE;
                mainTimer.start();
                getRandomWord();
                std::cout << word << std::endl;

                break;
            case 2 :
            state = AUTOPLAY;
             mainTimer.start();
            getRandomWord();
            this->config.autoplay = true;
            break;

            case 3:

            state = LEADERBOARD;
            break;

            case 4: 
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
        int fontSize = 20;
        int x = MeasureText(text.c_str(), 20);
        DrawText(text.c_str(), (GetScreenWidth() - x) / 2, 70, fontSize, LIGHTGRAY);
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
            mainTimer.stop();
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
    int marginX = (GetScreenWidth() - (SQUARE_SIZE * 6)) / 2;
       for(size_t i = 0; i < 6; i++) {
        float currentRowOffset = i == activeY ? offset : 0.0f;
        for(size_t j = 0; j < 5; j++) {
            buf.clear();
            auto c = this->history[i][j];
            buf += c.c;
        float calculateX = (float) ((j * SQUARE_SIZE * 1.1) + marginX + 15) + currentRowOffset;
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
else if(state == LEADERBOARD) {
    if(!leaderboard.leaderboardLoaded) {
    leaderboard.loadLeaderboard();
    } else leaderboard.renderLeaderboard();
    
}
else {
    drawFrontScreen();
    drawUsername();
}
 }

void Wordly::renderKeyBoard(void) {
int posY = 8 * SQUARE_SIZE + SQUARE_SIZE / 3;
int posX = (GetScreenWidth() - (SQUARE_SIZE * 6)) / 2;
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
        posX = (GetScreenWidth() - (SQUARE_SIZE * 6)) / 2;
    }

}
}

void Wordly::drawUsername(void) const {
    int fontSize = 25;
    int paddingX = 15;
    
    int width = MeasureText(this->username.c_str(), fontSize);
    int x = GetScreenWidth() - width - paddingX;
    int y = paddingX;
    Rectangle rec = {(float) x - 10, (float) y - 5, (float) width + 20, (float) fontSize + 10};
    DrawRectangleRounded(rec, 0.5f, 10, ColorAlpha(BLACK, 0.3f));
    DrawText(this->username.c_str(), x, y, fontSize, RAYWHITE);
}
