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