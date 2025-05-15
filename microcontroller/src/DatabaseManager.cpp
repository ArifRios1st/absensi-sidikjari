#include "DatabaseManager.hpp"
#include "Utils/Log.hpp"
#include "configs.hpp"

#include <sstream>

constexpr char CHECK_TABLE_SQL[] PROGMEM = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?";
constexpr char CREATE_SETTINGS_TABLE_SQL[] PROGMEM = R"(
    CREATE TABLE IF NOT EXISTS settings (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        token TEXT DEFAULT '1234567890',
        ssid TEXT DEFAULT 'AKB',
        password TEXT DEFAULT 'Sdaisiyah1@22',
        ap_ssid TEXT DEFAULT 'Absensi Biometrik',
        ap_password TEXT DEFAULT '1234567890',
        ap_hidden INTEGER DEFAULT 1,
        admin_pin TEXT DEFAULT '123456',
        url TEXT DEFAULT 'http://192.168.1.10:8000',
    );
)";
constexpr char CREATE_FINGERPRINTS_TABLE_SQL[] PROGMEM = R"(
    CREATE TABLE IF NOT EXISTS fingerprints (
        id INTEGER PRIMARY KEY,
        user_id INTEGER NOT NULL,
    );
)";
constexpr char CREATE_ABSENCES_TABLE_SQL[] PROGMEM = R"(
    CREATE TABLE IF NOT EXISTS absences (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        fingerprint_id	INTEGER,
        timestamp	TEXT DEFAULT (datetime('now')),
        sync_at	TEXT DEFAULT (NULL),
        FOREIGN KEY(fingerprint_id) REFERENCES fingerprints(id) ON DELETE CASCADE
    );
)";

constexpr char CHECK_SETTINGS_DATA_EXIST_SQL[] PROGMEM = "SELECT COUNT(*) FROM settings;";
constexpr char INSERT_DEFAULT_SETTINGS_DATA_SQL[] PROGMEM = "INSERT OR REPLACE INTO settings (id) VALUES (1);";

constexpr char DELETE_TABLE_FINGERPRINTS_SQL[] PROGMEM = "DROP TABLE IF EXISTS fingerprints;";
constexpr char DELETE_TABLE_ABSENCES_SQL[] PROGMEM = "DROP TABLE IF EXISTS absences;";
constexpr char DELETE_TABLE_SETTINGS_SQL[] PROGMEM = "DROP TABLE IF EXISTS settings;";

constexpr char BEGIN_TRANSACTION_SQL[] PROGMEM = "BEGIN TRANSACTION;";
constexpr char COMMIT_TRANSACTION_SQL[] PROGMEM = "COMMIT;";
constexpr char ROLLBACK_TRANSACTION_SQL[] PROGMEM = "ROLLBACK;";

constexpr char GET_USER_ID_FROM_FINGERPRINT_ID_SQL[] PROGMEM = "SELECT user_id FROM fingerprints WHERE id = ?;";
constexpr char INSERT_FINGERPRINT_ID_SQL[] PROGMEM = "INSERT OR REPLACE INTO fingerprints (id, user_id) VALUES (?, ?);";
constexpr char DELETE_FINGERPRINT_ID_SQL[] PROGMEM = "DELETE FROM fingerprints WHERE id = ?;";

constexpr char GET_ALL_SETTINGS_DATA_SQL[] PROGMEM = "SELECT ssid, password, ap_ssid, ap_password, ap_hidden, token, url, admin_pin FROM settings WHERE id = 1;";
constexpr char UPDATE_ALL_SETTINGS_DATA_SQL[] PROGMEM = "UPDATE settings SET ssid = ?, password = ?, ap_ssid = ?, ap_password = ?, ap_hidden = ?,token = ?, admin_pin = ?, url = ? WHERE id = 1;";

constexpr char GET_ALL_FINGERPRINT_ID_BY_USER_ID_SQL[] PROGMEM = "SELECT id FROM fingerprints WHERE user_id = ?;";



DatabaseManager::DatabaseManager() : db(nullptr),SDCARD_SPI(HSPI) {}

DatabaseManager::~DatabaseManager()
{
    closeDb();
    sqlite3_shutdown();
    SDCARD_SPI.end();
}

bool DatabaseManager::Initialize()
{
    SDCARD_SPI.begin(SDCARD_SCK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);
    if (!SD.begin(SDCARD_CS, SDCARD_SPI)) {
        LOGE("Failed to mount SD card");
        return false;
    }

    LOGD("SD Card Size: %lu MB", SD.cardSize() / (1024 * 1024));

    sqlite3_config(SQLITE_CONFIG_SERIALIZED);
    sqlite3_initialize();

    db_exec("PRAGMA synchronous = NORMAL;", nullptr);  // Optimize writes
    db_exec("PRAGMA cache_size = 32;", nullptr);  // Optimize writes
    db_exec("PRAGMA foreign_keys = ON;", nullptr);  // Enable Foreign Key
    createTables();
    closeDb();

    LOGD("Database initialized");
    return true;
}

int DatabaseManager::openDb()
{
    if (db) {
        return SQLITE_OK;
    }

    int rc = sqlite3_open(SDCARD_BASEPATH DATABASE_FILE, &db);
    if (rc != SQLITE_OK) {
        LOGE("Can't open database: %s", sqlite3_errmsg(db));
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
    } else {
        // LOGD("Opened database successfully");
    }
    return rc;
}

void DatabaseManager::closeDb()
{
    if (db) {
        sqlite3_close(db);
        db = nullptr;
        // LOGD("Database closed successfully");
    }
}

int DatabaseManager::db_exec(const char *sql, cJSON *jsonArray)
{
    if (openDb() != SQLITE_OK) return SQLITE_ERROR;

    char *zErrMsg = nullptr;
    int rc = sqlite3_exec(db, sql, 
        (jsonArray ? callback : nullptr),  // Use callback only if jsonArray is valid
        jsonArray, 
        &zErrMsg
    );

    if (rc != SQLITE_OK) {
        LOGE("SQL error: %s", zErrMsg);
        LOGE("Query: %s", sql);
        sqlite3_free(zErrMsg);
    } else {
        LOGD("Operation completed successfully");
    }

    return rc;
}

int DatabaseManager::db_exec(const char *sql)
{
    if (openDb() != SQLITE_OK) return SQLITE_ERROR;

    sqlite3_stmt *stmt;
    LOGD("Query: %s",sql);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOGE("Failed to prepare statement");
        return SQLITE_ERROR;
    }

    int result = (sqlite3_step(stmt) == SQLITE_DONE) ? SQLITE_OK : SQLITE_ERROR;
    sqlite3_finalize(stmt);

    return result;
}

int DatabaseManager::beginTransaction()
{
    return db_exec(BEGIN_TRANSACTION_SQL, nullptr);
}

int DatabaseManager::commitTransaction()
{
    return db_exec(COMMIT_TRANSACTION_SQL, nullptr);
}

int DatabaseManager::rollbackTransaction()
{
    return db_exec(ROLLBACK_TRANSACTION_SQL, nullptr);
}

int DatabaseManager::executeTransaction(const char *query, std::function<void(sqlite3_stmt *)> bindFunc)
{
    if (openDb() != SQLITE_OK) return SQLITE_ERROR;

    if (beginTransaction() != SQLITE_OK) {
        LOGE("Failed to start transaction");
        closeDb();
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt;
    LOGD("Query: %s",query);
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        LOGE("Failed to prepare statement");
        rollbackTransaction();
        closeDb();
        return SQLITE_ERROR;
    }

    bindFunc(stmt);
    int result = (sqlite3_step(stmt) == SQLITE_DONE) ? SQLITE_OK : SQLITE_ERROR;
    sqlite3_finalize(stmt);

    if(result == SQLITE_OK)
        commitTransaction();
    else
        rollbackTransaction();

    closeDb();
    return result;
}

int DatabaseManager::executeQuery(const char *query, std::function<void(sqlite3_stmt *)> bindFunc, std::function<void(sqlite3_stmt *, int *)> processFunc)
{
    if (openDb() != SQLITE_OK) return SQLITE_ERROR;

    sqlite3_stmt *stmt;
    LOGD("Query: %s",query);
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        LOGE("Failed to prepare query");
        closeDb();
        return SQLITE_ERROR;
    }

    bindFunc(stmt);  // Bind parameters (if any)

    int result = SQLITE_OK;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        processFunc(stmt, &result);  // Process each row
    }

    sqlite3_finalize(stmt);
    closeDb();
    return result;
}

bool DatabaseManager::tableExist(const char *tableName)
{
    bool exist = false;
    int rc = executeQuery(CHECK_TABLE_SQL,
        [&](sqlite3_stmt *stmt) {
            sqlite3_bind_text(stmt, 1, tableName, -1, SQLITE_STATIC);
        },
        [&](sqlite3_stmt *stmt,int *result) {
            exist = (sqlite3_column_int(stmt, 0) > 0);
        });

    return (rc == SQLITE_OK) && exist;
}

int DatabaseManager::createTables()
{
    int rc = SQLITE_OK;

    rc = db_exec(CREATE_FINGERPRINTS_TABLE_SQL);
    if (rc != SQLITE_OK) return rc;

    rc = db_exec(CREATE_ABSENCES_TABLE_SQL);
    if (rc != SQLITE_OK) return rc;

    rc = db_exec(CREATE_SETTINGS_TABLE_SQL);
    if (rc != SQLITE_OK) return rc;

    bool hasSettings = false;
    rc = executeQuery(CHECK_SETTINGS_DATA_EXIST_SQL,
        [&](sqlite3_stmt *stmt) { /* no binding */ },
        [&](sqlite3_stmt *stmt, int *result) {
            hasSettings = (sqlite3_column_int(stmt, 0) > 0);
        }
    );
    if (rc != SQLITE_OK) return rc;
    if (!hasSettings) {
        rc = db_exec(INSERT_DEFAULT_SETTINGS_DATA_SQL);
    }
    return rc;
}

int DatabaseManager::deleteTables()
{
    return (db_exec(DELETE_TABLE_ABSENCES_SQL) == SQLITE_OK && db_exec(DELETE_TABLE_FINGERPRINTS_SQL) == SQLITE_OK && db_exec(DELETE_TABLE_SETTINGS_SQL) == SQLITE_OK) ? SQLITE_OK : SQLITE_ERROR;
}

int DatabaseManager::getUserIdByFingerprint(int fingerprintId, int *user_id)
{
    return executeQuery(GET_USER_ID_FROM_FINGERPRINT_ID_SQL, 
        [&](sqlite3_stmt *stmt) {
            sqlite3_bind_int(stmt, 1, fingerprintId);
        },
        [&](sqlite3_stmt *stmt,int *result) {
            if (user_id) {
                *user_id = sqlite3_column_int(stmt, 0);
            }
            *result = SQLITE_OK;
        }
    );
}

int DatabaseManager::insertFingerprint(int userId, int fingerprintId)
{
    return executeTransaction(INSERT_FINGERPRINT_ID_SQL, [&](sqlite3_stmt *stmt) {
        sqlite3_bind_int(stmt, 1, fingerprintId);
        sqlite3_bind_int(stmt, 2, userId);
    });
}

int DatabaseManager::deleteFingerprintById(int fingerprintId)
{
    return executeTransaction(DELETE_FINGERPRINT_ID_SQL, [&](sqlite3_stmt *stmt) {
        sqlite3_bind_int(stmt, 1, fingerprintId);
    });
}

int DatabaseManager::getUnsyncedAbsences(cJSON **result)
{
    if (!result) {
        return SQLITE_ERROR; // Null pointer passed
    }

    *result = cJSON_CreateObject();
    if (!*result) {
        return SQLITE_ERROR; // Memory allocation failed
    }
    cJSON *attendancesArray = cJSON_CreateArray();

    const char *query = "SELECT id, fingerprint_id, timestamp FROM absences WHERE sync_at IS NULL";

    int rc = executeQuery(query,
        [](sqlite3_stmt *stmt) {
            // No binding needed
        },
        [&](sqlite3_stmt *stmt, int *result) {
            int id = sqlite3_column_int(stmt, 0);
            int fingerprintId = sqlite3_column_int(stmt, 1);
            const char *timestamp = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

            cJSON *jsonObj = cJSON_CreateObject();
            cJSON_AddNumberToObject(jsonObj, "id", id);
            cJSON_AddNumberToObject(jsonObj, "fingerprint_id", fingerprintId);
            cJSON_AddStringToObject(jsonObj, "timestamp", timestamp);

            cJSON_AddItemToArray(attendancesArray, jsonObj);
        });

    cJSON_AddItemToObject(*result, "attendances", attendancesArray);

    if (rc != SQLITE_OK) {
        cJSON_Delete(*result);
        *result = nullptr;
    }

    return rc;
}

int DatabaseManager::markAbsenceAsSyncedById(int id)
{
    const char *query = "UPDATE absences SET sync_at = datetime('now') WHERE id = ?";
    return executeQuery(query,
        [id](sqlite3_stmt *stmt) {
            sqlite3_bind_int(stmt, 1, id);
        },
        [](sqlite3_stmt *, int *) {});
}

int DatabaseManager::deleteAbsenceById(int id)
{
    const char *query = "DELETE FROM absences WHERE id = ?";
    return executeQuery(query,
        [id](sqlite3_stmt *stmt) {
            sqlite3_bind_int(stmt, 1, id);
        },
        [](sqlite3_stmt *, int *) {});
}

int DatabaseManager::getAllSettings(cJSON **jsonSettings)
{
    if (!jsonSettings) {
        return SQLITE_ERROR; // Null pointer passed
    }

    *jsonSettings = cJSON_CreateObject();
    if (!*jsonSettings) {
        return SQLITE_ERROR; // Memory allocation failed
    }

    return executeQuery(GET_ALL_SETTINGS_DATA_SQL, 
        [](sqlite3_stmt *stmt) {
            // No parameters to bind
        },
        [&](sqlite3_stmt *stmt, int *result) {
            cJSON_AddStringToObject(*jsonSettings, "ssid", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            cJSON_AddStringToObject(*jsonSettings, "password", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            cJSON_AddStringToObject(*jsonSettings, "ap_ssid", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            cJSON_AddStringToObject(*jsonSettings, "ap_password", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            cJSON_AddBoolToObject(*jsonSettings, "ap_hidden", sqlite3_column_int(stmt, 4));
            cJSON_AddStringToObject(*jsonSettings, "token", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
            cJSON_AddStringToObject(*jsonSettings, "url", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
            cJSON_AddStringToObject(*jsonSettings, "admin_pin", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
        }
    );
}

int DatabaseManager::updateAllSettings(cJSON *jsonSettings)
{
    // Parse JSON string using cJSON
    if (!jsonSettings) {
        return SQLITE_ERROR; // Null check to prevent crashes
    }

    int result = executeTransaction(UPDATE_ALL_SETTINGS_DATA_SQL, [&](sqlite3_stmt *stmt) {
        sqlite3_bind_text(stmt, 1, cJSON_GetObjectItemCaseSensitive(jsonSettings, "ssid")->valuestring, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, cJSON_GetObjectItemCaseSensitive(jsonSettings, "password")->valuestring, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, cJSON_GetObjectItemCaseSensitive(jsonSettings, "ap_ssid")->valuestring, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, cJSON_GetObjectItemCaseSensitive(jsonSettings, "ap_password")->valuestring, -1, SQLITE_STATIC);
        sqlite3_bind_int( stmt, 5, cJSON_GetObjectItemCaseSensitive(jsonSettings, "ap_hidden")->valueint); // Convert to boolean (1 or 0)
        sqlite3_bind_text(stmt, 6, cJSON_GetObjectItemCaseSensitive(jsonSettings, "token")->valuestring, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, cJSON_GetObjectItemCaseSensitive(jsonSettings, "admin_pin")->valuestring, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 8, cJSON_GetObjectItemCaseSensitive(jsonSettings, "url")->valuestring, -1, SQLITE_STATIC);
    });

    return result;
}

int DatabaseManager::getAllFingerprintIdsByUserId(int userId, cJSON *fingerprint_ids)
{
    return executeQuery(GET_ALL_FINGERPRINT_ID_BY_USER_ID_SQL, 
        [&](sqlite3_stmt *stmt) {
            sqlite3_bind_int(stmt, 1, userId);
        },
        [&](sqlite3_stmt *stmt, int *result) {
            if(fingerprint_ids){
                cJSON_AddItemToArray(fingerprint_ids, cJSON_CreateNumber(sqlite3_column_int(stmt, 0)));
            }
        }
    );
}

int DatabaseManager::ResetData()
{
    if (beginTransaction() != SQLITE_OK) {
        LOGE("Failed to start transaction");
        closeDb();
        return SQLITE_ERROR;
    }

    int result1 = deleteTables();
    int result2 = createTables();

    bool result = result1 == SQLITE_OK && result2 == SQLITE_OK;

    if(result){
        commitTransaction();
    }else{
        rollbackTransaction();
    }
    closeDb();
    return (result) ? SQLITE_OK : SQLITE_ERROR;
}

int DatabaseManager::callback(void *data, int argc, char **argv, char **azColName)
{
    if (data) {
        cJSON *jsonArray = static_cast<cJSON*>(data);
        cJSON *jsonObject = cJSON_CreateObject();
        
        if (!jsonObject) {
            return SQLITE_NOMEM; // Memory allocation failure
        }

        for (int i = 0; i < argc; i++) {
            if (argv[i]) {
                cJSON_AddStringToObject(jsonObject, azColName[i], argv[i]);
            } else {
                cJSON_AddStringToObject(jsonObject, azColName[i], "NULL");
            }
        }

        cJSON_AddItemToArray(jsonArray, jsonObject);
    }

    return SQLITE_OK;
}
