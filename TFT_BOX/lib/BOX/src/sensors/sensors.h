#ifndef SENSORS_H
#define SENSORS_H

#include "Arduino.h"
#include "sensors/sensors_helper.h"

class sensors {
    private:
        bool _bmp_started;
        

        bool _gyro_started;
        const char *_acc_range;
        const char *_gyro_range;
        const char *_gyro_bandwidth;
        
        
    public:
        sensors(/* args */);
        ~sensors();

        sensors_init_t init_sensors();
        bool getBmpStartStatus();
        bool getGyroStartStatus();

        void setTempUnit(int unit);
        int getTempUnit();
};




#endif /* SENSORS_H */