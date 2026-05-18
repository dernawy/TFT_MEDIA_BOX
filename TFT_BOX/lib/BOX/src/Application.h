#ifndef APPLICATION_H
#define APPLICATION_H


#include "FilesPath.h"
#include "config/definitions.h"




class Application : public definitions, public SpiffsIo, public wifi, public ServerTime, public display, public sensors, public tft_tools {
    
    private:
        /* data */
    public:
        Application(/* args */);
        ~Application();

        void init_application();


};

    
extern Application APP;
extern Application app;



#endif /* APPLICATION_H */
