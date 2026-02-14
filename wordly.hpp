#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>
#include "button.hpp"
#include "raylib.h"
#include <array>
#include <unistd.h>
#include <unordered_set>
#include <algorithm>
#include "parser.hpp"
enum Type {CORRECT_POS, INCORRECT_POS, NOT_IN};

struct Character {
    char c;
Type type;
Character(char ch, Type t) : c(ch),type(t) {}
Character() : c(' '), type(NOT_IN) {}
};

struct Config {
Color bg_color;
Color grid_color;
Color text_color;
bool hardMode;
Config() : bg_color(BLACK), grid_color(GREEN), text_color(GREEN), hardMode(false) {}
};

class Wordly {
    private :
    std::unordered_set<std::string> dictionary;
    std::vector<std::string> rs;
    std::istream & ss;
    size_t attempts = 0;
    std::array<std::array<Character, 5>, 6> history;
    int activeX = 0;
    int activeY = 0;
    bool renderErrorMessage = false;
    std::string errorMessage = "";
    bool gameOver = false;
    std::string word;
    bool userWon;
    std::unordered_set<char> mustUsedChars;
    float shakeTimer = 0.0f;
    float shakeIntensity = 10.f;
    bool isEmpty(std::string_view str) const;

    bool handleInput(std::string_view word) const;


    void getRandomWord(void);

    void parseFile(void);

    bool lengthChecker(void) const;

    Color getColor(const Type & t)const;

    Color applyColor(std::string_view toCheck) const;

    void clearHistory(void);

    size_t getLength(const std::string & str) const;

    void readConfig(void);
    void drawError(const std::string & msg) const;
    void initHistory(void);

    public :
        ParserJSON usersHistory {"../history.json"};
    Config config;
        bool wordChecker(void);

    Wordly(std::istream & s) : ss(s) {
        this->parseFile();
        this->getRandomWord();
        this->readConfig();
        this->initHistory();
    }

 void draw(void);

void updateCurrentWord(const char & c);

void backspace(void);

Button drawBtn(const Rectangle & box, const std::string & text, const Color & color) const;

void gameOverScreenRenderer(void);
void setGameOver(void);

};