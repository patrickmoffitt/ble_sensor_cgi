How to Build a CGI That a Queries SQLite Database
-------------------------------------------------
This project shows you how to query data from my 
[Bluetooth Low Energy Environmental Sensor](https://github.com/patrickmoffitt/zephyr_ble_sensor) that was received and 
logged into a SQLite database by my 
[Bluetooth Low Energy Sensor Service](https://github.com/patrickmoffitt/ble_sensor_service) project so you can RESTfully
query the data in JSON (with this project) and inspect it
with [interactive data graphics](https://github.com/patrickmoffitt/ble_sensor_charts) powered by
[Plotly](https://plotly.com/javascript/).

Building
--------
This is a CMake project that requires cURL and libmagic.


Installing the CGI
------------------
As root, do the following;
1. Copy the CGI binary you built (ble_sensor_cgi) with cmake into `/usr/lib/cgi-bin/` If you renamed the project in
   CMakeLists.txt the binary will have a new name as well. You should reflect this URL change in the JavaScript of the
   [data charting project]((https://github.com/patrickmoffitt/ble_sensor_charts))
2. Enable the CGI module.
```bash
sudo a2enmod cgi
```
3. Restart Apache
```bash
sudo systemctl restart apache2
```

Do I Have to Use Apache?
------------------------
Certainly not. The CGI interface standard is supported by many different HTTP servers. You should have little difficulty
adapting this project to work with your chosen web server.

Resources
---------
There are also tools you may find useful to you in other projects. For example;
- Http_parser::parse_query_string stores the HTML query string in a
  `std::map<std::string, std::vector<std::string>>` From this you can create a project that handles HTML GET requests.
- Http_parser::parse_multipart_form stores multipart/form-data encoded HTTP message bodies in two
  `std::map<std::string, std::vector<std::string>>` - one for variables and another for files. From this you can
  create a project where forms are used to control a robot/embedded system or your Raspberry Pi. To help you get started
  there is an [everything-but-the-kitchen-sink HTML form](https://github.com/patrickmoffitt/jpeg_catcher/blob/main/test/multipart_form.html). Edit the `<form>` tag at the top
  and change the `action` attribute to reflect your server name.

Credits
-------
- Thank you to [hjiang](https://github.com/hjiang) for the [jsonxx](https://github.com/hjiang/jsonxx) project. I tested
  several C++ JSON libraries including [jsoncpp](https://github.com/open-source-parsers/jsoncpp) before deciding to use
  jsonxx for its simplicity and speed.
- Thank you to [Marty Stepp](https://www.martystepp.com/) for his base 64 encoding and decoding library.
- Thank you to [Ian Darwin and Christos Zoulas](https://www.darwinsys.com/file/) the inventor and maintainer
  (respectively) of all things magic number. I've been using the file(1) command all my life and was delighted to find
  the development library for it so easy to use.

Bugs, Issues, and Pull Requests
------------------------------
If you find a bug please create an issue. If you'd like to contribute please send a pull request.

References
----------
The following were helpful references in the development of this project.

- [Apache Tutorial: Dynamic Content with CGI](http://httpd.apache.org/docs/current/howto/cgi.html)
- [Common Gateway Interface](https://en.wikipedia.org/wiki/Common_Gateway_Interface)
