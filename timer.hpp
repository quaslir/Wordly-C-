#include "raylib.h"
#include <format>
class Timer {
    private :
    float minutes, seconds;
    bool active;

    public:

    Timer();

    float getMins(void) const;
    float getSeconds(void) const;
    void update(void);
    void start(void);
    void stop(void);
    std::string getCurrentTime(void) const;
};