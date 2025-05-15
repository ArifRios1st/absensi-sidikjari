#pragma once
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
// #include <ArduinoJson.h>
#include <cJSON.h>

class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();

    bool Initialize();

    int getUserIdByFingerprint(int fingerprintId, int *user_id);
    int insertFingerprint(int userId, int fingerprintId);
    int getAllFingerprintIdsByUserId(int userId, cJSON *fingerprint_ids);
    int deleteFingerprintById(int fingerprintId);
    int getUnsyncedAbsences(cJSON **attendancesArray);
    int markAbsenceAsSyncedById(int id);
    int deleteAbsenceById(int id);

    // int getAllSettings(JsonDocument *settings);
    // int updateAllSettings(const JsonDocument &settings);

    int getAllSettings(cJSON **jsonSettings);
    int updateAllSettings(cJSON *jsonSettings);
    
    int ResetData();

protected:
    static int callback(void *data, int argc, char **argv, char **azColName);
    int openDb();
    void closeDb();
    int db_exec(const char *sql, cJSON *jsonArray);
    int db_exec(const char *sql);
    int executeTransaction(const char *query, std::function<void(sqlite3_stmt *)> bindFunc);
    int executeQuery(const char *query, std::function<void(sqlite3_stmt *)> bindFunc, std::function<void(sqlite3_stmt *, int *)> processFunc);

    bool tableExist(const char *tableName);
    int createTables();
    int deleteTables();

    int beginTransaction();
    int commitTransaction();
    int rollbackTransaction();

protected:
    sqlite3 *db;
    SPIClass SDCARD_SPI;
};