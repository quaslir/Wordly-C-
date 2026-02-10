#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>
#include "colors.hpp"
#include "wordly.hpp"
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
                std::cout << Color::green << x.c << Color::reset;
            }
            else if(x.type == INCORRECT_POS) {
                std::cout << Color::yellow << x.c << Color::reset;
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

int main(int argc, char * argv[]) {
    clearScreen();
    std::cout << Color::violet;
    std::cout << R"(                       _ _                             
__      _____  _ __ __| | |_   _        ___  _     _   
\ \ /\ / / _ \| '__/ _` | | | | |_____ / __|| |_ _| |_ 
 \ V  V / (_) | | | (_| | | |_| |_____| (_|_   _|_   _|
  \_/\_/ \___/|_|  \__,_|_|\__, |      \___||_|   |_|  
                           |___/                       
)";
std::cout << Color::reset;
    if(argc < 2) {
        std::cerr << "File was not provided" << std::endl;
        return 1;
    }
    std::ifstream file (argv[1]);
    if(!file.is_open()) {
        std::cerr << "File could not be found" << std::endl;
        return 1;
    }

    Wordly wordly (file);
    bool gameOver = false;
    std::cout << wordly.word << std::endl;
    while(!gameOver) {
        wordly.input();
    }
    return 0;
} 