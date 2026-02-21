#include "raylib.h"
#include <string>
inline void drawLogo(void) {
    const std::string text = "WORDLE";
    int space = 10;
    int squareSize = 50;
        int x = (GetScreenWidth() / 2) - (((6 * squareSize) + (5 * space)) / 2);
        int y = 15;
        int fontSize = 40;
    static std::string buffer;
    Color squareColor = {83, 141, 78, 255};
    for(int i = 0; i < text.size(); i++) {
      squareColor = (i % 2 == 0) ? Color{201, 180,88,255} : Color{83, 141, 78, 255};
        buffer.clear();
        buffer += text[i];
        int currentX = x + (i * (squareSize + space));
   DrawRectangle(currentX, y, squareSize, squareSize, squareColor);
   int textWidth = MeasureText(buffer.c_str(), fontSize);

   int textX  = currentX + (squareSize - textWidth) / 2;
   int textY = y + (squareSize - fontSize) / 2;
      DrawText(buffer.c_str(), textX + 2, textY + 2, fontSize, BLACK);
    DrawText(buffer.c_str(), textX, textY, fontSize, RAYWHITE);
    } 
    
}