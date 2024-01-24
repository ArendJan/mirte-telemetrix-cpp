#include "mirte-actuators.hpp"

Mirte_Actuators::Mirte_Actuators(node_handle nh,
                                 std::shared_ptr<TMX> tmx,
                                 std::shared_ptr<Mirte_Board> board,
                                 std::shared_ptr<Parser> parser) {
  this->tmx = tmx;
  this->nh = nh;
  this->board = board;
  this->actuators = Motor::get_motors(nh, tmx, board, parser);
  auto servos = Servo_data::parse_servo_data(parser, board);
  auto motors = Motor_data::parse_motor_data(parser, board);
}

Mirte_Actuator::Mirte_Actuator(node_handle nh,
                               std::shared_ptr<TMX> tmx,
                               std::shared_ptr<Mirte_Board> board,
                               std::vector<pin_t> pins, std::string name) {
  this->tmx = tmx;
  this->nh = nh;
  this->board = board;
  this->pins = pins;
  this->name = name;
}
std::vector<std::shared_ptr<Mirte_Actuator>>
Motor::get_motors(node_handle nh, std::shared_ptr<TMX> tmx,
                  std::shared_ptr<Mirte_Board> board,
                  std::shared_ptr<Parser> parser) {
  std::vector<std::shared_ptr<Mirte_Actuator>> motors;
  auto motor_datas = Motor_data::parse_motor_data(parser, board);
  for (auto motor_data : motor_datas) {
    if (motor_data->check()) {
      if (motor_data->type == Motor_data::Motor_type::PP) {
        motors.push_back(std::make_shared<PPMotor>(nh, tmx, board, motor_data,
                                                   motor_data->name));
      } else if (motor_data->type == Motor_data::Motor_type::DP) {
        motors.push_back(std::make_shared<DPMotor>(nh, tmx, board, motor_data,
                                                   motor_data->name));
      } else if (motor_data->type == Motor_data::Motor_type::DDP) {
        // motors.push_back(std::make_shared<DDPMotor>(
        //     nh, tmx, board, motor_data,
        //     motor_data->name));
      }
    }
  }
  return motors;
}
bool Motor::service_callback(
    const std::shared_ptr<mirte_msgs_set_motor_speed::Request> req,
    std::shared_ptr<mirte_msgs_set_motor_speed::Response> res) {
  this->set_speed(req->speed);
  res->status = true;
  return true;
}

void Motor::motor_callback(const std_msgs_int32 &msg) {
  this->set_speed(msg.data);
}

Motor::Motor(node_handle nh, std::shared_ptr<TMX> tmx,
             std::shared_ptr<Mirte_Board> board,
             std::shared_ptr<Motor_data> motor_data, std::string name,
             std::vector<pin_t> pins)
    : Mirte_Actuator(nh, tmx, board, pins, name) {
  motor_service = nh->create_service<mirte_msgs_set_motor_speed>(
      "/mirte/set_" + this->name + "_speed",
      std::bind(&Motor::service_callback, this, std::placeholders::_1,
                std::placeholders::_2));

  ros_client = nh->create_subscription<std_msgs_int32>(
      "/mirte/motor_" + this->name + "_speed", 1000,
      std::bind(&Motor::motor_callback, this, std::placeholders::_1));
  this->motor_data = motor_data;
  this->max_pwm = board->get_max_pwm();
}

DPMotor::DPMotor(node_handle nh, std::shared_ptr<TMX> tmx,
                 std::shared_ptr<Mirte_Board> board,
                 std::shared_ptr<Motor_data> motor_data, std::string name)
    : Motor(nh, tmx, board, motor_data, name,
            {motor_data->D1, motor_data->P1}) {
  this->pwm_pin = motor_data->P1;
  this->dir_pin = motor_data->D1;
  tmx->setPinMode(this->pwm_pin, TMX::PIN_MODES::PWM_OUTPUT);
  tmx->setPinMode(this->dir_pin, TMX::PIN_MODES::DIGITAL_OUTPUT);
  this->set_speed(0);
  // motor_service = nh.advertiseService(name, &Motor::service_callback, this);
  // ros_client = nh.subscribe<mirte_msgs::SetMotorSpeed>(name, 1000,
  //                                                      &Motor::motor_callback,
  //                                                      this);
}

void DPMotor::set_speed(int speed) {
  int32_t speed_ = (int32_t)((float)speed * (this->max_pwm) / 100.0);
  tmx->pwmWrite(this->pwm_pin, speed > 0 ? speed_ : -speed_);
  std::cout << "1:" << std::dec << speed << std::endl;
  tmx->digitalWrite(this->dir_pin, speed > 0 ? 1 : 0);
  std::cout << "2:" << std::dec << (speed > 0 ? 1 : 0) << std::endl;
  std::cout << "Setting speed to " << std::dec << speed << std::endl;
  // tmx->set_digital_pwm(pins[0], speed);
}

PPMotor::PPMotor(node_handle nh, std::shared_ptr<TMX> tmx,
                 std::shared_ptr<Mirte_Board> board,
                 std::shared_ptr<Motor_data> motor_data, std::string name)
    : Motor(nh, tmx, board, motor_data, name,
            {motor_data->P1, motor_data->P2}) {
  this->pwmA_pin = motor_data->P1;
  this->pwmB_pin = motor_data->P2;
  std::cout << "PPMotor" << std::hex << this->pwmA_pin << " " << std::hex
            << this->pwmB_pin << std::endl;
  tmx->setPinMode(this->pwmA_pin, TMX::PIN_MODES::PWM_OUTPUT);
  tmx->setPinMode(this->pwmB_pin, TMX::PIN_MODES::PWM_OUTPUT);
}
void PPMotor::set_speed(int speed) {
  int32_t speed_ = (int32_t)((float)speed * (this->max_pwm) / 100.0);

  tmx->pwmWrite(this->pwmA_pin, speed > 0 ? speed_ : 0);
  tmx->pwmWrite(this->pwmB_pin, speed < 0 ? -speed_ : 0);
  std::cout << "1:" << std::dec << (speed < 0 ? -speed_ : 0) << std::endl;
  std::cout << "2:" << std::dec << (speed > 0 ? speed_ : 0) << std::endl;
  std::cout << "Setting speed to " << std::dec << speed << std::endl;

  std::cout << "PP Setting speed to " << std::dec << speed << std::endl;
  // tmx->set_pwm(pins[0], speed);
}
