#include "wordle.hpp"
void Wordly::trim(std::string & text) const {
text.erase(0, text.find_first_not_of(" \t\r\n"));
text.erase(text.find_last_not_of(" \t\r\n") + 1);
}
void Wordly::setUsername(void) {
    static std::string buffer{""};

    int key = GetCharPressed();
        while(key > 0) {
            if((key >= 32) && (key <= 125)) {
            buffer += (char) key;
   
            }
            key = GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE)) {
            buffer.pop_back();
        }

        if(IsKeyPressed(KEY_ENTER)) {
            trim(buffer);
            if(buffer.empty()) return;
            this->username = buffer;
            state = MAIN_MENU;
            try {
               usersHistory.updateValue<std::string>("username", username);
               usersHistory.stringify();
            } catch(...) {
                std::cerr << "Json data was corrupted, could not update username" << std::endl;
            }
            buffer.clear();
        }

}