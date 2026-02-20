#include "wordle.hpp"
std::random_device rd;
std::mt19937 gen(rd());

Wordly::Wordly(std::istream & s) : ss(s) {
    this->state = MAIN_MENU;
    this->initHistoryFile();
     this->initHistory();
    if(usersHistory.exists("username")) {
        username = usersHistory.getValue<std::string>("username").value();
        if(username.empty()) this->state = EMPTY_USERNAME;
        trim(username);
        if(username.empty()) this->state = EMPTY_USERNAME;
    }
    else this->state = EMPTY_USERNAME;
        this->parseFile();  

       if(usersHistory.exists("daily_challenge")) {

        auto x = usersHistory.getObject("daily_challenge");
        if(x.has_value()) {
                            
        auto first = x.value().getValue<bool>("daily_challenge_active");
        auto second = x.value().getValue<long>("daily_challenge_id");
        if(first.has_value() && second.has_value()) {

            dailyChallenge.first = first.value();
            dailyChallenge.second = second.value();
            if(!dailyChallenge.first) {
                long current = generateDayId();
                if(current != dailyChallenge.second) {
                    dailyChallenge.first = true;
                    dailyChallenge.second = current;
                    usersHistory.stringify();
                }
            }
        }
    }
       }
        this->initKeyboard();
           
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
    main.insert("username", "");
    main.insert("daily_challenge", "");
    ParserJSON dailyCh;
    dailyCh.insert("daily_challenge_active", false);
    dailyCh.insert("daily_challenge_id", 0);
    main.updateValue<std::string>("daily_challenge", dailyCh.toString());
    main.insert("total_xp", 0);
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
keyboard.clear();
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
    void Wordly::clearHistory(void) {
        std::for_each(history.begin(), history.end(), [] (std::array<Character, 5> & arr) {
            return arr.fill(Character(' ', NOT_IN));
        });
    }
    size_t Wordly::getLength(const std::string & str) const {return str.length();}
    

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
        if(this->config.autoplay && (toCheck == word || attempts == 6)) {
            pendingGameOver = true;
            timer = 2.0f;
        }
        else if(toCheck == word) {
            try {
               int totalXP = 1000;
                totalXP /= attempts;
                int timeBonus = (mainTimer.getMins() * 60 + mainTimer.getSeconds()) / 10;
                if(timeBonus > 1) {
                totalXP /= timeBonus;
                } 
                this->totalXp = totalXP;
                if(usersHistory.exists("total_xp")) {
                    usersHistory.updateValue<std::string>("total_xp",
                   std::to_string(usersHistory.getValue<int>("total_xp").value() + totalXP));
                }
              if(state == DAILY_CHALLENGE) updateDailyChallengeStatus();
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

        } catch(...) {
            std::cerr << "history.json file was corrupted, please delete this file" << std::endl;
        }
    } 
        else if(attempts == 6) {
            try {
                if(state == DAILY_CHALLENGE) updateDailyChallengeStatus();
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
        } catch(...) {
            std::cerr << "history.json file was corrupted, please delete this file" << std::endl;
        }
        }
        userWon = toCheck == word;
        return toCheck == word;
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
                wordChecker();
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
std::unordered_map<int, char> incorrectPosition;
std::unordered_map<int, char> correct;
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
    if(wordChecker());
    else botTimer = 0.8f;
}
}

long Wordly::generateDayId(void) const {
    auto now = std::chrono::system_clock::now();
    time_t formatted = std::chrono::system_clock::to_time_t(now);
    
    struct tm * parts = std::localtime(&formatted);

    return (parts->tm_year + 1900) * 1000 + (parts->tm_mon + 1) * 100 + parts->tm_mday;
}

void Wordly::getRandomWordDayChallenge(void) {
    long id = generateDayId();
    this->word = rs[id % rs.size()];
}


void Wordly::updateDailyChallengeStatus(void) {
    try {
        auto dailyCh = usersHistory.getObject("daily_challenge").value();
    dailyCh.updateValue<std::string>("daily_challenge_active", "false");
    dailyCh.updateValue<std::string>("daily_challenge_id", std::to_string(generateDayId()));
    usersHistory.updateValue<std::string>("daily_challenge", dailyCh.toString());
    dailyChallenge.first = false;
    } catch(...) {
        std::cerr << "Json file was corrupted" << std::endl;
    } 
}


void Wordly::clearVariables(void) {
                gameOver = false;
                clearHistory();
                totalXp = 0;
                activeX = 0;
                activeY = 0;
                attempts = 0;
                errorMessage = "";
                renderErrorMessage = false;
                mustUsedChars.clear();
                initKeyboard();
}