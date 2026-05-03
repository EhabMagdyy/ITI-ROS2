#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <memory>
#include <functional>

class VelocitySub : public rclcpp::Node {
public:
    VelocitySub() : Node("velocity_subscriber"){
        subscription_ = this->create_subscription<geometry_msgs::msg::Twist>("/cmd_vel", 5, std::bind(&VelocitySub::velocity_callback, this, std::placeholders::_1));
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel_limited", 5);
    }

private:
    void velocity_callback(const geometry_msgs::msg::Twist::SharedPtr msg){
        RCLCPP_INFO(this->get_logger(), "Received Velocity: %.2f - Receivied Angular: %.2f", msg->linear.x, msg->angular.z);
        double linear = msg->linear.x;
        double angular = msg->angular.z;
        // log a warning
        if(msg->linear.x > 1.0){
            RCLCPP_WARN(this->get_logger(), "Linear velocity is greater than 1.0: %.2f", msg->linear.x);
            linear = 1.0;
        }
        if(msg->angular.z > 1.5){
            RCLCPP_WARN(this->get_logger(), "Angular velocity is greater than 1.5: %.2f", msg->angular.z);
            angular = 1.5;
        }
        geometry_msgs::msg::Twist capped_msg;
        capped_msg.linear.x = linear;
        capped_msg.angular.z = angular;
        publisher_->publish(capped_msg);
    }

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscription_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
};

int main(int argc, char ** argv){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<VelocitySub>());
    rclcpp::shutdown();
  return 0;
}
