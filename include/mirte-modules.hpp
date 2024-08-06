#pragma once
#include "mirte_msgs/msg/servo_position.hpp"
#include "mirte_msgs/srv/get_servo_range.hpp"
#include "mirte_msgs/srv/set_motor_speed.hpp"
#include "mirte_msgs/srv/set_servo_angle.hpp"
#include "parsers/p_modules.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/header.hpp"
#include "std_msgs/msg/int32.hpp"
#include "std_srvs/srv/set_bool.hpp"
#include <memory>
#include <mirte-board.hpp>
#include <modules/PCA9685.hpp>
#include <string>
#include <tmx.hpp>
#include <vector>

// #include <mirte-actuators.hpp>
class Mirte_module;
class Mirte_modules {
public:
  Mirte_modules(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
                std::shared_ptr<Mirte_Board> board,
                std::shared_ptr<Parser> parser);
  std::shared_ptr<TMX> tmx;
  std::shared_ptr<rclcpp::Node> nh;
  std::shared_ptr<Mirte_Board> board;
  std::vector<std::shared_ptr<Mirte_module>> modules;
  std::shared_ptr<Modules> module_sys;
  std::shared_ptr<Sensors> sensor_sys;
};

class Mirte_module {
public:
  std::shared_ptr<TMX> tmx;
  std::shared_ptr<rclcpp::Node> nh;
  std::shared_ptr<Mirte_Board> board;
  std::string name;

  auto get_header() {
    std_msgs::msg::Header header;
    header.stamp = nh->now();

    return header;
  }
  Mirte_module(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
               std::shared_ptr<Mirte_Board> board, std::string name);
};
class PCA_Motor;
class PCA_Module : public Mirte_module {
public:
  PCA_Module(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
             std::shared_ptr<Mirte_Board> board, std::string name,
             std::shared_ptr<Modules> modules,
             std::shared_ptr<PCA_data> pca_data);
  std::shared_ptr<PCA9685_module> pca9685;
  std::vector<std::shared_ptr<PCA_Motor>> motors;
  //   std::vector<std::shared_ptr<PCA_Servo>> servos;
  static std::vector<std::shared_ptr<PCA_Module>>
  get_pca_modules(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
                  std::shared_ptr<Mirte_Board> board,
                  std::shared_ptr<Parser> parser,
                  std::shared_ptr<Modules> modules);
};

class PCA_Motor {
public:
  PCA_Motor(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
            std::shared_ptr<Mirte_Board> board,
            std::shared_ptr<PCA_Motor_data> motor_data,
            std::shared_ptr<PCA9685_module> pca9685);
  std::shared_ptr<PCA_Motor_data> motor_data;
  std::shared_ptr<PCA9685_module> pca9685_mod;
  // Stolen from mirtes-actuators.hpp::motor, but it was too shit to inherit
  // from that one as well.
  void set_speed(int speed);
  rclcpp::Service<mirte_msgs::srv::SetMotorSpeed>::SharedPtr motor_service;
  bool service_callback(
      const std::shared_ptr<mirte_msgs::srv::SetMotorSpeed::Request> req,
      std::shared_ptr<mirte_msgs::srv::SetMotorSpeed::Response> res);
  void motor_callback(const std_msgs::msg::Int32 &msg);
  int last_speed = 0;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr ros_client;
};

class Hiwonder_servo;
class Hiwonder_bus_module : public Mirte_module {
public:
  Hiwonder_bus_module(std::shared_ptr<rclcpp::Node> nh,
                      std::shared_ptr<TMX> tmx,
                      std::shared_ptr<Mirte_Board> board, std::string name,
                      std::shared_ptr<Modules> modules,
                      std::shared_ptr<Hiwonder_bus_data> bus_data);
  std::shared_ptr<HiwonderServo_module> bus;
  std::vector<std::shared_ptr<Hiwonder_servo>> servos;
  //   std::vector<std::shared_ptr<PCA_Servo>> servos;

  std::shared_ptr<Hiwonder_bus_data> bus_data;
  static std::vector<std::shared_ptr<Hiwonder_bus_module>>

  get_hiwonder_modules(std::shared_ptr<rclcpp::Node> nh,
                       std::shared_ptr<TMX> tmx,
                       std::shared_ptr<Mirte_Board> board,
                       std::shared_ptr<Parser> parser,
                       std::shared_ptr<Modules> modules);

  void position_cb(std::vector<HiwonderServo_module::Servo_pos>);
  void verify_cb(int, bool);
  void range_cb(int, uint16_t, uint16_t);
  void offset_cb(int, uint16_t);

  // ROS:
  rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr enable_service;
  bool enable_cb(const std::shared_ptr<std_srvs::srv::SetBool::Request> req,
                 std::shared_ptr<std_srvs::srv::SetBool::Response> res);
};

class Hiwonder_servo {
public:
  Hiwonder_servo(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
                 std::shared_ptr<Mirte_Board> board,
                 std::shared_ptr<Hiwonder_servo_data> servo_data,
                 std::shared_ptr<HiwonderServo_module> bus);
  std::shared_ptr<Hiwonder_servo_data> servo_data;
  std::shared_ptr<HiwonderServo_module> bus_mod;

  // callbacks from the pico
  void position_cb(HiwonderServo_module::Servo_pos &pos);

  // ROS:
  //  set_x_servo_enable
  rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr enable_service;
  bool enable_cb(const std::shared_ptr<std_srvs::srv::SetBool::Request> req,
                 std::shared_ptr<std_srvs::srv::SetBool::Response> res);

  // set_x_servo_angle
  rclcpp::Service<mirte_msgs::srv::SetServoAngle>::SharedPtr angle_service;
  bool
  angle_cb(const std::shared_ptr<mirte_msgs::srv::SetServoAngle::Request> req,
           std::shared_ptr<mirte_msgs::srv::SetServoAngle::Response> res);

  // get_x_servo_range
  rclcpp::Service<mirte_msgs::srv::GetServoRange>::SharedPtr range_service;
  bool
  range_cb(const std::shared_ptr<mirte_msgs::srv::GetServoRange::Request> req,
           std::shared_ptr<mirte_msgs::srv::GetServoRange::Response> res);

  // /servos/x/position publisher
  rclcpp::Publisher<mirte_msgs::msg::ServoPosition>::SharedPtr position_pub;

  // angle calcs
  uint16_t calc_angle_out(float angle_in);
  float calc_angle_in(uint16_t angle_out);
};

#include "sensor_msgs/msg/battery_state.hpp"
#include "sensors/INA226.hpp"
class INA226_sensor : public Mirte_module {
public:
  INA226_sensor(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
                std::shared_ptr<Mirte_Board> board, std::string name,
                std::shared_ptr<Sensors> modules,
                std::shared_ptr<INA226_data> ina_data);
  std::shared_ptr<INA226_data> ina_data;
  std::shared_ptr<INA226_module> ina226;
  rclcpp::Publisher<sensor_msgs::msg::BatteryState>::SharedPtr battery_pub;
  void data_cb(float voltage, float current);
  void publish();
  static std::vector<std::shared_ptr<INA226_sensor>>
  get_ina_modules(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
                  std::shared_ptr<Mirte_Board> board,
                  std::shared_ptr<Parser> parser,
                  std::shared_ptr<Sensors> sensors);
};