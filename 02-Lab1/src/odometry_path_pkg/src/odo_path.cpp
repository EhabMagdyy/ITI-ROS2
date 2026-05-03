#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"

using namespace std::chrono_literals;

class OdometryPath : public rclcpp::Node {
public:
    OdometryPath() : Node("odo_path"){
        publisher_ = this->create_publisher<nav_msgs::msg::Odometry>("/odom", 5);
        timer_ = this->create_wall_timer(100ms, std::bind(&OdometryPath::pub_callback, this));
    }

private:
    void pub_callback(){
        auto message = nav_msgs::msg::Odometry();
        message.header.stamp = this->get_clock()->now();
        message.header.frame_id = "odom";
        message.child_frame_id = "base_link";
        message.pose.pose.position.x = x;
        message.pose.pose.orientation.w = 1.0;
        message.twist.twist.linear.x = 1.0;
        x += 0.1;
        publisher_->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr publisher_;
    double x = 0;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<OdometryPath>());
    rclcpp::shutdown();
    return 0;
}