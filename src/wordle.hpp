#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>
#include "raylib.h"
#include <array>
#include <unistd.h>
#include <unordered_set>
#include <algorithm>
#include "../json-parser-c/parser.hpp"
#include "keyboard.hpp"
#include <map>
#include "timer.hpp"
#include <exception>
#define SQUARE_SIZE 65
#define CELL_SIZE 35
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
bool autoplay;
Config() : bg_color(BLACK), grid_color(GREEN), text_color(GREEN), hardMode(false), autoplay(false) {}
};

enum gameState_t {
    MAIN_MENU,
    AUTOPLAY,
    DAILY_CHALLENGE,
    PRACTICE
};
class Wordly {
    private :
    std::unordered_set<std::string> dictionary;
    std::vector<std::string> rs;
    std::istream & ss;
    size_t attempts = 0;
    std::array<std::array<Character, 5>, 6> history;
    std::vector<Key> keyboard;
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
    float timer = 0.0f;
    bool pendingGameOver = false;
    float botTimer = 0.0f;
    Timer mainTimer;
    gameState_t state;
    std::string username;
    bool isEmpty(std::string_view str) const;
    void initHistoryFile(void);
    bool handleInput(std::string_view word) const;

        void initKeyboard(void);
    int centerTextByX(const std::string & text, int fontSize, int width, int marginX) const;
    void getRandomWord(void);
    void writeKey(void);
    void parseFile(void);
    void renderKeyBoard(void) ;
    bool lengthChecker(void) const;
    std::string generateTheMostAccurateWord(void) const;
    Color getColor(const Type & t)const;

    Color applyColor(std::string_view toCheck) const;

    void clearHistory(void);

    size_t getLength(const std::string & str) const;
    void readConfig(void);
    void drawError(const std::string & msg) const;
    void initHistory(void);
    void drawTimer(void) const;
    void drawGuessDistribution(const Rectangle & rec) const;
    ParserJSON usersHistory {"../history.json"};
    void drawFrontScreen(void);
    void drawLogo(void) const;
    public :
            Config config;
        bool wordChecker(void);

    Wordly(std::istream & s); 

 void draw(void);
void autoBotPlay(void);
void updateCurrentWord(const char & c);
bool getAutoplayStatus(void) const;
void backspace(void);

Button drawBtn(const Rectangle & box, const std::string & text, const Color & color) const;

void gameOverScreenRenderer(void);
void setGameOver(void);

};