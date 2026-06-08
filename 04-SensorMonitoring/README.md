# LiDAR-Based Obstacle Monitoring System

## Overview

This project is a **ROS 2 based obstacle monitoring system** that combines:

- **RPLiDAR** for 360° environment scanning
- **HC-SR04 Ultrasonic Sensor** for close-range distance measurement
- **Potentiometer** for adjustable alert threshold
- **Arduino** for sensor acquisition
- **ROS 2 Nodes** for data processing and monitoring

The system continuously measures nearby obstacles and generates an alert whenever an object gets closer than the user-defined threshold.

---

# System Architecture

```text
                    +----------------------+
                    |      Potentiometer   |
                    |   Adjustable Limit   |
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |      Arduino UNO     |
                    |----------------------|
                    |  Reads:
                    |   - Potentiometer
                    |   - HC-SR04 Sensor
                    +----------+-----------+
                               |
                         Serial UART
                               |
                               v
+-------------------+   +------------------------+
|     RPLiDAR       |   |   ROS2 Python Node     |
|   Laser Scanner   +--->   arduino_bridge.py    |
+---------+---------+   +-----------+------------+
          |                         |
          | /scan                   |
          |                         |
          |               +---------+---------+
          |               |                   |
          v               v                   v
   LaserScan Topic   /ultrasonic       /pot_threshold
                                           |
                                           |
                                           v
                              +-----------------------+
                              |   monitor_node.cpp    |
                              |-----------------------|
                              |  - Finds minimum      |
                              |    obstacle distance  |
                              |  - Compares against   |
                              |    threshold          |
                              |  - Generates alerts   |
                              +-----------+-----------+
                                          |
                           +--------------+-------------+
                           |                            |
                           v                            v
                    /min_distance                  /alert
```

---

# Project Structure

```text
04-Project/
│
├── LiDAR/
│   └── src/
│       ├── monitor/
│       │   ├── include/
│       │   ├── src/
│       │   │   └── monitor_node.cpp
│       │   ├── CMakeLists.txt
│       │   └── package.xml
│       │
│       ├── sensors/
│       │   ├── launch/
│       │   ├── sensors/
│       │   │   └── arduino_bridge.py
│       │   ├── setup.py
│       │   └── package.xml
│       │
│       └── rplidar_ros/
│           ├── launch/
│           ├── sdk/
│           └── src/
│
├── US_Pot/
│   └── US_Pot.ino
│
└── LiDARBag/
    ├── metadata.yaml
    └── rosbag2_*.db3
```

---

# Hardware Components

| Component | Purpose |
|---|---|
| Arduino UNO | Reads ultrasonic + potentiometer |
| HC-SR04 Ultrasonic Sensor | Measures close-range distance |
| Potentiometer | Controls alert threshold |
| RPLiDAR | 360° obstacle detection |
| PC / Raspberry Pi | Runs ROS 2 nodes |

---

# ROS 2 Nodes

## 1. Arduino Bridge Node

File:

```text
sensors/sensors/arduino_bridge.py
```

### Responsibilities

- Reads serial data from Arduino
- Parses:
  - Potentiometer value
  - Ultrasonic distance
- Publishes ROS 2 topics

### Published Topics

| Topic | Type | Description |
|---|---|---|
| `/ultrasonic` | `sensor_msgs/Range` | Ultrasonic distance |
| `/pot_threshold` | `std_msgs/Float32` | Threshold value |

---

## 2. Monitor Node

File:

```text
monitor/src/monitor_node.cpp
```

### Responsibilities

- Subscribes to:
  - LiDAR scan
  - Ultrasonic range
  - Potentiometer threshold
- Calculates minimum distance
- Generates obstacle alert

### Subscribed Topics

| Topic | Type |
|---|---|
| `/scan` | `sensor_msgs/LaserScan` |
| `/ultrasonic` | `std_msgs/Float32` |
| `/pot_threshold` | `std_msgs/Float32` |

### Published Topics

| Topic | Type | Description |
|---|---|---|
| `/min_distance` | `std_msgs/Float32` | Minimum detected distance |
| `/alert` | `std_msgs/Bool` | Collision warning |

---

# Data Flow

```text
         Ultrasonic Sensor
                 |
                 v
          +-------------+
          |  Arduino    |
          +-------------+
                 |
                 v
         Serial Communication
                 |
                 v
      +----------------------+
      |  arduino_bridge.py   |
      +----------------------+
          |            |
          |            |
          v            v
    /ultrasonic   /pot_threshold
           \          /
            \        /
             v      v
          +-------------+
          | monitor_node|
          +-------------+
                 ^
                 |
               /scan
                 ^
                 |
            +---------+
            | LiDAR   |
            +---------+
```

---

# Arduino Logic

The Arduino performs:

1. Read potentiometer value
2. Trigger ultrasonic pulse
3. Measure echo duration
4. Convert echo time to distance
5. Send formatted serial data

### Serial Output Format

```text
POT_VALUE | DISTANCE
```

Example:

```text
512 | 35
```

Where:

- `512` = potentiometer reading
- `35` = distance in centimeters

---

# Monitor Logic

The monitor node performs:

```text
minimum_distance =
    min(lidar_distance, ultrasonic_distance)

IF minimum_distance < threshold:
    alert = TRUE
ELSE:
    alert = FALSE
```

---

# Build Instructions

## Prerequisites

Install:

- ROS 2 Humble
- colcon
- Python serial package

```bash
sudo apt install python3-colcon-common-extensions
pip install pyserial
```

---

# Build Workspace

```bash
cd LiDAR

colcon build

source install/setup.bash
```

---

# Running the Project

## 1. Upload Arduino Code

Open:

```text
US_Pot/US_Pot.ino
```

Upload using Arduino IDE.

---

## 2. Start LiDAR Driver

```bash
ros2 launch rplidar_ros rplidar_a1_launch.py
```

---

## 3. Run Arduino Bridge

```bash
ros2 run sensors arduino_bridge
```

---

## 4. Run Monitor Node

```bash
ros2 run monitor monitor_node
```

---

# ROS Topic Graph

```text
                 +----------------+
                 |  RPLiDAR Node  |
                 +--------+-------+
                          |
                          v
                       /scan
                          |
                          v
+----------------+   +----------------+   +----------------+
| Arduino Bridge |-->|  Monitor Node |-->| Alert Consumer |
+--------+-------+   +----------------+   +----------------+
         |                    |
         |                    |
         v                    v
   /ultrasonic         /min_distance

         |
         v
  /pot_threshold
```

---

# Example Workflow

```text
1. User rotates potentiometer
2. Threshold changes dynamically
3. LiDAR scans surroundings
4. Ultrasonic measures nearby object
5. Monitor node computes nearest object
6. Alert is triggered if obstacle is too close
```

---

# Possible Improvements

- Add GUI visualization using RViz
- Add buzzer or LED alerts
- Support multiple ultrasonic sensors
- Add camera integration
- Add autonomous obstacle avoidance
- Deploy on Raspberry Pi robot

---

# LiDAR Bag Files

The `LiDARBag/` directory contains recorded ROS 2 bag data.

These files can be replayed using:

```bash
ros2 bag play <bag_name>
```

Example:

```bash
ros2 bag play rosbag2_2026_05_17-16_11_42
```

---

# Future Enhancements

## Suggested Features

```text
+-------------------------+
|  Future Improvements    |
+-------------------------+
| - Web Dashboard         |
| - Mobile Notifications  |
| - SLAM Integration      |
| - Path Planning         |
| - AI Obstacle Detection |
+-------------------------+
```

---

# Troubleshooting

## Serial Port Error

Check Arduino connection:

```bash
ls /dev/ttyACM*
```

Update serial port inside:

```python
self.serial_port = '/dev/ttyACM0'
```

---

## LiDAR Permission Issue

Run:

```bash
sudo chmod 666 /dev/ttyUSB0
```

---

# Technologies Used

| Technology | Usage |
|---|---|
| ROS 2 | Middleware |
| Python | Sensor bridge |
| C++ | Monitoring node |
| Arduino | Hardware interface |
| RPLiDAR SDK | LiDAR communication |


