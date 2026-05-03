from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='temp_pub',
            executable='tempPublisher',
            name='Temperature_Publisher',
            output='screen',
        ),
    ])