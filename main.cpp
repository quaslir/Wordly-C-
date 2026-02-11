#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>
#include "wordly.hpp"
#include "raylib.h"
#include <array>
std::random_device rd;
std::mt19937 gen(rd());
enum Type {CORRECT_POS, INCORRECT_POS, NOT_IN};

struct Character {
    char c;
Type type;

Character(char ch, Type t) : c(ch),type(t) {}
Character() : c(' '), type(NOT_IN) {}
};
class Wordly {
    private :
    std::vector<std::string> rs;
    std::istream & ss;
    size_t attempts = 0;
    std::array<std::array<Character, 5>, 6> history;
    int activeX = 0;
    int activeY = 0;
    bool isEmpty(std::string_view str) {
        return str.empty() || str.find_first_not_of(" \t\r\n") == std::string::npos;
    }

    bool handleInput(std::string_view word) const{
        if(word.length() != 5) return false;

        for(const auto & x : word) {
            if(!isalpha(x)) return false;
        }
        return true;
    }


    void getRandomWord(void)  {
        std::uniform_int_distribution<> dis(0, rs.size());
         this->word = rs[dis(gen)];
    }
            void parseFile(void) {
        std::string buffer;
        while(getline(ss, buffer)) {
            if(this->isEmpty(buffer)) continue;

            rs.push_back(buffer);
         }
    }
    bool lengthChecker(void) {
        for(const auto &c : history[activeY]) {
            if(!isalpha(c.c)) return false;
        }

        return true;
    }
    Color getColor(const Type & t)const {
        switch(t) {
            case INCORRECT_POS :
            return YELLOW;

            case CORRECT_POS :
            return GREEN;
        }
        return GRAY;
    }
    public :
    std::string word;
        bool wordChecker(void) {
            if(!lengthChecker()) return false;
            std::string toCheck;
        size_t idx = 0;
        for(auto & c : history[activeY]) {
            if(this->word.find(c.c) == std::string::npos) c.type = NOT_IN;
            else if(this->word[idx] == c.c) c.type  = CORRECT_POS;
            else c.type = INCORRECT_POS;
            idx++;
            toCheck += c.c;
        }
        activeX = 0;
        if(activeY < 6) {
             activeY++;
        }
       

        return toCheck == word;
    }
    Wordly(std::istream & s) : ss(s), attempts(0) {
        this->parseFile();
        this->getRandomWord();
        std::cout << word << std::endl;
    }

 void draw(void) {
    std::string buf;
       for(size_t i = 0; i < 6; i++) {
        for(size_t j = 0; j < 5; j++) {
            buf.clear();
            auto c = this->history[i][j];
            buf += c.c;
        float calculateX = (float) (j * 70 * 1.1);
        float calculateY =  (float) (i * 70 * 1.1);
        Rectangle box = {(float) calculateX,calculateY, 70, 70};
            Vector2 textSize = MeasureTextEx(GetFontDefault(), buf.c_str(), 40.f, 2);
            float textX = box.x + (box.width / 2) - (textSize.x / 2);
            float textY = box.y + (box.height / 2) - (textSize.y / 2);
            DrawText(buf.c_str(), (int) textX, (int) textY, 40, getColor(c.type));
        float thickness = 3.0f;

        DrawRectangleLinesEx(box, thickness, GREEN);
       }
       }
    }

void updateCurrentWord(const char & c) {
if(activeX < 5 && activeY < 6) {
history[activeY][activeX] = Character(c, NOT_IN);
activeX++;
}

}

void backspace(void) {
    if(activeX > 0) {
        activeX--;
        history[activeY][activeX] = Character(' ', NOT_IN);
    }

}

void gameOverScreenRenderer(void) {
    std::string text = "You guessed the word by " + std::to_string(attempts) + " attempts!";
    DrawText(text.c_str(), 10, 10, 40, GREEN);
}

};

int main(int argc, char * argv[]) {
    std::ifstream file (argv[1]);
    Wordly wordly (file);
    InitWindow(500, 500, "Worldy-C++");
    SetTargetFPS(120);
    while(!WindowShouldClose()) {
        BeginDrawing();
        //DrawText("Wordly-C++",140,5,32,GREEN);
            ClearBackground(BLACK);
        wordly.draw();

        int key = GetCharPressed();
        while(key > 0) {
            if((key >= 32) && (key <= 125)) {
            wordly.updateCurrentWord((char) key);
   
            }
            key = GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE)) {
            wordly.backspace();
        }
        if(IsKeyPressed(KEY_ENTER)){
            if(wordly.wordChecker()) break;
        }
        EndDrawing();
        
}
CloseWindow();
    return 0;
} 