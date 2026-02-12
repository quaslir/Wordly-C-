#include "raylib.h"
#include <string>
struct Button {
    Rectangle btn;
    Color color;
    std::string text;
    bool isClicked;

    bool checkClick(const Vector2 & pos) const;
    
    Button (const Rectangle & rec, const Color & c, const std::string & t) :
    btn(rec), color(c), text(t), isClicked(false) {}
};