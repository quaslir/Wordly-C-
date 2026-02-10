#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>
#include "wordly.hpp"
#include "raylib.h"
std::random_device rd;
std::mt19937 gen(rd());
enum Type {CORRECT_POS, INCORRECT_POS, NOT_IN};

struct Character {
    char c;
Type type;

Character(char ch, Type t) : c(ch),type(t) {}
};
class Wordly {
    private :
    std::vector<std::string> rs;
    std::istream & ss;
    size_t attempts;
    std::vector<Character> w;
    std::vector<std::vector<Character>> history;

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

    bool wordChecker(std::string_view wr) {
        w.clear();
        size_t idx = 0;
        for(const auto & c : wr) {
            if(this->word.find(c) == std::string::npos) w.push_back(Character(c, NOT_IN));
            else if(this->word[idx] == c) w.push_back(Character(c, CORRECT_POS));
            else w.push_back(Character(c, INCORRECT_POS));
            idx++;
        }
        history.push_back(w);
        this->print();
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
    void print(void) {
        clearScreen();
        for(const auto & y : this->history) {
            for(const auto &x : y) {
            if(x.type == CORRECT_POS) {
                std::cout  << x.c;
            }
            else if(x.type == INCORRECT_POS) {
                std::cout << x.c ;
            }

            else {
                std::cout << x.c;
            }
        }
        std::cout << std::endl;
    }
    }
    public :
    std::string word;
    Wordly(std::istream & s) : ss(s), attempts(0) {
        this->parseFile();
        this->getRandomWord();
    }

    void input(void) {
        std::string str;
        
        getline(std::cin, str);
        while(!this->handleInput(str)) {
            std::cerr << "Incorrect input, please try again" << std::endl;
            getline(std::cin, str);
        }
                
        wordChecker(str);
        }
};
 void draw(void) {
       for(size_t i = 1; i < 6; i++) {
        for(size_t j = 1; j < 5; j++) {
        float calculateX = (float) (j * 70 * 1.1);
        float calculateY =  (float) (i * 70 * 1.1);

        Rectangle box = {(float) calculateX,calculateY, 70, 70};

        float thickness = 3.0f;

        DrawRectangleLinesEx(box, thickness, GREEN);
       }
       }
    }
int main(int argc, char * argv[]) {
    InitWindow(500, 500, "Worldy-C++");
    SetTargetFPS(120);

    while(!WindowShouldClose()) {
        BeginDrawing();
        DrawText("Wordly-C++",140,5,32,GREEN);
            ClearBackground(BLACK);
        draw();
        EndDrawing();
}
CloseWindow();
    return 0;
} 