import rclpy
from rclpy.node import Node
import random
from sensor_msgs.msg import Range

class ObsDetectNode(Node):
    def __init__(self):
        super().__init__('obs_detect_node')
        self.publisher_ = self.create_publisher(Range, '/sensor/distance', 10)
        self.timer = self.create_timer(0.1, self.timer_callback)

    def timer_callback(self):
        msg = Range()
        msg.range = random.uniform(0.03, 5.0)
        msg.header.stamp = self.get_clock().now().to_msg()
        self.publisher_.publish(msg)
        self.get_logger().info('Publishing: "%f"' % msg.range)

def main(args=None):
    rclpy.init(args=args)
    node = ObsDetectNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()