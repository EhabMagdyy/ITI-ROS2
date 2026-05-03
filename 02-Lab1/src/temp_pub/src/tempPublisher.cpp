#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <fstream>
#include <fcntl.h>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/temperature.hpp"

using namespace std::chrono_literals;

class TempPublisher : public rclcpp::Node {
public:
    TempPublisher() : Node("temp_pub"){
        publisher_ = this->create_publisher<sensor_msgs::msg::Temperature>("/cpu_temp", 5);
        timer_ = this->create_wall_timer(1000ms, std::bind(&TempPublisher::publisher_callback, this));
    }

private:
    void publisher_callback(){
        // Reading CPU temperature
        int fd = open("/sys/class/thermal/thermal_zone0/temp", O_RDONLY);
        if (fd < 0) {
            RCLCPP_ERROR(this->get_logger(), "Failed to open temperature file");
            return;
        }
        char buffer[16];
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
        close(fd);
        if(bytesRead < 0) {
            RCLCPP_ERROR(this->get_logger(), "Failed to read temperature");
            return;
        }
        // Publishing CPU temperature
        buffer[bytesRead] = '\0';
        auto message = sensor_msgs::msg::Temperature();
        message.header.stamp = this->get_clock()->now();
        message.temperature = std::stof(buffer) / 1000.0;   // Convert millidegree to degree
        RCLCPP_INFO(this->get_logger(), "Publishing CPU Temperature: %.2f", message.temperature);
        publisher_->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<sensor_msgs::msg::Temperature>::SharedPtr publisher_;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TempPublisher>());
    rclcpp::shutdown();
    return 0;
}