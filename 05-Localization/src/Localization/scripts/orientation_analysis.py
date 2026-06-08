#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from sensor_msgs.msg import Imu
from nav_msgs.msg import Odometry
import math

def quaternion_to_yaw(x, y, z, w):
    siny_cosp = 2.0 * (w * z + x * y)
    cosy_cosp = 1.0 - 2.0 * (y * y + z * z)
    return math.atan2(siny_cosp, cosy_cosp)

class OrientationAnalysis(Node):
    def __init__(self):
        super().__init__('orientation_analysis')
        
        # Bag topics use Best Effort
        best_effort_qos = QoSProfile(
            history=HistoryPolicy.KEEP_LAST,
            depth=10,
            reliability=ReliabilityPolicy.BEST_EFFORT
        )
        
        # EKF publishes Reliable
        reliable_qos = QoSProfile(
            history=HistoryPolicy.KEEP_LAST,
            depth=10,
            reliability=ReliabilityPolicy.RELIABLE
        )
        
        # Subscribe to /bag topics
        self.create_subscription(Imu, '/imu/data/bag', self.imu_cb, best_effort_qos)
        self.create_subscription(Odometry, '/odometry/local', self.local_cb, reliable_qos)
        
        self.imu_yaw = None
        self.local_yaw = None
        self.imu_count = 0
        self.local_count = 0
        self.create_timer(1.0, self.compare)

    def imu_cb(self, msg):
        q = msg.orientation
        self.imu_yaw = quaternion_to_yaw(q.x, q.y, q.z, q.w)
        self.imu_count += 1

    def local_cb(self, msg):
        q = msg.pose.pose.orientation
        self.local_yaw = quaternion_to_yaw(q.x, q.y, q.z, q.w)
        self.local_count += 1

    def compare(self):
        if self.imu_yaw is None and self.local_yaw is None:
            self.get_logger().warn('No data received on either topic!')
            return
        if self.imu_yaw is None:
            self.get_logger().warn(f'IMU: NO DATA ({self.imu_count} msgs) | Local: {math.degrees(self.local_yaw):.2f} deg')
            return
        if self.local_yaw is None:
            self.get_logger().warn(f'IMU: {math.degrees(self.imu_yaw):.2f} deg | Local: NO DATA ({self.local_count} msgs)')
            return
            
        i = math.degrees(self.imu_yaw)
        l = math.degrees(self.local_yaw)
        diff = ((i - l + 180) % 360) - 180
        self.get_logger().info(
            f'IMU: {i:.2f} deg | /odometry/local: {l:.2f} deg | Diff: {diff:.2f} deg'
        )

def main(args=None):
    rclpy.init(args=args)
    rclpy.spin(OrientationAnalysis())
    rclpy.shutdown()

if __name__ == '__main__':
    main()