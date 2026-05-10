#include <chrono>
#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "turtle_on_circle/msg/robot_status.hpp"
#include "turtlesim/msg/pose.hpp"

using namespace std::chrono_literals;

class StatusPublisher : public rclcpp::Node{
public:
    StatusPublisher() : Node("status_publisher"){
        this->declare_parameter("status_rate", 5.0);
        double status_rate = this->get_parameter("status_rate").as_double();

        publisher_ = this->create_publisher<turtle_on_circle::msg::RobotStatus>("/robot/status", 5);
        subscriber_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 5, std::bind(&StatusPublisher::status_callback, this, std::placeholders::_1));
        timer_ = this->create_wall_timer(std::chrono::milliseconds(static_cast<int>(1000.0 / status_rate)), std::bind(&StatusPublisher::publisher_callback, this));
        RCLCPP_INFO(this->get_logger(), "Status Publisher Started...");
    }
private:
    void publisher_callback(){
        auto status = turtle_on_circle::msg::RobotStatus();

        // Copy pose
        status.pose.x = current_pose_.x;
        status.pose.y = current_pose_.y;
        status.pose.theta = current_pose_.theta;

        if(first_pose_) {
            prev_theta_ = current_pose_.theta;
            first_pose_ = false;
        }
        // lap counter (when it crosses from positive to negative => 3.14 to -3.14)
        if(prev_theta_ > 0 && current_pose_.theta < 0 && (prev_theta_ - current_pose_.theta) > 3.0) {
            lap_count_++;
        }
        status.lap_count = lap_count_;

        // check if the turtle is moving (if theta changes)
        if(std::abs(current_pose_.theta - prev_theta_) > 0.01) {
            status.state = "running";
        }
        else {
            status.state = "stopped";
        }

        prev_theta_ = current_pose_.theta;

        // Dummy
        status.temperature = 33.5;
        publisher_->publish(status);
        RCLCPP_INFO(this->get_logger(), "Publishing Robot Status");
    }
    void status_callback(const turtlesim::msg::Pose::SharedPtr msg){
        current_pose_ = *msg;
    }

    rclcpp::Publisher<turtle_on_circle::msg::RobotStatus>::SharedPtr publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscriber_;
    turtlesim::msg::Pose current_pose_;
    double prev_theta_ = 0.0;
    int lap_count_ = 0;
    bool first_pose_ = true;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<StatusPublisher>());
    rclcpp::shutdown();
  return 0;
}
