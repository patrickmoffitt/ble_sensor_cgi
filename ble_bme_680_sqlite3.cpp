/*
    Copyright (c) 2020 Patrick Moffitt

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "ble_bme_680_sqlite3.hpp"

Ble_bme_680_db::Ble_bme_680_db(const char *sqlite3_db_dir,
                               const char *sqlite3_db_file) {
    fs::path sqlite3_db_filename{sqlite3_db_dir};
    if (!fs::is_directory(fs::status(sqlite3_db_dir))) {
        cerr << sqlite3_db_dir << " is not a directory." << endl;
        exit(EXIT_DB_DIRECTORY_ERROR);
    }

    // Connect and set the DB handle.
    sqlite3_db_filename.append(sqlite3_db_file);
    auto error = sqlite3_open(sqlite3_db_filename.c_str(), &db_h);
    on_error_exit(error, SQLITE_OK, EXIT_DB_HANDLE_ERROR);
}

Ble_bme_680_db::~Ble_bme_680_db() {
    sqlite3_close(db_h);
}

bool Ble_bme_680_db::sensor_table_exists() const {
    const char *stmt = sensor_table_exists_stmt;
    sqlite3_stmt *p_stmt;
    auto error = sqlite3_prepare_v2(db_h, stmt, strlen(stmt),
                                    &p_stmt, nullptr);
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_SENSOR_DATA_TBL_COUNT);

    error = sqlite3_step(p_stmt);
    on_error_exit(error, SQLITE_ROW,
                  EXIT_DB_EXEC_STMT_SENSOR_TABLE_EXISTS);

    auto count = sqlite3_column_int(p_stmt, 0);
    sqlite3_finalize(p_stmt);
    if (count == 1)
        return true;

    return false;
}

void Ble_bme_680_db::create_sensor_data_table() const {
    const char *stmt = create_sensor_data_table_stmt;
    sqlite3_stmt *p_stmt;
    auto error = sqlite3_prepare_v2(db_h, stmt, strlen(stmt),
                                    &p_stmt, nullptr);
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_CREATE_SENSOR_TABLE);

    error = sqlite3_step(p_stmt);
    sqlite3_finalize(p_stmt);
    on_error_exit(error, SQLITE_DONE,
                  EXIT_DB_EXEC_STMT_CREATE_SENSOR_TABLE);
}

void Ble_bme_680_db::create_sensor_data_table_idx() const {
    const char *stmt = create_sensor_data_table_idx_stmt;
    sqlite3_stmt *p_stmt;
    auto error = sqlite3_prepare_v2(db_h, stmt, strlen(stmt),
                                    &p_stmt, nullptr);
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_CREATE_SENSOR_TABLE_IDX);

    error = sqlite3_step(p_stmt);
    sqlite3_finalize(p_stmt);
    on_error_exit(error, SQLITE_DONE,
                  EXIT_DB_EXEC_STMT_CREATE_SENSOR_TABLE_IDX);
}

void Ble_bme_680_db::sensor_data_insert(int32_t t, int8_t t_exp,
                                        int32_t h, int8_t h_exp,
                                        int32_t p, int8_t p_exp,
                                        int32_t g, int8_t g_exp,
                                        int32_t b, int8_t b_exp) const {
    const char *stmt = sensor_data_insert_stmt;
    sqlite3_stmt *p_stmt;
    auto error = sqlite3_prepare_v2(db_h, stmt, strlen(stmt),
                                    &p_stmt, nullptr);
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_SENSOR_DATA_INSERT);

    // Bind Epoch
    auto epoch = system_clock::to_time_t(system_clock::now());
    error = sqlite3_bind_int(p_stmt, 1, epoch);
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_SENSOR_DATA_BIND_E);

    // Bind T
    error = sqlite3_bind_double(p_stmt, 2, t * pow(10, t_exp));
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_SENSOR_DATA_BIND_T);

    // Bind H
    error = sqlite3_bind_double(p_stmt, 3, h * pow(10, h_exp));
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_SENSOR_DATA_BIND_H);

    // Bind P
    error = sqlite3_bind_double(p_stmt, 4, p * pow(10, p_exp));
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_SENSOR_DATA_BIND_P);

    // Bind G
    error = sqlite3_bind_int(p_stmt, 5, g * pow(10, g_exp));
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_SENSOR_DATA_BIND_G);

    // Bind B
    error = sqlite3_bind_double(p_stmt, 6, b * pow(10, b_exp));
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_SENSOR_DATA_BIND_B);

    error = sqlite3_step(p_stmt);
    sqlite3_finalize(p_stmt);
    on_error_exit(error, SQLITE_DONE,
                  EXIT_DB_EXEC_STMT_SENSOR_DATA_INSERT);
}

/*
 * @brief Get an epoch-range of sensor data
 *
 * @param begin the start of the range in seconds.
 * @param end the end of the range in seconds.
 *
 * @return data in JSON format
 *
 * @note If end >= begin an empty object is returned.
 * Hint, end is further into the past.
 */
string Ble_bme_680_db::get_sensor_data_range(int32_t epoch1,
                                             int32_t epoch2) const {
    int32_t begin, end;
    if (epoch1 > epoch2) {
        begin = epoch1;
        end = epoch2;
    } else {
        begin = epoch2;
        end = epoch1;
    }
    string data = "{}";
    if (end >= begin) return data;

    const char *stmt = get_sensor_data_range_stmt;
    sqlite3_stmt *p_stmt;
    auto error = sqlite3_prepare_v2(db_h, stmt, strlen(stmt),
                                    &p_stmt, nullptr);
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_GET_SENSOR_DATA_RANGE);

    // Bind Begin
    error = sqlite3_bind_int(p_stmt, 1, begin);
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_GET_SENSOR_DATA_RANGE_BIND_B);

    // Bind End
    error = sqlite3_bind_int(p_stmt, 2, end);
    on_error_exit(error, SQLITE_OK,
                  EXIT_DB_PREP_STMT_GET_SENSOR_DATA_RANGE_BIND_E);

    Object object;
    object << "epoch1" << epoch1;
    object << "epoch2" << epoch2;
    Array row;
    row << "epoch";
    row << "temperature_c";
    row << "humidity_rh";
    row << "pressure_kpa";
    row << "gas_ohms";
    row << "battery_volts";
    object << "columns" << row;
    row.reset();
    Array rows;

    stringstream ss;
    while ((error = sqlite3_step(p_stmt)) == SQLITE_ROW) {
        ss.str("");
        ss << sqlite3_column_text(p_stmt, 0);
        row << ss.str();                             // epoch;
        row << sqlite3_column_double(p_stmt, 1); // temperature_c;
        row << sqlite3_column_double(p_stmt, 2); // humidity_rh;
        row << sqlite3_column_double(p_stmt, 3); // pressure_kpa;
        row << sqlite3_column_int(p_stmt, 4);    // gas_ohms;
        row << sqlite3_column_double(p_stmt, 5); // battery_volts;
        rows.append(row);
        row.reset();
    }
    object << "sensor_data" << rows;
    data = object.json();
    on_error_exit(error, SQLITE_DONE,
                  EXIT_DB_EXEC_STMT_GET_SENSOR_DATA_RANGE);

    sqlite3_finalize(p_stmt);

    return data;
}

void Ble_bme_680_db::on_error_exit(int error,
                                   int condition,
                                   int exit_code) const {
    if (error != condition) {
        sqlite3_close(db_h);
        cerr << sqlite3_errmsg(db_h) << endl;
        exit(exit_code);
    }
}





