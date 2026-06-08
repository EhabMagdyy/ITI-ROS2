#include <cstdio>
#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/bool.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

using namespace std::chrono_literals;

class Monitor : public rclcpp::Node{
public:
  Monitor() : Node("monitor_node"){
    // Subscribers
    pot_sub_ = this->create_subscription<std_msgs::msg::Float32>("/pot_threshold", 10, std::bind(&Monitor::pot_callback, this, std::placeholders::_1));
    ultra_sub_ = this->create_subscription<std_msgs::msg::Float32>("/ultrasonic", 10, std::bind(&Monitor::ultra_callback, this, std::placeholders::_1));
    scan_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>("/scan", 10, std::bind(&Monitor::scan_callback, this, std::placeholders::_1));
    
    // Publisher
    min_dist_pub_ = this->create_publisher<std_msgs::msg::Float32>("/min_distance", 10);
    alert_pub_ = this->create_publisher<std_msgs::msg::Bool>("/alert", 10);

    // Timer
    timer_ = this->create_wall_timer(100ms, std::bind(&Monitor::publisher_callback, this));
  }

private:
  void pot_callback(const std_msgs::msg::Float32::SharedPtr msg){
    pot_threshold_ = msg->data;
    RCLCPP_INFO(this->get_logger(), "Received Potentiometer Threshold: %.2f", pot_threshold_);
  }
  void ultra_callback(const std_msgs::msg::Float32::SharedPtr msg){
    ultrasonic_range_ = msg->data;
    RCLCPP_INFO(this->get_logger(), "Received Ultrasonic Range: %.2f", ultrasonic_range_);
  }
  void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg){
    min_scan_distance_ = std::numeric_limits<float>::infinity();
    for(const auto& range : msg->ranges){
      if(range < min_scan_distance_){
        min_scan_distance_ = range;
      }
    }
    RCLCPP_INFO(this->get_logger(), "Received Minimum Scan Distance: %.2f", min_scan_distance_);
  }

  void publisher_callback(){
    std_msgs::msg::Float32 min_dist;
    min_dist.data = min_scan_distance_ < ultrasonic_range_ ? min_scan_distance_ : ultrasonic_range_;
    min_dist_pub_->publish(min_dist);
    RCLCPP_INFO(this->get_logger(), "Published Minimum Distance: %.2f", min_dist.data);

    std_msgs::msg::Bool alert;
    alert.data = min_dist.data < pot_threshold_;
    alert_pub_->publish(alert);
    RCLCPP_INFO(this->get_logger(), "Published Alert: %s", alert.data ? "TRUE" : "FALSE");
  }

  rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr pot_sub_;
  rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr ultra_sub_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
  rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr min_dist_pub_;
  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr alert_pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  float pot_threshold_ = 0.0;
  float ultrasonic_range_ = 0.0;
  float min_scan_distance_ = std::numeric_limits<float>::infinity();
};

int main(int argc, char ** argv){
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Monitor>());
  rclcpp::shutdown();
  return 0;
}
