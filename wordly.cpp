#include "wordly.hpp"
std::random_device rd;
std::mt19937 gen(rd());
bool Wordly::isEmpty(std::string_view str) const{
        return str.empty() || str.find_first_not_of(" \t\r\n") == std::string::npos;
    }

    bool Wordly::handleInput(std::string_view word) const{
        if(word.length() != 5) return false;

        for(const auto & x : word) {
            if(!isalpha(x)) return false;
        }
        return true;
    }


    void Wordly::getRandomWord(void)  {
        std::uniform_int_distribution<> dis(0, rs.size());
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
        else if(toCheck == "BLUE") return BLUE;
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
        std::ifstream config("../config.conf");
        if(!config.is_open()) {
            std::cout << "NOT FOUND" << std::endl;
            return;
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
        }
    }
    void Wordly::drawError(void) const {
        DrawText("This word does not exists in our database", 40, 580, 20, RED);
    }

      bool Wordly::wordChecker(void) {
            if(!lengthChecker()) return false;
            std::string toCheck;
         for(auto & c : history[activeY]) {
            toCheck += c.c;
         }
         if(!dictionary.contains(toCheck)) {
            renderErrorMessage = true;
            return false;
         }
         renderErrorMessage = false;
        size_t idx = 0;
        for(auto & c : history[activeY]) {
            if(this->word.find(c.c) == std::string::npos) c.type = NOT_IN;
            else if(this->word[idx] == c.c) c.type  = CORRECT_POS;
            else c.type = INCORRECT_POS;
            idx++;
        }
        activeX = 0;
        if(activeY < 6) {
             activeY++;
        }  
        attempts++;
        if(attempts == 6) {
            gameOver = true;

        }
         userWon = toCheck == word; 
        return toCheck == word;
    }

 void Wordly::draw(void) {
    DrawText("Wordly-C++",125,20,50,config.text_color);
    if(!gameOver) {
    std::string buf;
       for(size_t i = 0; i < 6; i++) {
        for(size_t j = 0; j < 5; j++) {
            buf.clear();
            auto c = this->history[i][j];
            buf += c.c;
        float calculateX = (float) ((j * 70 * 1.1) + 75);
        float calculateY =  (float) ((i * 70 * 1.1) + 100);
        Rectangle box = {(float) calculateX,calculateY, 70, 70};
            Vector2 textSize = MeasureTextEx(GetFontDefault(), buf.c_str(), 40.f, 2);
            float textX = box.x + (box.width / 2) - (textSize.x / 2);
            float textY = box.y + (box.height / 2) - (textSize.y / 2);
            DrawText(buf.c_str(), (int) textX, (int) textY, 40, getColor(c.type));
        float thickness = 3.0f;

        DrawRectangleLinesEx(box, thickness, this->config.grid_color);
       }
       }
    } else {
        gameOverScreenRenderer();
    }

    if(renderErrorMessage) {
        drawError();
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
    Button btn (box, PINK, text);

    DrawRectangle(btn.btn.x, btn.btn.y, btn.btn.width, btn.btn.height, btn.color);
    DrawText(btn.text.c_str(),btn.btn.x + 10, btn.btn.y + 5, 20, BLACK);
    return btn;
}
void Wordly::gameOverScreenRenderer(void) {
    if(userWon) {
    std::string text = "You guessed the word by " + std::to_string(attempts);
    std::string next = attempts == 1 ? " attempt" : " attempts";
    text += next;
    DrawText(text.c_str(), 25, 90, 25, this->config.text_color);   
    } else {
        std::string text = "Unfortunately, you did not guess the\nword " + word + " within 6 attempts";
        DrawText(text.c_str(), 25, 90, 23, this->config.text_color);   
    }
    Rectangle box = {120, 150, 120, 30};
    std::string text = "Play again";
    Button playAgain = drawBtn(box, text, PINK);
        if(playAgain.checkClick(GetMousePosition())) {
        gameOver = false;
        clearHistory();
        activeX = 0;
        activeY = 0;
        attempts = false;
        getRandomWord();
    }

    Rectangle box2 =  {255, 150, 120, 30};
    std::string text2 = "Exit";
    Button exit = drawBtn(box2, text2, PINK);

    if(exit.checkClick(GetMousePosition())) {
        std::exit(0);
    }
}
void Wordly::setGameOver(void) {
    gameOver = true;
}