# Get Started with simple ROS2 Project

## Setup Workspace
```sh
mkdir -p 00-GetStarted/src 
cd 00-GetStarted/src
source /opt/ros/humble/setup.bash
```

---

## Create C++ Node
```sh
ros2 pkg create --build-type ament_cmake cpp_node --dependencies rclcpp
```

### Write the C++ Code to src/hello_node.cpp

### Edit CMakeLists.txt 
```sh
add_executable(hello_node src/hello_node.cpp)
ament_target_dependencies(hello_node rclcpp)

install(TARGETS hello_node
  DESTINATION lib/${PROJECT_NAME})
```

---

## Create Python Node
```sh
ros2 pkg create --build-type ament_python --node-name hello_node my_python_pkg --dependencies rclpy
```

### Write the Python Code (my_python_pkg/hello_node.py)

---

## Build & Run
```sh
# back to 00-GetStarted/
cd ../
# build
colcon build --symlink-install
# source install
source install/setup.bash
# Verify packages
ros2 pkg list | grep my_python_pkg
ros2 pkg list | grep cpp_node
# run
ros2 run my_python_pkg hello_node
ros2 run cpp_node hello_node
```
