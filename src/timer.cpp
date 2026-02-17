#include "timer.hpp"

float Timer::getMins(void) const {
    return this->minutes;
}
float Timer::getSeconds(void) const {
   return this->seconds;
}

Timer::Timer() {
minutes = 0.0f;
seconds = 0.0f;
active = false;
}

void Timer::start(void) {
minutes = 0.0f;
seconds = 0.0f;
active = true;
}

void Timer::update(void) {
    if(!active) return;
    if(seconds >= 60) {
        minutes++;
        seconds -= 60;
    }
    seconds += GetFrameTime();
}

std::string Timer::getCurrentTime(void) const {
return std::format("{:02}:{:02}", (int) minutes, (int) seconds);
}

void Timer::stop(void) {
    active = false;
}