#define Phoenix_No_WPI // this ain't yo mama's frc team bud, we are doing
                       // serious shiz here man c'mon get it together
#include "ctre/Phoenix.h"
#include <iostream>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"

using namespace std::chrono_literals;

#include "ctre/phoenix/unmanaged/Unmanaged.h" //needed for watchdog enable

using namespace ctre::phoenix::motorcontrol::can;
using namespace ctre::phoenix::motorcontrol;

class motorSubscriber : public rclcpp::Node {
public:
  motorSubscriber() : Node("motorSubscriber") {
    auto topic_callback = [this](sensor_msgs::msg::Joy msg) -> void {
      // callback code here
      RCLCPP_INFO(this->get_logger(), "reading a value of %f", msg.axes[0]);
    };
    subscription_ = this->create_subscription<sensor_msgs::msg::Joy>(
        "joy", 10, topic_callback);
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;
};

int main(int argc, char **argv) {

  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<motorSubscriber>());

  // right hip front motor, right hip back motor, right hip angle motor, etc
  TalonSRX R_F{0, "can0"};
  // TalonSRX R_B{1, "can0"};
  // TalonSRX R_A{2, "can0"};
  // TalonSRX L_F{3, "can0"};
  // TalonSRX L_B{4, "can0"};
  // TalonSRX L_A{5, "can0"};

  R_F.ConfigFactoryDefault(100);

  while (true) {

    ctre::phoenix::unmanaged::Unmanaged::FeedEnable(
        100); // allow control for another 100ms

    R_F.Set(ControlMode::PercentOutput, -0.25);

    std::cout << "Voltage: " << R_F.GetMotorOutputVoltage() << "V"
              << " Current: " << R_F.GetStatorCurrent() << "A" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }

  rclcpp::shutdown();
  return 0;
}
