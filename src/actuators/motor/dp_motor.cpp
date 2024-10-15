#include <mirte_telemetrix_cpp/actuators/motor/dp_motor.hpp>

DPMotor::DPMotor(NodeData node_data, MotorData motor_data)
: Motor(node_data, {motor_data.D1, motor_data.P1}, motor_data)
{
  this->pwm_pin = motor_data.P1;
  this->dir_pin = motor_data.D1;
  tmx->setPinMode(this->pwm_pin, tmx_cpp::TMX::PIN_MODES::PWM_OUTPUT);
  tmx->setPinMode(this->dir_pin, tmx_cpp::TMX::PIN_MODES::DIGITAL_OUTPUT);
  // motor_service = nh.advertiseService(name, &Motor::service_callback, this);
  // ros_client = nh.subscribe<mirte_msgs::SetMotorSpeed>(name, 1000,
  //                                                      &Motor::motor_callback,
  //                                                      this);
}

void DPMotor::set_speed(int speed)
{
  int32_t speed_ = (int32_t)((float)speed * (this->max_pwm) / 100.0);
  tmx->pwmWrite(this->pwm_pin, speed > 0 ? speed_ : -speed_);
  std::cout << "1:" << std::dec << speed << std::endl;
  tmx->digitalWrite(this->dir_pin, speed > 0 ? 1 : 0);
  std::cout << "2:" << std::dec << (speed > 0 ? 1 : 0) << std::endl;
  std::cout << "Setting speed to " << std::dec << speed << std::endl;
  // tmx->set_digital_pwm(pins[0], speed);
}