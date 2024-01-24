#pragma once
#include "parsers/actuators.hpp"
#include "ros.hpp"
#include <memory>
#include <mirte-board.hpp>
#include <string>
#include <tmx.hpp>
#include <vector>
class Mirte_Actuator;
class Mirte_Actuators {
public:
  Mirte_Actuators(node_handle nh, std::shared_ptr<TMX> tmx,
                  std::shared_ptr<Mirte_Board> board,
                  std::shared_ptr<Parser> parser);
  std::shared_ptr<TMX> tmx;
  node_handle nh;
  std::shared_ptr<Mirte_Board> board;
  std::vector<std::shared_ptr<Mirte_Actuator>> actuators;
};

class Mirte_Actuator {
public:
  std::shared_ptr<TMX> tmx;
  node_handle nh;
  std::shared_ptr<Mirte_Board> board;
  std::vector<pin_t> pins;
  std::string name;
  auto get_header() {
    std_msgs_header header;
    // nh->
    // header.stamp = nh->now();

    return header;
  }
  Mirte_Actuator(node_handle nh, std::shared_ptr<TMX> tmx,
                 std::shared_ptr<Mirte_Board> board, std::vector<pin_t> pins,
                 std::string name);
};

class Motor : public Mirte_Actuator {
public:
  Motor(node_handle nh, std::shared_ptr<TMX> tmx,
        std::shared_ptr<Mirte_Board> board,
        std::shared_ptr<Motor_data> motor_data, std::string name,
        std::vector<pin_t> pins);
  service<mirte_msgs_set_motor_speed> motor_service;
  bool service_callback(
      const std::shared_ptr<mirte_msgs_set_motor_speed::Request> req,
      std::shared_ptr<mirte_msgs_set_motor_speed::Response> res);
  void motor_callback(const std_msgs_int32 &msg);
  int last_speed = 0;
  subscriber<std_msgs_int32> ros_client;
  static std::vector<std::shared_ptr<Mirte_Actuator>>
  get_motors(node_handle nh, std::shared_ptr<TMX> tmx,
             std::shared_ptr<Mirte_Board> board,
             std::shared_ptr<Parser> parser);
  virtual void set_speed(int speed) = 0;
  std::shared_ptr<Motor_data> motor_data;
  int max_pwm;
};

class PPMotor : public Motor {
public:
  PPMotor(node_handle nh, std::shared_ptr<TMX> tmx,
          std::shared_ptr<Mirte_Board> board,
          std::shared_ptr<Motor_data> motor_data, std::string name);
  void set_speed(int speed);
  pin_t pwmA_pin;
  pin_t pwmB_pin;
};

class DPMotor : public Motor {
public:
  DPMotor(node_handle nh, std::shared_ptr<TMX> tmx,
          std::shared_ptr<Mirte_Board> board,
          std::shared_ptr<Motor_data> motor_data, std::string name);
  void set_speed(int speed);
  pin_t dir_pin;
  pin_t pwm_pin;
};

class DDPMotor : public Motor {
public:
  DDPMotor(node_handle nh, std::shared_ptr<TMX> tmx,
           std::shared_ptr<Mirte_Board> board,
           std::shared_ptr<Motor_data> motor_data, std::string name);
  void set_speed(int speed);
  pin_t A_pin;
  pin_t B_pin;
  pin_t pwm_pin;
};
