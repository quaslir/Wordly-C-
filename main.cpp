#include "wordly.hpp"
int main(int argc, char * argv[]) {
    std::ifstream file (argv[1]);
    Wordly wordly (file);
    InitWindow(540, 730, "Worldy-C++");
    SetTargetFPS(120);
    bool autoplay = wordly.getAutoplayStatus();
    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(wordly.config.bg_color);
        wordly.draw();
        if(autoplay) {

            //wordly.autoBotPlay();
        } else {
        int key = GetCharPressed();
        while(key > 0) {
            if((key >= 32) && (key <= 125)) {
            wordly.updateCurrentWord((char) key);
   
            }
            key = GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE)) {
            wordly.backspace();
        }
        if(IsKeyPressed(KEY_ENTER)){
            if(wordly.wordChecker()) {
               wordly.setGameOver();
            }
        }
    }
        EndDrawing();
}
CloseWindow();
    return 0;
} 