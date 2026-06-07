import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Range
from std_msgs.msg import Float32
import serial

class SensorNode(Node):
    def __init__(self):
        super().__init__('sensor_node')
        
        # 1. Define Publishers
        self.ultra_pub = self.create_publisher(Range, '/ultrasonic', 10)
        self.pot_pub = self.create_publisher(Float32, '/pot_threshold', 10)
    
        # 2. Configure Serial Connection
        self.serial_port = '/dev/ttyACM0' 
        self.baud_rate = 9600
        
        try:
            self.ser = serial.Serial(self.serial_port, self.baud_rate, timeout=0.1)
            self.ser.reset_input_buffer()
            self.get_logger().info(f"Successfully connected to {self.serial_port}")
        except serial.SerialException as e:
            self.get_logger().error(f"Could not open serial port {self.serial_port}: {e}")
            raise e

        self.timer = self.create_timer(0.02, self.timer_callback)

    def timer_callback(self):
        try:
            # Check if data is waiting in the serial buffer
            if self.ser.in_waiting > 0:
                line = self.ser.readline().decode('utf-8').rstrip()
                
                # Verify data format matches expected "POT | ULTRASONIC"
                if '|' in line:
                    parts = line.split('|')
                    if len(parts) == 2:
                        pot_raw = float(parts[0].strip())
                        dist_raw = float(parts[1].strip())

                        # --- Publish Potentiometer Threshold ---
                        pot_msg = Float32()
                        pot_msg.data = pot_raw / 1024.0  * 5.0 # 0 to 5 meter
                        self.pot_pub.publish(pot_msg)

                        # --- Publish Ultrasonic Range (Converting cm to meters) ---
                        range_msg = Range()
                        range_msg.header.stamp = self.get_clock().now().to_msg()
                        range_msg.field_of_view = 0.26  # ~15 degrees for HC-SR04 in radians
                        range_msg.min_range = 0.02      # 2 cm
                        range_msg.max_range = 4.0       # 400 cm
                        range_msg.range = dist_raw / 100.0  # Convert cm to meters

                        self.ultra_pub.publish(range_msg)

                        self.get_logger().info(f"Published -> Dist: {range_msg.range:.2f}m | Pot: {pot_msg.data}")
                        
        except Exception as e:
            self.get_logger().error(f"Error parsing serial data: {e}")

    def destroy_node(self):
        if hasattr(self, 'ser') and self.ser.is_open:
            self.ser.close()
            self.get_logger().info("Serial port securely closed.")
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    node = SensorNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.try_shutdown()

if __name__ == '__main__':
    main()