#include "wordle.hpp"
std::random_device rd;
std::mt19937 gen(rd());

int Wordly::centerTextByX(const std::string & text, int fontSize, int width, int marginX = 0) const {
int textWidth = MeasureText(text.c_str(), fontSize);

return marginX + (width - textWidth) / 2;
}

void Wordly::drawLogo(void) const {
    int x = centerTextByX("WORDLE", 50, GetScreenWidth());
    DrawText("WORDLE", x, 10, 50, VIOLET);
}

void Wordly::drawFrontScreen(void) {
    drawLogo();
    Rectangle rec = {160, 200, 200, 30};
    Rectangle rec2 = rec;
    rec2.y += 40;
    Rectangle rec3 = rec2;
    rec3.y += 40;
    Rectangle rec4 = rec3;
    rec4.y += 40;
    static Color dailyChColor = GREEN;
    static Color practiceColor = GREEN;
    static Color BotShowCaseColor = GREEN;
    static Color exitColor = GREEN;
    Button dailyCh = drawBtn(rec, "Daily challenge", dailyChColor);
    Button practice = drawBtn(rec2, "Practice mode", practiceColor);
    Button BotShowCase = drawBtn(rec3, "Autoplay showcase", BotShowCaseColor);
    Button exit = drawBtn(rec4, "Exit", exitColor);
    dailyChColor = dailyCh.checkHover(GetMousePosition()) ? DARKGREEN : GREEN;
    practiceColor = practice.checkHover(GetMousePosition()) ? DARKGREEN : GREEN;
    BotShowCaseColor = BotShowCase.checkHover(GetMousePosition()) ? DARKGREEN : GREEN;
    exitColor = exit.checkHover(GetMousePosition()) ? DARKGREEN : GREEN;

    if(exit.checkClick(GetMousePosition())) {
        std::exit(0);
    }
    else if(dailyCh.checkClick(GetMousePosition())) {
        state = DAILY_CHALLENGE;
    }
}

Wordly::Wordly(std::istream & s) : ss(s) {
    this->state = MAIN_MENU;
    this->initHistoryFile();
     this->initHistory();
    if(usersHistory.exists("username")) {
        username = usersHistory.getValue<std::string>("username").value();
        username.erase(0,username.find_first_not_of(" \t\r\n"));
        username.erase(username.find_last_not_of(" \t\r\n") + 1, username.length() - 1);
    }
        this->parseFile();
        try {
            this->readConfig();
        } catch(std::exception & err) {
            std::cerr << err.what() << std::endl;
        }
        
       
        this->initKeyboard();
                this->getRandomWord();
        mainTimer.start();
}
bool Wordly::isEmpty(std::string_view str) const{
        return str.empty() || str.find_first_not_of(" \t\r\n") == std::string::npos;
}

void Wordly::initHistoryFile(void) {

    std::ifstream historyCheck("../history.json");
        if(historyCheck.is_open()) return;      
    ParserJSON main ("../history.json");
    main.insert("total_games", 0);
    main.insert("best_streak", 0);
    main.insert("last_played_date", "");
    main.insert("losses", 0);
    main.insert("guess_distribution", "");
    main.insert("current_streak", 0);
    main.insert("wins", 0);
    ParserJSON submain;
    submain.insert("1", 0);
    submain.insert("2", 0);
    submain.insert("3", 0);
    submain.insert("4", 0);
    submain.insert("5", 0);
    submain.insert("6", 0);

    main.updateValue<std::string>("guess_distribution", submain.toString());

    main.stringify();
}

void Wordly::initKeyboard(void) {
std::string layout{"qwertyuiopasdfghjklDELzxcvbnmENT"};
std::string buffer;
for(int i = 0; i < layout.size(); i++) {
    buffer.clear();
    if(layout[i] == 'D') {
        while(layout[i] != 'z') buffer += layout[i++];
    }
    else if(layout[i] == 'E') {
        while(layout[i]) buffer += layout[i++];
    }
    else {
         buffer += layout[i];
    }
   
    keyboard.push_back({buffer, NOT_CHECKED});
}
}
    bool Wordly::handleInput(std::string_view word) const{
        if(word.length() != 5) return false;

        for(const auto & x : word) {
            if(!isalpha(x)) return false;
        }
        return true;
    }

    void Wordly::initHistory(void) {
        this->usersHistory.parse();
    }
    void Wordly::getRandomWord(void)  {
        std::uniform_int_distribution<> dis(0, rs.size() - 1);
         this->word = rs[dis(gen)];
    }
            void Wordly::parseFile(void) {
        std::string buffer;
        while(getline(ss, buffer)) {
            if(isEmpty(buffer)) continue;

            rs.push_back(buffer);
            dictionary.insert(buffer);
         }

    }
    bool Wordly::lengthChecker(void) const {
        for(const auto &c : history[activeY]) {
            if(!isalpha(c.c)) return false;
        }

        return true;
    }
    Color Wordly::getColor(const Type & t)const {
        switch(t) {
            case INCORRECT_POS :
            return YELLOW;

            case CORRECT_POS :
            return GREEN;

            default: 
            return GRAY;
        }
        return GRAY;
    }
    Color Wordly::applyColor(std::string_view toCheck) const {
        if(toCheck == "RED") return RED;
        else if(toCheck == "BLUE") return BLUE;
        else if(toCheck == "GREEN") return GREEN;
        else if(toCheck == "YELLOW") return YELLOW;
        else if(toCheck == "BLACK") return BLACK;
        else if(toCheck == "PINK") return PINK;
        else if(toCheck == "VIOLET") return VIOLET;
        return BLACK;
    }
    void Wordly::clearHistory(void) {
        std::for_each(history.begin(), history.end(), [] (std::array<Character, 5> & arr) {
            return arr.fill(Character(' ', NOT_IN));
        });
    }
    size_t Wordly::getLength(const std::string & str) const {return str.length();}

    void Wordly::readConfig(void) {
        std::ifstream config("../src/config.conf");
        if(!config.is_open()) {
            throw std::runtime_error("Config file was not found");
        }
        std::string buffer;

        while(getline(config, buffer)) {
            size_t toFind;
            if((toFind = buffer.find("BG_COLOR=")) != std::string::npos){
                buffer = buffer.substr(getLength("BG_COLOR="));
                this->config.bg_color = applyColor(buffer);
            }
            else if((toFind = buffer.find("GRID_COLOR=")) != std::string::npos){
                buffer = buffer.substr(getLength("GRID_COLOR="));
                this->config.grid_color = applyColor(buffer);
            }
             else if((toFind = buffer.find("TEXT_COLOR=")) != std::string::npos){
                buffer = buffer.substr(getLength("TEXT_COLOR="));
                this->config.text_color = applyColor(buffer);
            }
            else if((toFind = buffer.find("HARD_MODE=")) != std::string::npos){
                buffer = buffer.substr(getLength("HARD_MODE="));
                if(buffer == "TRUE" || buffer == "ENABLED") this->config.hardMode = true;
                else this->config.hardMode  = false;
            }
            else if((toFind = buffer.find("AUTOPLAY=")) != std::string::npos){
                buffer = buffer.substr(getLength("AUTOPLAY="));
                if(buffer == "TRUE" || buffer == "ENABLED") this->config.autoplay = true;
                else this->config.autoplay  = false;
            }
        }
    }
    void Wordly::drawError(const std::string & msg) const {
        DrawText(msg.c_str(), 40, 680, 20, RED);
    }

      bool Wordly::wordChecker(void) {
            if(!lengthChecker()) return false;
            std::string toCheck;
         for(const auto & c : history[activeY]) {
            toCheck += c.c;
         }
         if(this->config.hardMode) {
            for(const char &c : this->mustUsedChars) {
                if(toCheck.find(c) == std::string::npos) {
                    renderErrorMessage = true;
                    errorMessage = "You must use ";
                    errorMessage += " letter";
                    errorMessage += c;
                    errorMessage += " in your word";
                    return false;
                }
            }
         }
         if(!dictionary.contains(toCheck)) {
            errorMessage = "This word does not exists in our database";
            renderErrorMessage = true;
            shakeTimer = 0.5f;
            return false;
         }
         renderErrorMessage = false;
        size_t idx = 0;
        for(auto & c : history[activeY]) {
            if(this->word.find(c.c) == std::string::npos) {
                c.type = NOT_IN;
                auto it = std::find_if(keyboard.begin(), keyboard.end(), [c](const Key & target) {
                    return target.c[0] == c.c;
                });

                if(it != keyboard.end()) {
                    it->status = INVALID;
                }
            }
            else if(this->word[idx] == c.c) {
                 c.type  = CORRECT_POS;
                 if(this->config.hardMode) {
                    this->mustUsedChars.insert(c.c);
                 }
                 auto it = std::find_if(keyboard.begin(), keyboard.end(), [c](const Key & target) {
                    return target.c[0] == c.c;
                });

                if(it != keyboard.end()) {
                    it->status = CORRECT;
                }
            }
            else  {
                c.type = INCORRECT_POS;
                 if(this->config.hardMode) {
                    this->mustUsedChars.insert(c.c);
                 }
                 auto it = std::find_if(keyboard.begin(), keyboard.end(), [c](const Key & target) {
                    return target.c[0] == c.c;
                });

                if(it != keyboard.end()) {
                    it->status = INCORRECT;
                }
            }
            idx++;
        }
        activeX = 0;
        if(activeY < 6) {
             activeY++;
        }  
        attempts++;
        if(toCheck == word) {
            auto current = usersHistory.getValue<int>("current_streak");
            usersHistory.updateValue<std::string>("current_streak", std::to_string(current.value() + 1));
            auto best = usersHistory.getValue<int>("best_streak");
            if(current.has_value() && best.has_value()) {
                if(current.value() + 1 > best.value()) {
                    usersHistory.updateValue<std::string>("best_streak", std::to_string(current.value() + 1));
                }
            }
            auto x  = usersHistory.getValue<int>("total_games");
            auto y = usersHistory.getValue<int>("wins");
            if(x.has_value()) {
                usersHistory.updateValue<std::string>("total_games",std::to_string(x.value() + 1));
            }
            if(y.has_value()) {
                usersHistory.updateValue<std::string>("wins",std::to_string(y.value() + 1));
            }
            auto guessDistribution = usersHistory.getObject("guess_distribution");
            if(guessDistribution.has_value()) {
                auto currentValue =  guessDistribution.value().getValue<int>(std::to_string(attempts));
                if(currentValue.has_value()) {
                    guessDistribution.value().updateValue<std::string>(std::to_string(attempts), std::to_string(currentValue.value() + 1));
                    usersHistory.updateValue<std::string>("guess_distribution", guessDistribution->toString());
                }
                
            }
            
            pendingGameOver = true;
            timer = 2.0f;
            usersHistory.stringify();

        }
        else if(attempts == 6) {
            auto x  = usersHistory.getValue<int>("total_games");
            auto y = usersHistory.getValue<int>("losses");
            auto current = usersHistory.getValue<int>("current_streak");
            usersHistory.updateValue<std::string>("current_streak", "0");
            auto best = usersHistory.getValue<int>("best_streak");
            if(current.has_value() && best.has_value()) {
                if(current.value() > best.value()) {
                    usersHistory.updateValue<std::string>("best_streak", std::to_string(current.value()));
                }

            }
            if(x.has_value()) {
                usersHistory.updateValue<std::string>("total_games",std::to_string(x.value() + 1));
            }
            if(y.has_value()) {
                usersHistory.updateValue<std::string>("losses",std::to_string(y.value() + 1));
            }
            
            pendingGameOver = true;
            timer = 2.0f;
            usersHistory.stringify();
        }
        userWon = toCheck == word;
        return toCheck == word;
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
    if(state == DAILY_CHALLENGE || state == PRACTICE) {
    mainTimer.update();
    drawTimer();
    drawLogo();
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
}
 }
void Wordly::updateCurrentWord(const char & c) {
if(activeX < 5 && activeY < 6) {
history[activeY][activeX] = Character(c, NOT_IN);
activeX++;
}

}

void Wordly::backspace(void) {
    if(activeX > 0) {
        activeX--;
        history[activeY][activeX] = Character(' ', NOT_IN);
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
        keyboard.clear();
        initKeyboard();
        mainTimer.start();
    }

    Rectangle box2 =  {280, 500, 120, 30};
    std::string text2 = "Exit";
    Button exit = drawBtn(box2, text2, PINK);

    if(exit.checkClick(GetMousePosition())) {
        std::exit(0);
    }
}
void Wordly::setGameOver(void) {
    gameOver = true;
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
         x.box = Rectangle(posX, posY, CELL_SIZE,CELL_SIZE);
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

void Wordly::writeKey(void) {
for(const auto & x : keyboard) {
    if(x.clickStatus()) {
        if(x.c == "DEL") {
            if(activeX > 0) {
                history[activeY][--activeX].c = ' ';
            }
        }
        else if(x.c == "ENT") {
            if(activeX == 5) {
                if(wordChecker()) {
               setGameOver();
            }
            } else shakeTimer = 0.5f;
        }
        else if(activeX < 5) {
            history[activeY][activeX++].c = x.c[0];
        }
    }
}
}
bool Wordly::getAutoplayStatus(void) const {
    return this->config.autoplay;
}
std::string Wordly::generateTheMostAccurateWord(void) const {
std::string notInWord;
std::map<int, char> incorrectPosition;
std::map<int, char> correct;
std::vector<std::string> total;
std::string buffer;
for(const auto & row : history) {

        buffer.clear();
    for(int i = 0; i < row.size(); i++) {
        buffer += row[i].c;
        if(row[i].type == NOT_IN) {
            bool needed = false;
            for(const auto & cell : row) {
                if((cell.c == row[i].c) && (cell.type == INCORRECT_POS || cell.type == CORRECT_POS)) needed = true;
            }
            if(notInWord.find(row[i].c) == std::string::npos &&!needed ) {
                 notInWord += row[i].c;
            }

           
        }
        else if(row[i].type == INCORRECT_POS) {
            if(!incorrectPosition.contains(row[i].c)) {
            incorrectPosition.insert({i, row[i].c});
        }
    }
        else if(row[i].type == CORRECT_POS) {
           correct.insert({i, row[i].c});
        }
    }
    total.push_back(buffer);
}

auto mostProbableWord = std::find_if(rs.begin(), rs.end(), [notInWord, incorrectPosition, correct, total] (const std::string &w) {
    for(const auto c : total) {
        if(w == c) return false;
    }
for(const char & c : notInWord) {
    if(w.find(c) != std::string::npos) return false;
}

for(const auto & c : incorrectPosition) {
    if(w.find(c.second) == std::string::npos || w[c.first] == c.second) return false;
}

for(auto & c : correct) {
    if(w[c.first] != c.second) return false;
}

return true;

});
if(mostProbableWord != rs.end())  return *mostProbableWord;
return "";
}
void Wordly::autoBotPlay(void) {
    if(activeY == 6 || pendingGameOver) return;
    if(botTimer >= 0) {
        botTimer -= GetFrameTime();
        return;
    }
    std::string target;
    if(activeY ==0) {
        std::uniform_int_distribution<> dis(0, rs.size());
        target = rs[dis(gen)];
    }
    else {
target = generateTheMostAccurateWord();
    }
    std::uniform_int_distribution<> dis(0, rs.size());
    if(activeX == 0) {
    for(int i = 0; i < 5; i++) {
    history[activeY][activeX++].c = target[i];  
    }
    if(wordChecker()) {
        
    }
    else botTimer = 0.8f;
}
}