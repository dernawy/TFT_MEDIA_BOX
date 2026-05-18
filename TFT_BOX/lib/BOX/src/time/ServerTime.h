#ifndef SERVERTIME_H
#define SERVERTIME_H


#include "time/Server_time_helper.h"

class ServerTime {

    private:
        /* data */

        long  gmtOffset_sec;
        int   daylightOffset_sec;
        const char * default_server;
        const char *timeApiUrl;
        bool use_internal_time;
        bool set_internal_time_by_ntp;

        server_time_internet_connection_t start_update_time_wifi();
        void _do_init(server_time_config_t config);


    public:
        ServerTime(/* args */);
        ~ServerTime();

        NTPClient timeClient();

        bool setOffset(long val);
        bool setDayLight(int val);

        void init_time_wifi();
        void init_server_time();
        void server_time_loop();
};




#endif /* SERVERTIME_H */
