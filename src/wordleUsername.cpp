#include "wordle.hpp" 
void Wordly::trim(std::string & text) const {

text.erase(0, text.find_first_not_of(" \t\r\n"));
text.erase(text.find_last_not_of(" \t\r\n") + 1);

}
void Wordly::checkUsername(std::string & buffer) {
            trim(buffer);
            if(buffer.empty()) return;
            this->username = buffer;
            state = MAIN_MENU;
            try {
               usersHistory.updateValue<std::string>("username", username);
               usersHistory.stringify("../history.json");
            } catch(...) {
                std::cerr << "Json data was corrupted, could not update username" << std::endl;
            }
            buffer.clear();
}

void Wordly::setUsername(void) {
    float boxW = 200.0f;
    float boxH = 40.0f;
    float startX = (GetScreenWidth() - boxW) / 2.0f;
    float startY = GetScreenHeight() / 2.0f - 50.0f;

    static std::string buffer{""};

    Rectangle rec = {startX, startY, boxW, boxH};
    Rectangle rec2 = {startX, startY + boxH + 15, boxW, boxH};

    DrawText("ENTER YOUR NAME", centerTextByX("ENTER YOUR NAME", 20, GetScreenWidth(), 0), startY - 40, 20, LIGHTGRAY);
    Color boxColor = buffer.size() > 0 ? RAYWHITE : GRAY;
    drawTextBox(rec, 2.0f, boxColor, 20, buffer);
    bool toSubmit = !buffer.empty();
    Color btnColor = toSubmit ? Color{83,141,78, 255} : DARKGRAY;

    Button btn;
    btn = btn.drawBtn(rec2, "Set username", btnColor);

    if(toSubmit && btn.checkClick(GetMousePosition())) {
            checkUsername(buffer);
    }
    int key = GetCharPressed();
        while(key > 0) {
            if((key >= 32) && (key <= 125)) {
            buffer += (char) key;
                
            }
            key = GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE)) {
            if(buffer.size() > 0) {
                buffer.pop_back();
            }
        }

        if(IsKeyPressed(KEY_ENTER)) {
           checkUsername(buffer);
        }

}