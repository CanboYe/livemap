# Zone Cloudlet

### Installation and Running
To build:
```
cd build && cmake .. && make -j 9
```
The main server is a C++ application which communicates with the Vehicle Cloudlet via MQTT.  To run the main server:
```
cd build && ./cloudlet
```
The Node.js server gets notified by a PostgreSQL database add command, and pushes added hazards to connected clients.  To run the Node.js server:
```
cd nodejs && ./nodeListen.js
```

After each run, you can run the following executable to reset everything:
```
./testSystem
```
This command will
1. Delete PostgreSQL Hazards table and Drive table.
2. Create a new PostgreSQL Hazards table and Drive table
3. Test the system (reading JSON files, connecting to PostgreSQL, etc.)
If all tests are passed, you should be fine to run the system live!

### Setup

This codebase requires a config file in config directory named "DatabaseConfig.JSON".

```
{
    "dbname": "testdb",
    "user": "osm",
    "password": "mypassword",
    "hostaddr": "127.0.0.1",
    "port": "5432"
}
```

To make a config file in the config directory:
```
./makeDatabase -c "nameofdatabase"
```
Note that the default settings are in 'settings.h' already

The web server display requires the following actions:
1. Copy contents of html folder into /var/www/html
2. Create a symlink between ZoneCloudlet/images and /var/www/html/images/cloudletImages via (note that you may need to be root)
```
ln -s ZoneCloudlet/images /var/www/html/images/cloudletImages
```


To set up the tile map server, a good resource is 

[OSM Tile Map Server Setup](https://www.linuxbabe.com/linux-server/openstreetmap-tile-server-ubuntu-16-04)
