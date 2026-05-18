#include "SpiffsIo.h"
#include <SPIFFS.h>
#include "Application.h"

top_bar_config_t spiffs_set_top_bar_control;



SpiffsIo::SpiffsIo(){
    LAST_BACKUP_TIMESTAMP = "";
    backup_now            = false;
    backup_delay          = 60000;
}
SpiffsIo::~SpiffsIo(){}

void SpiffsIo::setBackupNow(bool now){
    backup_now = now;
}

bool SpiffsIo::getBackupNow(){
    return backup_now;
}

backup_control_t SpiffsIo::control_backup(backup_control_t *control){
    return app.bk_control = {control->backup_enabled, control->init_backup, control->backup_now, control->delay};
}

bool SpiffsIo::backupNow(){

    if(make_config_backup() == FIO_BACKUP_FILE_OK){
        return true;
    }

    return false;

}

void vSpiffsBackupTask_cb(void *arg){

    Serial.println("Backup Spiffs task started");

    backup_control_t * data = (backup_control_t *) arg;

    unsigned long timeout = millis();

    for(;;){

        if(data->backup_enabled){

            if(data->backup_now){


                if(app.backupNow()){
                    Serial.println("Config backup passed");
                }

            }

            if(data->init_backup && !data->backup_now){
                
                if(millis() - timeout > data->delay){

                    timeout = 0;
            
                    app.make_config_backup();

                    timeout = millis();
                }
            }
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

filesio_info_t SpiffsIo::init_spiffs(){

    if(!SPIFFS.begin()){

        Serial.println("SPIFFS");

        return FIO_MOUNT_FAIL;
    }

    if(!SPIFFS.exists(CONFIG_FILE_PATH)){

        Serial.println("SPIFFS");

        return FIO_CONFIG_FILE_NOT_EXIST;
    }

    app.init_config();

    if(app.SETUP_OK){

        app.bk_control = {true, false, false, 60000};

        xTaskCreate( vSpiffsBackupTask_cb, "BK_SPIFFS_TASK", 10000, (void *)&app.bk_control, tskIDLE_PRIORITY + 10, &app.BACKUP_SPIFFS_TASK);
    }
    
    return FIO_OK;

}

void SpiffsIo::init_config(){

    Serial.println("\nInit config file\n");

    app.SETUP_OK     = app.getBool("SETUP_OK");
    app.COUNTRY      = app.getString("COUNTRY");
    app.CITY         = app.getString("CITY");
    app.TIMEZONE     = app.getString("TIMEZONE");
    app.TIMEZONE_STR = app.getString("TIMEZONE_STR");
    app.TIMEZONE_OS  = app.getLong("TIMEZONE_OS");
    app.TIMEZONE_DL  = app.getLong("TIMEZONE_DL");
    app.ROUTER_SSID  = app.getString("ROUTER_SSID");
    app.ROUTER_PASS  = app.getString("ROUTER_PASS");

}

bool SpiffsIo::fileExiste(const String& path){
    return SPIFFS.exists(path);
}

filesio_operations_t SpiffsIo::createFile(const String& path){

    if(!fileExiste(path)){

        File file = SPIFFS.open(path, FILE_READ);

        if(!file){
            file.flush();
            file.close();
            
            return FIO_FILE_CREATE_KO;
        }

        file.flush();
        file.close();

        return FIO_OPERATION_OK;
    }
    else
    {
        return FIO_FILE_CREATE_FILE_EXIST;
    }

    return FIO_OPERATION_FAIL;

}

bool SpiffsIo::deleteFile(const String& path){
    return SPIFFS.remove(path);
}

bool SpiffsIo::renameFile(const String& pathFrom, const String& pathTo){
    return SPIFFS.rename(pathFrom, pathTo);
}

filesio_operations_t SpiffsIo::copyFile(const String& copyFrom, const String& copyTo){

    if(!fileExiste(copyFrom)){
        return FIO_SOURCE_FILE_NOT_EXIST;
    }

    File f_from = SPIFFS.open(copyFrom , FILE_READ);

    if(!f_from){
        return FIO_FAILD_OPEN_FILE;
    }

    if(fileExiste(copyTo)){
        deleteFile(copyTo);
    }

    File f_to   = SPIFFS.open(copyTo, FILE_WRITE);

    if(!f_to){
        return FIO_FAILD_CREATE_FILE;
    }

    char buffer[4096];

    while( f_from.available() ) {
        size_t read_bytes = f_from.readBytes( buffer, 4096 );
        f_to.write( (const uint8_t *)buffer, read_bytes ); 
    }

    return FIO_OPERATION_OK;

}

filesio_operations_t SpiffsIo::make_config_backup(){

    filesio_operations_t operation_backup = copyFile(CONFIG_FILE_PATH, CONFIG_BACKUP_FILE_PATH);

    switch (operation_backup) {
        
        case FIO_SOURCE_FILE_NOT_EXIST:
            Serial.println("\nFile [config.json] not existe");
            if(!backup_now){
                LAST_BACKUP_TIMESTAMP = "";
                LAST_BACKUP_TIMESTAMP = "File [config.json] not existe",
                delay(1000);
            }
            return FIO_FAILD_BACKUP_FILE;
        break;

        case FIO_FAILD_OPEN_FILE:
            Serial.println("\nFailed open [config.json] to copy");
            if(!backup_now){
                LAST_BACKUP_TIMESTAMP = "";
                LAST_BACKUP_TIMESTAMP = "Failed open [config.json] to copy",
                delay(1000);
            }
            return FIO_FAILD_BACKUP_FILE;
        break;

        case FIO_FAILD_CREATE_FILE:
            Serial.println("\nFailed create [config_bk.json] to copy");
            if(!backup_now){
                LAST_BACKUP_TIMESTAMP = "";    
                LAST_BACKUP_TIMESTAMP = "Failed create [config_bk.json] to copy";
                delay(1000);
            }
            return FIO_FAILD_BACKUP_FILE;
        break;

        case FIO_OPERATION_OK:
            LAST_BACKUP_TIMESTAMP = "";
            LAST_BACKUP_TIMESTAMP +=  "Last backup ";
            LAST_BACKUP_TIMESTAMP += app.set_top_bar_control.day;
            LAST_BACKUP_TIMESTAMP +=  " - ";
            LAST_BACKUP_TIMESTAMP +=  app.server_time_info.fullDateNumbers;
            LAST_BACKUP_TIMESTAMP +=  " - ";
            LAST_BACKUP_TIMESTAMP += app.set_top_bar_control.time;
            

            Serial.println("\nBackup to [config_bk.json] success");
           
            if(!backup_now){
                delay(1000);
                printFile(CONFIG_FILE_PATH);
                delay(1000);
                printFile(CONFIG_BACKUP_FILE_PATH);
            }
            Serial.print("Last backup at: ");Serial.println(getBackupTimestamp());

            return FIO_BACKUP_FILE_OK;
        break;
        
        default:
        break;
    }

    return FIO_OPERATION_FAIL;

}

void SpiffsIo::printFile(const String& path){

    if(path == CONFIG_FILE_PATH){

        File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");
        DynamicJsonDocument document(CONFIG_FILE_SIZE);
        JsonObject object = document.to<JsonObject>();

        Serial.println("Config File Content:");
            
        deserializeJson(document, configFile);
        configFile.close();

        String table;
        serializeJson(object, table);
            
        Serial.println();    
        Serial.println(table);
        Serial.println();

    }

    if(path == CONFIG_BACKUP_FILE_PATH){

        File configFile = SPIFFS.open(CONFIG_BACKUP_FILE_PATH, "r");
        DynamicJsonDocument document(CONFIG_FILE_SIZE);
        JsonObject object = document.to<JsonObject>();

        Serial.println("Backup config File Content:");
            
        deserializeJson(document, configFile);
        configFile.close();

        String table;
        serializeJson(object, table);

        Serial.println();    
        Serial.println(table);
        Serial.println();

    }


    

}

bool SpiffsIo::setString(const char *key, const char *value){

    app.bk_control.backup_now = true;

    File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");
    DynamicJsonDocument doc(CONFIG_FILE_SIZE);
    deserializeJson(doc, configFile);
    configFile.close();

    doc[key] = value;

    configFile = SPIFFS.open(CONFIG_FILE_PATH, "w");

    if (!configFile) {
        Serial.println("Failed to open config file for writing");
        configFile.close();
        return false;
    }

    if (serializeJson(doc, configFile) == 0) {
        Serial.println(F("Failed to write to file"));
        configFile.close();
        return false;
    }
    
    configFile.close();

    app.bk_control.backup_now  = false;
    app.bk_control.init_backup = true;


    return true;
}

String SpiffsIo::getString(const char *key){

    if(!SPIFFS.begin()){
        return "Error";
    }

    File configFile   = SPIFFS.open(CONFIG_FILE_PATH, "r");
    SpiRamJsonDocument doc(CONFIG_FILE_SIZE);
    JsonObject object = doc.to<JsonObject>();
    deserializeJson(doc, configFile);
    configFile.flush();
    configFile.close();
    
    return object[key].as<String>();
}

bool SpiffsIo::setLong(const char *key, long value){

    app.bk_control.backup_now = true;

    File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");
    DynamicJsonDocument doc(CONFIG_FILE_SIZE);
    deserializeJson(doc, configFile);
    configFile.close();

    doc[key] = value;

    configFile = SPIFFS.open(CONFIG_FILE_PATH, "w");

    if (!configFile) {
        Serial.println("Failed to open config file for writing");
        configFile.close();
        return false;
    }

    if (serializeJson(doc, configFile) == 0) {
        Serial.println(F("Failed to write to file"));
        configFile.close();
        return false;
    }
    
    configFile.close();

    app.bk_control.backup_now  = false;
    app.bk_control.init_backup = true;


    return true;

}

long SpiffsIo::getLong(const char *key){
    
    if(!SPIFFS.begin()){
        return -1;
    }

    File configFile   = SPIFFS.open(CONFIG_FILE_PATH, "r");
    SpiRamJsonDocument doc(CONFIG_FILE_SIZE);
    JsonObject object = doc.to<JsonObject>();
    deserializeJson(doc, configFile);
    configFile.flush();
    configFile.close();
    
    return object[key].as<long>();
}

bool SpiffsIo::setBool(const char *key, bool value){
    
    app.bk_control.backup_now = true;

    File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");
    DynamicJsonDocument doc(CONFIG_FILE_SIZE);
    deserializeJson(doc, configFile);
    configFile.close();

    doc[key] = value;

    configFile = SPIFFS.open(CONFIG_FILE_PATH, "w");

    if (!configFile) {
        Serial.println("Failed to open config file for writing");
        configFile.close();
        return false;
    }

    if (serializeJson(doc, configFile) == 0) {
        Serial.println(F("Failed to write to file"));
        configFile.close();
        return false;
    }
    
    configFile.close();

    app.bk_control.backup_now  = false;
    app.bk_control.init_backup = true;


    return true;
}

long SpiffsIo::getBool(const char *key){

    if(!SPIFFS.begin()){
        return false;
    }

    File configFile   = SPIFFS.open(CONFIG_FILE_PATH, "r");
    SpiRamJsonDocument doc(CONFIG_FILE_SIZE);
    JsonObject object = doc.to<JsonObject>();
    deserializeJson(doc, configFile);
    configFile.flush();
    configFile.close();
    
    return object[key].as<bool>();

}