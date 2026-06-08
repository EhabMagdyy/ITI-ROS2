# Robot TF Localization

## Overview

This ROS 2 package builds a **TF tree** for a mobile robot and **fuses sensor data** using an Extended Kalman Filter (EKF). It compares raw IMU orientation against the EKF-fused estimate.

---

### Pub/Sub Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              RECORDED BAG FILE                              │
│  ┌─────────────────┐  ┌──────────────────┐  ┌──────────────────┐            │
│  │ /imu/data/bag   │  │ /odometry/wheel/ │  │ /odometry/gps/   │            │
│  │   (Best Effort) │  │   bag            │  │   bag            │            │
│  │   50-100 Hz     │  │   (Best Effort)  │  │   (Best Effort)  │            │
│  └────────┬────────┘  └────────┬─────────┘  └────────┬─────────┘            │
└───────────┼────────────────────┼─────────────────────┼──────────────────────┘
            │                    │                     │
            │                    │                     │
            ▼                    ▼                     │
┌─────────────────────────────────────────────────────┴────────────────────────┐
│                         YOUR NODES                                           │
│                                                                              │
│  ┌─────────────────────────────────────┐                                     │
│  │     Static TF Publishers (9x)       │                                     │
│  │  ┌─────────────┐  ┌─────────────┐   │                                     │
│  │  │ base_foot   │──│ base_link   │───┼──┬──┬──┬──┬──┬──┬──┐                │
│  │  │  print      │  │             │   │  │  │  │  │  │  │  │                │
│  │  └─────────────┘  └─────────────┘   │  │  │  │  │  │  │  │                │
│  │                                     │  ▼  ▼  ▼  ▼  ▼  ▼  ▼                │
│  │                                     │ imu gps u1 u2 u3 u4 u5 u6           │
│  │                                     │ link link                           │
│  └─────────────────────────────────────┘                                     │
│                                                                              │
│  ┌─────────────────────────────────────┐                                     │
│  │  EKF (robot_localization)           │                                     │
│  │                                     │                                     │
│  │  SUB: /imu/data ◄───────────────────┘ (from bag /imu/data/bag)            │
│  │  SUB: /odometry/wheel ◄─────────────┘ (from bag /odometry/wheel/bag)      │
│  │                                     │                                     │
│  │  PUB: /odometry/local ──────────────┼────► orientation_analysis           │
│  │  PUB: /tf (odom→base_footprint) ────┼────► RViz                           │
│  │                                     │                                     │
│  └─────────────────────────────────────┘                                     │
│                                                                              │
│  ┌─────────────────────────────────────┐                                     │
│  │  orientation_analysis.py            │                                     │
│  │                                     │                                     │
│  │  SUB: /imu/data ◄───────────────────┘ (from bag /imu/data/bag)            │
│  │  SUB: /odometry/local ◄─────────────┘ (from EKF)                          │
│  │                                     │                                     │
│  │  LOG: "IMU: 94.5 | /odometry/local:│ 125.0 | Diff: -30.4"                 │
│  │                                     │                                     │
│  └─────────────────────────────────────┘                                     │
│                                                                              │
│  ┌─────────────────────────────────────┐                                     │
│  │  RViz2                              │                                     │
│  │                                     │                                     │
│  │  SUB: /tf ◄─────────────────────────┘ (from Static TFs + EKF)             │
│  │  SUB: /odometry/local ◄─────────────┘ (from EKF)                          │
│  │                                     │                                     │
│  │  DISPLAY: 3D axes + arrow trail     │                                     │
│  │                                     │                                     │
│  └─────────────────────────────────────┘                                     │
└──────────────────────────────────────────────────────────────────────────────┘
```

---

### Data Flow Summary

| Topic | Publisher | Subscribers | QoS |
|-------|-----------|-------------|-----|
| `/imu/data/bag` | Bag file | (none directly) | Best Effort |
| `/odometry/wheel/bag` | Bag file | (none directly) | Best Effort |
| `/imu/data` | Bag file (remapped) | EKF, orientation_analysis | Best Effort |
| `/odometry/wheel` | Bag file (remapped) | EKF | Best Effort |
| `/odometry/local` | EKF | orientation_analysis, RViz | Reliable |
| `/tf` | Static TFs + EKF | RViz | Reliable |
| `/tf_static` | Static TFs (9x) | RViz | Reliable |

---

### TF Tree

```
odom
│
└── base_footprint          ← EKF publishes this (dynamic)
    │
    └── base_link           ← static (z=0.05)
        │
        ├── imu_link        ← static (x=0.55, z=0.15)
        │
        ├── gps_link        ← static (x=0.30, z=0.30)
        │
        ├── ultrasonic1_link ← static (x=0.60, y=+0.15, yaw=+45°)
        ├── ultrasonic2_link ← static (x=0.60, y=0, yaw=0°)
        ├── ultrasonic3_link ← static (x=0.60, y=-0.15, yaw=-45°)
        ├── ultrasonic4_link ← static (x=0, y=+0.15, yaw=+135°)
        ├── ultrasonic5_link ← static (x=0, y=0, yaw=180°)
        └── ultrasonic6_link ← static (x=0, y=-0.15, yaw=-135°)
```

---

### Key Point: QoS Mismatch

The bag uses **Best Effort**. The EKF publishes **Reliable**. 

Your analysis node uses `qos_profile_sensor_data` (Best Effort) for `/imu/data` to match the bag, but **must** use compatible QoS for `/odometry/local` (Reliable publisher → Best Effort subscriber works).

```
Bag (Best Effort) ──► /imu/data ──► analysis (Best Effort)  ✅ MATCH
Bag (Best Effort) ──► /imu/data ──► EKF (Reliable)          ❌ MISMATCH

EKF (Reliable) ──► /odometry/local ──► analysis (Best Effort)  ✅ COMPATIBLE
EKF (Reliable) ──► /odometry/local ──► RViz (Reliable)         ✅ MATCH
```

In practice, the EKF in `robot_localization` handles the bag's Best Effort input internally, so the EKF receives data correctly.


---

## Package Structure

```
Localization/
├── config/
│   ├── ekf.yaml              # EKF filter configuration
│   └── robot_tf.rviz         # RViz visualization layout
├── launch/
│   └── robot_tf_localization.launch.py  # Master launch file
├── scripts/
│   └── orientation_analysis.py          # Custom orientation comparison node
├── CMakeLists.txt
└── package.xml
```

---

## Component Descriptions

### 1. Static Transform Publishers (Launch File)

**File:** `launch/robot_tf_localization.launch.py`

**Purpose:** Defines the physical geometry of the robot by publishing fixed transforms between frames. This tells ROS where each sensor is located relative to the robot's base.

**Transforms Defined:**

| Transform | x (m) | y (m) | z (m) | yaw (rad) | Description |
|---|---|---|---|---|---|
| `base_footprint` → `base_link` | 0.0 | 0.0 | 0.05 | 0.0 | Robot center, 5 cm above ground |
| `base_link` → `imu_link` | 0.55 | 0.0 | 0.15 | 0.0 | 5 cm from front edge, 15 cm high |
| `base_link` → `gps_link` | 0.30 | 0.0 | 0.30 | 0.0 | Center of robot, 30 cm high |
| `base_link` → `ultrasonic1_link` | 0.60 | 0.15 | 0.15 | +0.785 | Front-Left corner, +45° |
| `base_link` → `ultrasonic2_link` | 0.60 | 0.0 | 0.15 | 0.0 | Front-Center, 0° |
| `base_link` → `ultrasonic3_link` | 0.60 | -0.15 | 0.15 | -0.785 | Front-Right corner, -45° |
| `base_link` → `ultrasonic4_link` | 0.0 | 0.15 | 0.15 | +2.356 | Rear-Left corner, +135° |
| `base_link` → `ultrasonic5_link` | 0.0 | 0.0 | 0.15 | +3.142 | Rear-Center, 180° |
| `base_link` → `ultrasonic6_link` | 0.0 | -0.15 | 0.15 | -2.356 | Rear-Right corner, -135° |

**Robot Dimensions:** Width = 30 cm, Length = 60 cm

**Coordinate Frame:** `base_link` is at the rear axle center. X = forward, Y = left, Z = up.

---

### 2. EKF Node (robot_localization)

**File:** `config/ekf.yaml`

**Purpose:** Fuses wheel odometry and IMU data to produce a filtered estimate of robot position and orientation. Publishes the transform `odom` → `base_footprint`.

**Inputs:**
- `/odometry/wheel` — Wheel encoder odometry (linear velocity X, angular velocity Z)
- `/imu/data` — IMU angular velocity Z only

**Output:**
- `/odometry/local` — Filtered odometry with fused position/orientation estimate
- `odom` → `base_footprint` transform (published to `/tf`)

**Key Parameters:**
- `base_link_frame: base_footprint` — Robot reference frame
- `world_frame: odom` — Fixed world frame for odometry
- `publish_tf: true` — Enable TF broadcasting
- `two_d_mode: false` — Full 3D estimation

---

### 3. Orientation Analysis Node

**File:** `scripts/orientation_analysis.py`

**Purpose:** Subscribes to raw IMU orientation and EKF-fused odometry, then periodically compares their yaw angles to verify sensor consistency.

**Subscriptions:**
- `/imu/data` — Raw IMU orientation (quaternion)
- `/odometry/local` — EKF-fused orientation (quaternion)

**QoS:** Uses `qos_profile_sensor_data` (Best Effort, depth 10) for both subscriptions to match the bag file's QoS profile.

**Output:** Logs yaw difference in degrees every 1 second:
```
IMU: 94.56 deg | /odometry/local: 125.00 deg | Diff: -30.44 deg
```

**Note:** A constant offset is expected because the EKF fuses wheel odometry (which drifts) with IMU angular velocity rate, not absolute IMU orientation.

---

### 4. RViz Configuration

**File:** `config/robot_tf.rviz`

**Purpose:** Pre-configured visualization layout for monitoring the robot state.

**Displays:**
- **Grid** — Reference ground plane
- **TF** — All coordinate frames with names visible (`base_link`, `imu_link`, `gps_link`, `ultrasonic1-6`, etc.)
- **Odometry** — `/odometry/local` trajectory as red arrows showing robot path

**Settings:**
- Fixed Frame: `odom`
- Odometry display: Arrow style, 100 history buffer

---

## TF Tree Structure

```
odom
└── base_footprint          ← Published by EKF (filtered position)
    └── base_link           ← Static: 5 cm above ground
        ├── imu_link        ← Static: front of robot, 15 cm high
        ├── gps_link        ← Static: center, 30 cm high
        ├── ultrasonic1_link ← Static: Front-Left, +45°
        ├── ultrasonic2_link ← Static: Front-Center, 0°
        ├── ultrasonic3_link ← Static: Front-Right, -45°
        ├── ultrasonic4_link ← Static: Rear-Left, +135°
        ├── ultrasonic5_link ← Static: Rear-Center, 180°
        └── ultrasonic6_link ← Static: Rear-Right, -135°
```

---

## QoS Compatibility Notes

The recorded bag file uses the **`sensor_data`** QoS profile with the following settings:
- **Reliability:** Best Effort
- **History:** Keep Last, depth 5
- **Durability:** Volatile

### Important

Nodes subscribing to bag topics **must** use a compatible QoS profile. If a node uses **Reliable** reliability while the bag publishes **Best Effort**, the subscription will fail silently and no messages will be received.

### Solution

The `orientation_analysis.py` node explicitly uses `qos_profile_sensor_data` for both subscriptions:

```python
from rclpy.qos import qos_profile_sensor_data

self.create_subscription(Imu, '/imu/data', self.imu_cb, qos_profile_sensor_data)
self.create_subscription(Odometry, '/odometry/local', self.local_cb, qos_profile_sensor_data)
```

This matches the bag's Best Effort profile and ensures messages are received correctly.

### Bag Topics

The bag publishes sensor data on topics with a `/bag` suffix:
- `/imu/data/bag`
- `/odometry/wheel/bag`
- `/odometry/gps/bag`

The EKF and analysis nodes subscribe to the remapped names (without `/bag`). Ensure your bag player or relay maps these correctly.

---

## How to Build and Run

### Build

```bash
cd ~/Documents/ITI_9Months/ROS/05-Localization
rm -rf build/ install/ log/
source /opt/ros/humble/setup.bash
colcon build --symlink-install
source install/setup.bash
```

### Run

Open **two terminals**:

**Terminal 1 — Play the bag file:**
```bash
source /opt/ros/humble/setup.bash
cd sample_bag_for_localization
ros2 bag play .
```

**Terminal 2 — Launch the robot stack:**
```bash
source /opt/ros/humble/setup.bash
colcon build --symlink-install
source install/setup.bash
ros2 launch Localization robot_tf_localization.launch.py
```

---

## Troubleshooting

| Problem | Cause | Solution |
|---|---|---|
| "No data received on either topic!" | QoS mismatch or bag not playing | Verify bag is playing; check `ros2 topic list` shows active topics |
| Giant yellow/purple blobs in RViz | Covariance ellipsoids enabled | In Odometry display → Covariance → uncheck Position and Orientation |
| TF frames not visible | Fixed Frame wrong | Set Fixed Frame to `odom` in RViz |
| EKF not publishing `/odometry/local` | EKF not receiving inputs | Check `ros2 topic hz /imu/data` and `/odometry/wheel` |
