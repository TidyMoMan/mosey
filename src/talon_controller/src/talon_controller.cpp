#define Phoenix_No_WPI //this ain't yo mama's frc team bud, we are doing serious shiz here man c'mon get it together
#include "ctre/Phoenix.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <string>


#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

#include "ctre/phoenix/unmanaged/Unmanaged.h" //needed for watchdog enable

using namespace ctre::phoenix::motorcontrol::can;
using namespace ctre::phoenix::motorcontrol;

class subscriber : public rclcpp::Node{
  public: 
  subscriber():Node("motor_listener"){
    subscription_ = this->create_subscription<std_msgs::sensor_msgs::Joy>("joy", 10, std::bind(&subscriber::topic_callback, this, _1));
  }

  private:
  void topic_callback(const std_msgs::sensor_msgs::Joy){
    
  }
}

int main() {
	
	//right hip front motor, right hip back motor, right hip angle motor, etc
	TalonSRX R_F{0, "can0"}; 
	//TalonSRX R_B{1, "can0"}; 
	//TalonSRX R_A{2, "can0"};
	//TalonSRX L_F{3, "can0"}; 
	//TalonSRX L_B{4, "can0"}; 
	//TalonSRX L_A{5, "can0"}; 

	R_F.ConfigFactoryDefault(100);

	while (true) {
        	
	ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100); //allow control for another 100ms

	R_F.Set(ControlMode::PercentOutput, -0.25);
            
        std::cout << "Voltage: " << R_F.GetMotorOutputVoltage() << "V" << " Current: " << R_F.GetStatorCurrent() << "A" << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    return 0;
}

