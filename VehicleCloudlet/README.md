# Vehicle Cloudlet

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