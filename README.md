# LiveMap

If you find this useful, please cite the related paper:
```
@inproceedings{christensen2019towards,
  title={Towards a Distraction-free Waze},
  author={Christensen, Kevin and Mertz, Christoph and Pillai, Padmanabhan and Hebert, Martial and Satyanarayanan, Mahadev},
  booktitle={Proceedings of the 20th International Workshop on Mobile Computing Systems and Applications},
  pages={15--20},
  year={2019},
  organization={ACM}
}
```

## Zone Cloudlet

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


## Vehicle Cloudlet

### Installation and Running
Assuming that you have already created the catkin workspace in Vehicle Cloudlet root directory, you can create this ROS package via:
```
catkin_create_pkg livemap_ros std_msgs rospy roscpp
```
To build the package, run:
```
catkin_make
```
**Demo Instructions**
For running the live demo, the Vehicle Cloudlet is interfaced via an ethernet connection.  To get the IP address, first make sure that your wired connection is set to "Shared to other computers" under Networks Connections --> Edit --> IPv4 Settings --> Shared to other computers.  Then you can get the IP address via:
```
cat /var/lib/misc/dnsmasq.leases
```
Now you can ssh with X11 forwarding via the IP address from the last command and this command
```
ssh -X cloudlet@<ipaddr>
```
After you have the Android phone plugged in (with tethering enabled under settings), you can run 

```
sudo ip route del default via 10.42.0.1 dev enp26s0f0
```
This will make sure that the default route is through the cell phone link.

Now that we are connected to the Vehicle Cloudlet with internet connection, we can start the Vehicle Cloudlet main program.
```
tmux
# Issue each following command in a new tmux terminal
roscore
roslaunch livemap_ros data_collection_edge.launch
rosrun livemap_ros cone_detector.py
rosrun livemap_ros navlabMain
```
**Please change the model prototxt and caffemodel files in cone_detector.py and pothole_detector.py to the absolute path.**