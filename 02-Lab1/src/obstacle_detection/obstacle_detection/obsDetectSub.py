import rclpy
from rclpy.node import Node
import random
from sensor_msgs.msg import Range
from std_msgs.msg import String

class ObsDetectNode(Node):
    def __init__(self):
        super().__init__('obs_detect_sub_node')
        self.subscription = self.create_subscription(Range, '/sensor/distance', self.listener_callback, 5)
        self.publisher_ = self.create_publisher(String, '/cmd/stop', 5)
        self.subscription
    
    def listener_callback(self, msg):
        output_msg = String()
        output_msg.data = "False"
        if msg.range < 2.0:
            output_msg.data = "True"
        self.publisher_.publish(output_msg)
        self.get_logger().info('Received: "%s"' % output_msg.data)

def main(args=None):
    rclpy.init(args=args)
    node = ObsDetectNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()