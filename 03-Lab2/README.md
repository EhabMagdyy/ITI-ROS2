# Lab-2 | Tirtle on a Circle
https://github.com/user-attachments/assets/e1fc8b51-eb06-4532-8ed0-907ec1e4d32f

---

## Structure
```

                    +----------------------+
                    |   turtlesim_node    |
                    |  (ROS 2 simulator)  |
                    +----------+----------+
                               |
                               | publishes
                               v
                    /turtle1/pose (turtlesim/msg/Pose)
                               |
                               |
        +----------------------+----------------------+
        |                                             |
        |                                             |
        v                                             v
+------------------------+                 +--------------------------+
|  status_publisher      |                 |   patrol_controller      |
|------------------------|                 |--------------------------|
| SUBSCRIBES:            |                 | SUBSCRIBES:             |
|  /turtle1/pose         |                 |  (none or optional)     |
|                        |                 |                          |
| PUBLISHES:             |                 | PUBLISHES:              |
|  /robot/status         |                 |  /turtle1/cmd_vel       |
| (RobotStatus.msg)     |                 |  (geometry_msgs/Twist)  |
+-----------+------------+                 +-----------+--------------+
            |                                          |
            |                                          |
            |                                          v
            |                             +--------------------------+
            |                             | turtlesim_node          |
            |                             | executes motion         |
            |                             +--------------------------+
            |
            v
+------------------------------+
| /robot/status topic         |
| RobotStatus.msg             |
|-----------------------------|
| Pose (x, y, theta)          |
| state ("running/stopped")   |
| temperature                 |
| lap_count                  |
+------------------------------+

                    CONTROL SERVICES (patrol_controller)
                          +------------------------+
                          | /stop (std_srvs/Empty)|
                          +-----------+------------+
                                      |
                                      v
                          sets running_ = false
                          → turtle stops
                  
                          +------------------------+
                          | /continue (Empty)     |
                          +-----------+------------+
                                      |
                                      v
                          sets running_ = true
                          → turtle resumes
```

## Build & Run
```sh
colcon build --symlink-install
# source in every terminal
source install/setup.bash
# launch
ros2 launch turtle_on_circle turtle.launch.py
```

## Monitor the robot status
```sh
ros2 topic echo /robot/status
```

## Control Stop/Continue
```sh
ros2 service call /continue std_srvs/srv/Empty
ros2 service call /stop std_srvs/srv/Empty
```

## Change speed (raduis) parameter
```sh
ros2 param set /patrol_controller linear_speed 2.0
```

