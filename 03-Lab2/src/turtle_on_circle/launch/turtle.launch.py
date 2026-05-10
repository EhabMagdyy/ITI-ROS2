from launch import LaunchDescription
from launch_ros.actions import Node

# Loading parameters
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    # Get the path to the parameters file
    config = os.path.join(
        get_package_share_directory('turtle_on_circle'),
        'params',
        'patrol_params.yaml'
    )

    return LaunchDescription([
        Node(
            package='turtlesim',
            executable='turtlesim_node',
            name='turtlesim'
        ),
        Node(
            package='turtle_on_circle',
            executable='patrol_controller',
            name='patrol_controller',
            output='screen',
            # Load parameters from the YAML file
            parameters=[config]
        ),
        Node(
            package='turtle_on_circle',
            executable='status_publisher',
            name='status_publisher',
            output='screen',
            # Load parameters from the YAML file
            parameters=[config]
        ),
    ])