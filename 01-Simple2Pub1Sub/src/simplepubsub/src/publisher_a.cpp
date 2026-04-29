#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <random>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

using namespace std::chrono_literals;

class PublisherA : public rclcpp::Node {
public:
    PublisherA() : Node("publisher_a"){
        publisher_ = this->create_publisher<std_msgs::msg::Int32>("topic_a", 5);
        timer_ = this->create_wall_timer(1000ms, std::bind(&PublisherA::timer_callback, this));
        // generate random number between 0 and 100
        std::random_device rd;
        gen_ = std::mt19937(rd());
        dis_ = std::uniform_int_distribution<>(0, 100);
    }

private:
    void timer_callback(){
        auto message = std_msgs::msg::Int32();

        message.data = dis_(gen_);
        // log the message being published
        RCLCPP_INFO(this->get_logger(), "Publishing: %d", message.data);
        // publish the message to the "calc_b" topic
        publisher_->publish(message);
    }
    // timer and publisher are shared pointers to manage their lifetimes
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;
    // random number generator and distribution for generating random numbers
    std::mt19937 gen_;
    std::uniform_int_distribution<> dis_;
};

int main(int argc, char * argv[]){
    // Keeps node alive
    rclcpp::init(argc, argv);
    // Shutdown the ROS 2 system after the node is done
    rclcpp::spin(std::make_shared<PublisherA>());
    rclcpp::shutdown();
    return 0;
}