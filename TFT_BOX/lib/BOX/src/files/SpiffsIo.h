#ifndef SPIFFSIO_H
#define SPIFFSIO_H

#include <Arduino.h>
#include "files/spiffsio_helper.h"

#define CONFIG_FILE_SIZE 2000
#define CONFIG_FILE_PATH "/config.json"
#define TIMEZONES_FILE_SIZE 60000
#define TIMEZONES_FILE_PATH "/zones.json"
#define CONFIG_BACKUP_FILE_PATH "/config_bk.json"

class SpiffsIo {

    private:
    String LAST_BACKUP_TIMESTAMP;
    bool backup_now;
    int backup_delay;

    public:
    SpiffsIo();
    ~SpiffsIo();

    void setBackupNow(bool now);
    bool getBackupNow();
    String getBackupTimestamp(){return LAST_BACKUP_TIMESTAMP;};
    backup_control_t control_backup(backup_control_t *control);
    bool backupNow();

    filesio_info_t init_spiffs();

    void init_config();
    bool fileExiste(const String& path);
    filesio_operations_t createFile(const String& path);
    bool deleteFile(const String& path);
    bool renameFile(const String& pathFrom, const String& pathTo);
    filesio_operations_t copyFile(const String& copyFrom, const String& copyTo);
    filesio_operations_t make_config_backup();
    void printFile(const String& path);

    bool setString(const char *key, const char *value);
    String getString(const char *key);

    bool setLong(const char *key, long value);
    long getLong(const char *key);

    bool setBool(const char *key, bool value);
    long getBool(const char *key);


};

#endif /* SPIFFSIO_H */