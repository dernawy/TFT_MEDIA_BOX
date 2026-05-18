#ifndef WIFI_H
#define WIFI_H 

#include <Arduino.h>

class wifi {

    private:
        /* data */
    public:
        wifi(/* args */);
        ~wifi();

        void socketServerListen();
        bool socketServerPoll();
        void serverPollLoop();
        void drawingTimeText(uint32_t deltaT, int yPos);

};





#endif /* WIFI_H */