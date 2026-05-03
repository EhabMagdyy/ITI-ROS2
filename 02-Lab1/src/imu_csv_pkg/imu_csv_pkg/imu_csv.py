import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Imu
import csv
import os

class imu_csv_node(Node):
    def __init__(self):
        super().__init__('imu_csv_node')
        self.publisher_ = self.create_publisher(Imu, '/imu/data', 10)
        self.timer = self.create_timer(0.1, self.publisher_callback)

        # Load CSV
        csv_path = os.path.abspath(
            os.path.join(os.path.dirname(__file__), '..', 'data', '/home/ehab/Documents/ITI_9Months/ROS/02-Lab1/src/imu_csv_pkg/imu_data.csv')
        )

        with open(csv_path, 'r') as f:
            reader = csv.DictReader(f)
            self.data = list(reader)    # Load all data into data list

        self.index = 0

    def publisher_callback(self):
        if not self.data:
            return
        row = self.data[self.index]
        msg = Imu()
        msg.header.stamp = self.get_clock().now().to_msg()

        # Fill IMU message fields from CSV file
        msg.linear_acceleration.x = float(row['acc_x'])
        msg.linear_acceleration.y = float(row['acc_y'])
        msg.linear_acceleration.z = float(row['acc_z'])

        msg.angular_velocity.x = float(row['ang_x'])
        msg.angular_velocity.y = float(row['ang_y'])
        msg.angular_velocity.z = float(row['ang_z'])

        msg.orientation.w = float(row['orient_w'])
        msg.orientation.x = float(row['orient_x'])
        msg.orientation.y = float(row['orient_y'])
        msg.orientation.z = float(row['orient_z'])

        self.get_logger().info(f'Publishing IMU data: {row}')

        self.publisher_.publish(msg)
        self.index = (self.index + 1) % len(self.data)  # Loop

def main(args=None):
    rclpy.init(args=args)
    node = imu_csv_node()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
