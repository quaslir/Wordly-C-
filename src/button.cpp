#include "button.hpp"

bool Button::checkClick(const Vector2 & pos)const{
        if(CheckCollisionPointRec(pos, btn)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                return true;
            }
        }
        return false;
    }
bool Button::checkHover(const Vector2 & pos) const {
if(CheckCollisionPointRec(pos, btn)) {
    return true;
}
return false;
}

Button Button::drawBtn(const Rectangle & box, const std::string & text, const Color & color) const{
    Button btn (box, color, text);

    DrawRectangle(btn.btn.x, btn.btn.y, btn.btn.width, btn.btn.height, btn.color);
    DrawText(btn.text.c_str(),btn.btn.x + 10, btn.btn.y + 5, 20, BLACK);
    return btn;
}