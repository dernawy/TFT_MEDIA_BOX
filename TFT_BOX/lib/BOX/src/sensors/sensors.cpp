#include "sensors.h"

#include "Application.h"

Adafruit_MPU6050 mpu;
Adafruit_BMP085 bmp;

int _c_f_unit                = 0; // 0 => °C -- 1 => °F
float _temp_c                = 0.0;
float _temp_f                = 0.0;
int32_t _pressure            = 0;
int32_t _sea_level_pressure  = 0;
float _altitude              = 0;

const float r1               = 100000.0f; //10000.0f; // R1 in ohm, 50K
const float r2               = 100000.0f; //10000.0f; // R2 in ohm, 10k potentiometer
float adcValue               = 0.0f;
float Vbatt                  = 0.0f;
uint64_t TimePastKalman      = esp_timer_get_time(); // used by the Kalman filter UpdateProcessNoise, time since last kalman calculation

int printCount               = 0;
float vRefScale              = (3.3f / 4096.0f) * ((r1 + r2) / r2);
TickType_t xLastWakeTime     = xTaskGetTickCount();
const TickType_t xFrequency  = 1000; //delay for mS

SimpleKalmanFilter KF_ADC_b( 1.0f, 1.0f, .01f );

sensors_event_t a, g, temp;

typedef struct {

    struct {
        float temp_c;
        float temp_f;
        int32_t pressure;
        int32_t sea_level_pressure;
        float altitude;
    } BMP085;

    struct {


    } MPU6050;

    

} sensors_info_t;

sensors_info_t s_info;

sensors::sensors(/* args */){
    _bmp_started         = false;
    _gyro_started        = false;
}
sensors::~sensors(){}

void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

bool sensors::getBmpStartStatus(){
    return _bmp_started;
}

bool sensors::getGyroStartStatus(){
    return _gyro_started;
}

void sensors::setTempUnit(int unit){
    _c_f_unit = unit;
}

int sensors::getTempUnit(){
    return _c_f_unit;
}

void vSensorsTaskCb(void * arg){

    Serial.println("\nSensors read task started\n");

    for(;;){

        tcaselect(0);
        _temp_c             = bmp.readTemperature();
        _temp_f             = ((_temp_c * 9/5) + 32);
        _pressure           = bmp.readPressure();
        _sea_level_pressure = bmp.readSealevelPressure();
        _altitude           = bmp.readAltitude();

        app.tft_main_page_control.temp_unit   = _c_f_unit;

        app.tft_main_page_control.elements[1] = (int)_temp_c;
        app.tft_main_page_control.elements[2] = (int)_temp_f;
        app.tft_main_page_control.elements[3] = _pressure;
        app.tft_main_page_control.elements[4] = _sea_level_pressure;
        app.tft_main_page_control.elements[5] = _altitude;

        tcaselect(1);
        mpu.getEvent(&a, &g, &temp);

        app.tft_main_page_control.elements[6]  = a.acceleration.pitch;
        app.tft_main_page_control.elements[7]  = a.acceleration.roll;
        app.tft_main_page_control.elements[8]  = a.acceleration.heading;

        app.tft_main_page_control.elements[9]  = g.gyro.pitch;
        app.tft_main_page_control.elements[10] = g.gyro.roll;
        app.tft_main_page_control.elements[11] = g.gyro.heading;

        app.tft_main_page_control.elements[12] = temp.temperature;

        //send_to_desktop_config_queu(&app.tft_main_page_control);

        vTaskDelay(50 / portTICK_PERIOD_MS);

    }

}

void fReadBattery( void * parameter ){

    Serial.println("Battry charge task started");
    delay(1000);

    for(;;){

        //adc1_get_raw(ADC1_CHANNEL_7); //read and discard
        adcValue = float( adc1_get_raw(ADC1_CHANNEL_7) ); //take a raw ADC reading

        KF_ADC_b.setProcessNoise( (esp_timer_get_time() - TimePastKalman) / 1000000.0f ); //get time, in microsecods, since last readings
        adcValue = KF_ADC_b.updateEstimate( adcValue ); // apply simple Kalman filter
        Vbatt    = (adcValue * vRefScale);
        
        printCount++;

        if ( printCount == 3 ){

            app.BAT_VOLT_STR    = String(Vbatt);
            app.BAT_PERCENT_STR = String((uint8_t)(((Vbatt - BATTV_MIN) / (BATTV_MAX - BATTV_MIN)) * 100));

            app.tft_main_page_control.elements[13]  = app.BAT_VOLT_STR;
            app.tft_main_page_control.elements[14]  = app.BAT_PERCENT_STR;

            if (xSemaphoreTake(app.g_send_lock, pdMS_TO_TICKS(app.wait_ticks)) != pdPASS) {
                return;
            }

            send_to_desktop_config_queu(&app.tft_main_page_control);

            xSemaphoreGive(app.g_send_lock);
            
            printCount = 0;

        }

        TimePastKalman = esp_timer_get_time(); // time of update complete
        xLastWakeTime  = xTaskGetTickCount();
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

    }

    vTaskDelete( NULL );

}

sensors_init_t sensors::init_sensors(){

    tcaselect(0);

    if(!bmp.begin()){
        Serial.println("Failed to find BMP085 chip");
        /*app.tft.setFreeFont(&DejaVu_Sans_Bold_11);
        app.tft.setTextColor(TFT_RED, TFT_BLUE);
        app.tft.drawString("BMP085 failed", 200, 170);*/
        _bmp_started = false;
    }
    else
    {
        Serial.println("BMP085 successfully inited");
        /*app.tft.setFreeFont(&DejaVu_Sans_Bold_11);
        app.tft.setTextColor(TFT_GREEN, TFT_BLUE);
        app.tft.drawString("BMP085 Found", 200, 170);*/
        _bmp_started = true;
    }
    
    tcaselect(1);

    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");

        /*app.tft.setFreeFont(&DejaVu_Sans_Bold_11);
        app.tft.setTextColor(TFT_RED, TFT_BLUE);
        app.tft.drawString("MPU6050 failed", 200, 150);*/
        _gyro_started = false;
        
    }
    else
    {
        Serial.println("MPU6050 successfully inited");
        /*app.tft.setFreeFont(&DejaVu_Sans_Bold_11);
        app.tft.setTextColor(TFT_GREEN, TFT_BLUE);
        app.tft.drawString("MPU6050 Found", 200, 150);*/
        _gyro_started = true;

        mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
        Serial.print("Accelerometer range set to: ");
        switch (mpu.getAccelerometerRange()) {
            case MPU6050_RANGE_2_G:
                Serial.println("+-2G");
                _acc_range = "+-2G";
            break;

            case MPU6050_RANGE_4_G:
                Serial.println("+-4G");
                _acc_range = "+-4G";
            break;

            case MPU6050_RANGE_8_G:
                Serial.println("+-8G");
                _acc_range = "+-8G";
            break;

            case MPU6050_RANGE_16_G:
                Serial.println("+-16G");
                _acc_range = "+-16G";
            break;
        }

        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        Serial.print("Gyro range set to: ");
        switch (mpu.getGyroRange()) {
            case MPU6050_RANGE_250_DEG:
            Serial.println("+-250 deg/s");
            _gyro_range = "+-250";
            break;

            case MPU6050_RANGE_500_DEG:
                Serial.println("+- 500 deg/s");
                _gyro_range = "+-500";
            break;
            case MPU6050_RANGE_1000_DEG:
                Serial.println("+- 1000 deg/s");
                _gyro_range = "+-1000";
            break;
            case MPU6050_RANGE_2000_DEG:
                Serial.println("+- 2000 deg/s");
                _gyro_range = "+-2000";
            break;
        }

        mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
        Serial.print("Filter bandwidth set to: ");
        switch (mpu.getFilterBandwidth()) {
            case MPU6050_BAND_260_HZ:
                Serial.println("260 Hz");
                _gyro_bandwidth = "260";
            break;

            case MPU6050_BAND_184_HZ:
                Serial.println("184 Hz");
                _gyro_bandwidth = "184";
            break;

            case MPU6050_BAND_94_HZ:
                Serial.println("94 Hz");
                _gyro_bandwidth = "94";
            break;

            case MPU6050_BAND_44_HZ:
                Serial.println("44 Hz");
                _gyro_bandwidth = "44";
            break;

            case MPU6050_BAND_21_HZ:
                Serial.println("21 Hz");
                _gyro_bandwidth = "21";
            break;

            case MPU6050_BAND_10_HZ:
                Serial.println("10 Hz");
                _gyro_bandwidth = "10";
            break;

            case MPU6050_BAND_5_HZ:
                Serial.println("5 Hz");
                _gyro_bandwidth = "5";
            break;
        }
    }

    if(_bmp_started && _gyro_started){

        xTaskCreate( vSensorsTaskCb, "SENSORS_READ_TASK", 3072, NULL, tskIDLE_PRIORITY + 2, &app.SENSORS_READ_TASK);
        
        delay(100);

        adc1_config_width(ADC_WIDTH_12Bit);
        adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);// using GPIO 35

        //xTaskCreate( fReadBattery, "BAT_CHARGE_TASK", 1024, NULL, tskIDLE_PRIORITY + 2, &app.BAT_CHARGE_TASK);
        return SENSORS_STARTED_OK;
    }
    else
    {
        if(!_bmp_started){
            return SENSOR_BMP085_INIT_KO;
        }

        if(!_gyro_started){
            return SENSOR_MPU6050_INIT_KO;
        }
    }

    return SENSOR_UNKNOWN_ERROR;
}

