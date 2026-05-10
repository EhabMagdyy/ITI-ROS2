cmake_minimum_required(VERSION 3.8)
project(turtle_on_circle)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# =========================================================
# Dependencies
# =========================================================
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)
find_package(std_srvs REQUIRED)

# =========================================================
# ROSIDL (CUSTOM MESSAGES)
# =========================================================
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/RobotStatus.msg"
  DEPENDENCIES geometry_msgs
)

# =========================================================
# EXECUTABLES (MUST BE BEFORE LINKING TYPESUPPORT)
# =========================================================

add_executable(status_publisher src/status_publisher.cpp)
add_executable(patrol_controller src/patrol_controller.cpp)

# =========================================================
# DEPENDENCIES FOR NODES
# =========================================================

ament_target_dependencies(status_publisher
  rclcpp
  geometry_msgs
  turtlesim
)

ament_target_dependencies(patrol_controller
  rclcpp
  geometry_msgs
  std_srvs
)

# =========================================================
# LINK GENERATED MESSAGE TYPESUPPORT (MODERN WAY)
# =========================================================

rosidl_get_typesupport_target(cpp_typesupport
  ${PROJECT_NAME}
  "rosidl_typesupport_cpp"
)

target_link_libraries(status_publisher "${cpp_typesupport}")
target_link_libraries(patrol_controller "${cpp_typesupport}")

# =========================================================
# INSTALL TARGETS
# =========================================================

install(TARGETS
  status_publisher
  patrol_controller
  DESTINATION lib/${PROJECT_NAME}
)

# =========================================================
# INSTALL LAUNCH + PARAMS
# =========================================================

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY params
  DESTINATION share/${PROJECT_NAME}
)

# =========================================================
# TESTING
# =========================================================

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()