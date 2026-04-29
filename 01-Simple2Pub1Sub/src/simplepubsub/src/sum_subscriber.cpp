#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

using std::placeholders::_1;

class SumSub : public rclcpp::Node {
public:
    SumSub() : Node("sum_subscriber"){
        sub_a_ = this->create_subscription<std_msgs::msg::Int32>("/topic_a", 5, std::bind(&SumSub::callback_a, this, _1));
        sub_b_ = this->create_subscription<std_msgs::msg::Int32>("/topic_b", 5, std::bind(&SumSub::callback_b, this, _1));

        publisher_ = this->create_publisher<std_msgs::msg::Int32>("/topic_sum", 5);

        got_a_ = false;
        got_b_ = false;
    }

private:
    void callback_a(const std_msgs::msg::Int32 & msg){
        a_ = msg.data;
        got_a_ = true;

        RCLCPP_INFO(this->get_logger(), "Received A: %d", a_);
        compute_sum();
    }

    void callback_b(const std_msgs::msg::Int32 & msg){
        b_ = msg.data;
        got_b_ = true;

        RCLCPP_INFO(this->get_logger(), "Received B: %d", b_);
        compute_sum();
    }

    void compute_sum(){
        if(got_a_ && got_b_){
            int sum = a_ + b_;
            RCLCPP_INFO(this->get_logger(), "Sum: %d + %d = %d", a_, b_, sum);

            // Publish the sum
            auto message = std_msgs::msg::Int32();
            message.data = sum;
            RCLCPP_INFO(this->get_logger(), "Published Sum: %d", sum);
            publisher_->publish(message);
        }
    }

    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr sub_a_;
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr sub_b_;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;

    int a_;
    int b_;
    bool got_a_;
    bool got_b_;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SumSub>());
    rclcpp::shutdown();
    return 0;
}