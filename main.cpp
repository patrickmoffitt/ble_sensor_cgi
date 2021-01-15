
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

#include <iostream>
#include "http_io.hpp"
#include "http_parser.hpp"
#include "ble_bme_680_sqlite3.hpp"

void fetch_json_data(int32_t epoch1, int32_t epoch2, ostringstream &content) {
    Ble_bme_680_db db("/home/patrick/SQLite3",
                      "ble_bme680.db");
    content << db.get_sensor_data_range(epoch1, epoch2);
};

/**
 * A program implementing the CGI for the purpose of reading a SQLite3 database.
 *
 * @param argc The count of command line arguments passed by the web server to
 * the CGI (us).
 * @param argv The command line argument strings.
 * @param envp The system environment variables as key=value pairs.
 * @return Exit status.
 *
 * @see https://en.wikipedia.org/wiki/Common_Gateway_Interface
 */
int main(int argc, char *argv[], char *envp[]) {
    http_io io;
    io.get_env(envp);

    Http_parser http(io.env);

    ostringstream content;
    content << noskipws;

    //
    if (http.post.find("epoch1") != http.post.end()
        and http.post.find("epoch2") != http.post.end()) {
        auto epoch1 = atoi(http.post.find("epoch1")->second.data()->c_str());
        auto epoch2 = atoi(http.post.find("epoch2")->second.data()->c_str());
        cerr << "epoch1: " << epoch1 << " epoch2: " << epoch2 << endl;
        fetch_json_data(epoch1, epoch2, content);
    }
    if (http.get.find("epoch1") != http.get.end()
        and http.get.find("epoch2") != http.get.end()) {
        auto epoch1 = atoi(http.get.find("epoch1")->second.data()->c_str());
        auto epoch2 = atoi(http.get.find("epoch2")->second.data()->c_str());
        fetch_json_data(epoch1, epoch2, content);
    }
    io.header["Content-Type"] = "application/json";
    io.respond(content);

    return 0;
}
