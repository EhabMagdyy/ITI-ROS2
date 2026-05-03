# launch/obstacle.launch.py
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='obstacle_detection',
            executable='obsDetect',
            name='obstacle_Detection_Publisher',
            output='screen',
        ),
        Node(
            package='obstacle_detection',
            executable='obsDetectSub',
            name='obstacle_Detection_Subscriber',
            output='screen',
        ),
    ])