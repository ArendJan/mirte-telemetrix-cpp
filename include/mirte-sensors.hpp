#pragma once
#include "mirte-board.hpp"
#include "parsers/sensors.hpp"
#include "ros.hpp"
#include <tmx.hpp>
class Mirte_Sensor;
class Mirte_Sensors {
public:
  Mirte_Sensors(node_handle nh, std::shared_ptr<TMX> tmx,
                std::shared_ptr<Mirte_Board> board,
                std::shared_ptr<Parser> parser);
  std::shared_ptr<TMX> tmx;
  node_handle nh;
  std::shared_ptr<Mirte_Board> board;
  std::vector<std::shared_ptr<Mirte_Sensor>> sensors;
  ros_timer timer;
  void publish();
  void stop();

  service<mirte_msgs_get_pin_value> pin_service;
  enum class PIN_USE { DIGITAL_IN, DIGITAL_OUT, ANALOG_IN, ANALOG_OUT, UNUSED };
  std::map<pin_t, std::tuple<PIN_USE, int, bool, bool>>
      pin_map; // pin -> (is_digital, value,analog_cb, digital_cb )
  bool
  pin_callback(const std::shared_ptr<mirte_msgs_get_pin_value::Request> req,
               std::shared_ptr<mirte_msgs_get_pin_value::Response> res);
};

class Mirte_Sensor {
public:
  std::shared_ptr<TMX> tmx;
  node_handle nh;
  std::shared_ptr<Mirte_Board> board;
  std::vector<uint8_t> pins;
  virtual void publish() = 0;
  std::string name;
  virtual void stop() {}
  auto get_header() {
    std_msgs_header header;
    // header.stamp = nh->now();

    return header;
  }
  Mirte_Sensor(node_handle nh, std::shared_ptr<TMX> tmx,
               std::shared_ptr<Mirte_Board> board, std::vector<uint8_t> pins,
               std::string name);
};

class KeypadMonitor : public Mirte_Sensor {
public:
  KeypadMonitor(node_handle nh, std::shared_ptr<TMX> tmx,
                std::shared_ptr<Mirte_Board> board,
                std::shared_ptr<Keypad_data> keypad_data);
  void publish();
  std::shared_ptr<Keypad_data> keypad_data;
  publisher<mirte_msgs_keypad> keypad_pub;
  // publisher<mirte_msgs_keypad> keypad_pub;
  publisher<mirte_msgs_keypad> keypad_pressed_pub;
  service<mirte_msgs_get_keypad> keypad_service; 
  bool keypad_callback(
      const std::shared_ptr<mirte_msgs::srv::GetKeypad::Request> req,
      std::shared_ptr<mirte_msgs::srv::GetKeypad::Response> res);
  void callback(uint16_t value);
  uint16_t value;
  std::string last_key;
  double last_debounce_time = nh->now().seconds();
  std::string last_debounced_key;

  static std::vector<std::shared_ptr<KeypadMonitor>>
  get_keypad_monitors(node_handle nh, std::shared_ptr<TMX> tmx,
                      std::shared_ptr<Mirte_Board> board,
                      std::shared_ptr<Parser> parser);
};

class SonarMonitor : public Mirte_Sensor {
public:
  SonarMonitor(node_handle nh, std::shared_ptr<TMX> tmx,
               std::shared_ptr<Mirte_Board> board,
               std::shared_ptr<Sonar_data> sonar_data);
  void publish();
  std::shared_ptr<Sonar_data> sonar_data;
  publisher<sensor_msgs_range> sonar_pub;
  static std::vector<std::shared_ptr<SonarMonitor>>
  get_sonar_monitors(node_handle nh, std::shared_ptr<TMX> tmx,
                     std::shared_ptr<Mirte_Board> board,
                     std::shared_ptr<Parser> parser);
  void callback(uint16_t value);
  uint16_t value;
  service<mirte_msgs_get_distance> sonar_service;
  bool
  service_callback(const std::shared_ptr<mirte_msgs_get_distance::Request> req,
                   std::shared_ptr<mirte_msgs_get_distance::Response> res);
};

class IntensityMonitor : public Mirte_Sensor {
public:
  IntensityMonitor(node_handle nh, std::shared_ptr<TMX> tmx,
                   std::shared_ptr<Mirte_Board> board, std::vector<pin_t> pins,
                   std::string name)
      : Mirte_Sensor(nh, tmx, board, pins, name) {}
  virtual void publish() = 0;
  static std::vector<std::shared_ptr<IntensityMonitor>>
  get_intensity_monitors(node_handle nh, std::shared_ptr<TMX> tmx,
                         std::shared_ptr<Mirte_Board> board,
                         std::shared_ptr<Parser> parser);
  ~IntensityMonitor() {}
  std::shared_ptr<Intensity_data> intensity_data;
};

class Digital_IntensityMonitor : public IntensityMonitor {
public:
  Digital_IntensityMonitor(node_handle nh, std::shared_ptr<TMX> tmx,
                           std::shared_ptr<Mirte_Board> board,
                           std::shared_ptr<Intensity_data> intensity_data);
  void publish();
  void callback(uint16_t value);
  bool value;
  publisher<mirte_msgs_intensity_digital> intensity_pub;

  service<mirte_msgs_get_intensity_digital> intensity_service;
  bool service_callback(
      const std::shared_ptr<mirte_msgs_get_intensity_digital::Request> req,
      std::shared_ptr<mirte_msgs_get_intensity_digital::Response> res);
};

class Analog_IntensityMonitor : public IntensityMonitor {
public:
  Analog_IntensityMonitor(node_handle nh, std::shared_ptr<TMX> tmx,
                          std::shared_ptr<Mirte_Board> board,
                          std::shared_ptr<Intensity_data> intensity_data);
  void publish();
  void callback(uint16_t value);
  uint16_t value;
  publisher<mirte_msgs_intensity> intensity_pub;
  service<mirte_msgs_get_intensity> intensity_service;
  bool
  service_callback(const std::shared_ptr<mirte_msgs_get_intensity::Request> req,
                   std::shared_ptr<mirte_msgs_get_intensity::Response> res);
};