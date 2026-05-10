#include <chrono>
#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_srvs/srv/empty.hpp"

using namespace std::chrono_literals;

class PatrolController : public rclcpp::Node{
public:
    PatrolController() : Node("patrol_controller"), running_(true){
        // Declare parameters with default values
        this->declare_parameter("linear_speed", 1.5);
        this->declare_parameter("angular_speed", 1.0);
        // Publisher for turtle movement
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 5);
        // Timer to continuously publish turtle control commands
        timer_ = this->create_wall_timer(100ms, std::bind(&PatrolController::timer_callback, this));
        // Stop service to stop the turtle
        stop_service_ = this->create_service<std_srvs::srv::Empty>("/stop", std::bind(&PatrolController::stop_callback, this, std::placeholders::_1, std::placeholders::_2));
        // Continue service to resume the turtle movement
        continue_service_ = this->create_service<std_srvs::srv::Empty>("/continue", std::bind(&PatrolController::continue_callback, this, std::placeholders::_1, std::placeholders::_2));
        RCLCPP_INFO(this->get_logger(), "Patrol Controller Started...");
    }

private:
    void timer_callback(){
        // Read parameters
        double linear_speed = this->get_parameter("linear_speed").as_double();
        double angular_speed = this->get_parameter("angular_speed").as_double();

        auto msg = geometry_msgs::msg::Twist();
        if(running_){
            msg.linear.x = linear_speed;
            msg.angular.z = angular_speed;
        }
        else{
            msg.linear.x = 0.0;
            msg.angular.z = 0.0;
        }

        publisher_->publish(msg);
        RCLCPP_INFO(this->get_logger(), "Publishing -> linear: %.2f | angular: %.2f | state: %s", msg.linear.x, msg.angular.z, running_ ? "running" : "stopped");
    }

    void stop_callback(const std::shared_ptr<std_srvs::srv::Empty::Request> request, const std::shared_ptr<std_srvs::srv::Empty::Response> response){
        running_ = false;
        RCLCPP_INFO(this->get_logger(), "Turtle movement stopped");
    }

    void continue_callback(const std::shared_ptr<std_srvs::srv::Empty::Request> request, const std::shared_ptr<std_srvs::srv::Empty::Response> response){
        running_ = true;
        RCLCPP_INFO(this->get_logger(), "Turtle movement resumed");
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Service<std_srvs::srv::Empty>::SharedPtr stop_service_;
    rclcpp::Service<std_srvs::srv::Empty>::SharedPtr continue_service_;
    bool running_;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PatrolController>());
    rclcpp::shutdown();
    return 0;
}