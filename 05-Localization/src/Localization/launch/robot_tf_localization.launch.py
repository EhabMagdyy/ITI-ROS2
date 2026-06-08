from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
     pkg = get_package_share_directory('Localization')

     static_tfs = [
        Node(package='tf2_ros', executable='static_transform_publisher',
             arguments=['0','0','0.05','0','0','0','base_footprint','base_link']),
        Node(package='tf2_ros', executable='static_transform_publisher',
             arguments=['0.55','0','0.15','0','0','0','base_link','imu_link']),
        Node(package='tf2_ros', executable='static_transform_publisher',
             arguments=['0.30','0','0.30','0','0','0','base_link','gps_link']),
        Node(package='tf2_ros', executable='static_transform_publisher',
             arguments=['0.60','0.15','0.15','0.785398','0','0','base_link','ultrasonic1_link']),
        Node(package='tf2_ros', executable='static_transform_publisher',
             arguments=['0.60','0','0.15','0','0','0','base_link','ultrasonic2_link']),
        Node(package='tf2_ros', executable='static_transform_publisher',
             arguments=['0.60','-0.15','0.15','-0.785398','0','0','base_link','ultrasonic3_link']),
        Node(package='tf2_ros', executable='static_transform_publisher',
             arguments=['0','0.15','0.15','2.35619','0','0','base_link','ultrasonic4_link']),
        Node(package='tf2_ros', executable='static_transform_publisher',
             arguments=['0','0','0.15','3.14159','0','0','base_link','ultrasonic5_link']),
        Node(package='tf2_ros', executable='static_transform_publisher',
             arguments=['0','-0.15','0.15','-2.35619','0','0','base_link','ultrasonic6_link']),
     ]

     ekf = Node(
        package='robot_localization',
        executable='ekf_node',
        name='ekf_filter_node',
        output='screen',
        parameters=[os.path.join(pkg, 'config', 'ekf.yaml')],
        remappings=[('/odometry/filtered', '/odometry/local')],
     )

     analysis = Node(
        package='Localization',
        executable='orientation_analysis.py',
        name='orientation_analysis',
        output='screen',
     )

     rviz = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', os.path.join(pkg, 'config', 'robot_tf.rviz')],
        output='screen',
     )

     return LaunchDescription(static_tfs + [ekf, analysis, rviz])