import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node

def generate_launch_description():
    rplidar_share = get_package_share_directory('rplidar_ros')
    rplidar_launch_dir = os.path.join(rplidar_share, 'launch')
    
    rviz_config_file = os.path.join(rplidar_share, 'rviz', 'rplidar.rviz')

    rplidar_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(rplidar_launch_dir, 'rplidar_a1_launch.py'))
    )

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_config_file],
        output='screen'
    )

    sensors_node = Node(
        package='sensors',
        executable='arduino_bridge', 
        name='arduino_bridge_node',
        output='screen'
    )

    monitor_node = Node(
        package='monitor',
        executable='monitor_node', 
        name='system_monitor_node',
        output='screen'
    )

    return LaunchDescription([
        rplidar_launch,
        rviz_node,
        sensors_node,
        monitor_node
    ])