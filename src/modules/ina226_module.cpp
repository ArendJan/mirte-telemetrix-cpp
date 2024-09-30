#include <stdint.h>
#include <rcl/context.h>
#include <stddef.h>
#include <boost/iterator/iterator_facade.hpp>
#include <rclcpp/duration.hpp>
#include <rclcpp/node.hpp>
#include <rclcpp/publisher.hpp>
#include <rclcpp/qos_event.hpp>
#include <rclcpp/service.hpp>
#include <rclcpp/time.hpp>
#include <sensor_msgs/msg/detail/battery_state__struct.hpp>
#include <std_msgs/msg/detail/int32__struct.hpp>
#include <std_srvs/srv/detail/set_bool__struct.hpp>
#include <functional>
#include <array>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "mirte_telemetrix_cpp/modules/base_module.hpp"
#include "mirte_telemetrix_cpp/node_data.hpp"
#include "mirte_telemetrix_cpp/parsers/modules/ina226_data.hpp"
#include "mirte_telemetrix_cpp/parsers/modules/module_data.hpp"
#include "mirte_telemetrix_cpp/parsers/parsers.hpp"
#include "mirte_telemetrix_cpp/util.hpp"
#include "rclcpp/node.hpp"
#include "tmx_cpp/sensors.hpp"
#include "tmx_cpp/sensors/INA226.hpp"
#include "tmx_cpp/tmx.hpp"

#ifdef WITH_GPIO
#include <chrono>

using namespace std::chrono_literals;
#endif

#include <mirte_telemetrix_cpp/modules/ina226_module.hpp>

using namespace std::placeholders;  // for _1, _2, _3...

INA226_sensor::INA226_sensor(
  NodeData node_data, INA226Data ina_data, std::shared_ptr<tmx_cpp::Sensors> modules)
: Mirte_module(node_data, {ina_data.scl, ina_data.sda}, (ModuleData)ina_data), data(ina_data)
{
  tmx->setI2CPins(ina_data.sda, ina_data.scl, ina_data.port);

  this->used_time = nh->now();
  this->total_used_mAh = 0;

  this->ina226 = std::make_shared<tmx_cpp::INA226_module>(
    ina_data.port, ina_data.addr, std::bind(&INA226_sensor::data_cb, this, _1, _2));
  this->battery_pub =
    nh->create_publisher<sensor_msgs::msg::BatteryState>("power/" + this->name, 1);

  this->used_pub = nh->create_publisher<std_msgs::msg::Int32>("power/" + this->name + "/used", 1);

  this->shutdown_service = nh->create_service<std_srvs::srv::SetBool>(
    "shutdown", std::bind(&INA226_sensor::shutdown_robot_cb, this, _1, _2));

  modules->add_sens(this->ina226);
  // TODO: add used topic
  // TODO: add shutdown service
  // TODO: add auto shutdown

#ifdef WITH_GPIO  // LED Battery indicator
  if (this->data.use_percentage_led)
  {
    battery_led_timer = nh->create_wall_timer(0.5s, std::bind(&INA226_sensor::battery_led_timer_callback, this));
  }
#endif
}

void INA226_sensor::data_cb(float voltage, float current)
{
  voltage_ = voltage;
  // std::cout << "INA226 data: " << current << " " << voltage << std::endl;
  auto msg = sensor_msgs::msg::BatteryState();
  msg.header = get_header();

  msg.voltage = voltage;
  msg.current = current;
  msg.percentage = calc_soc(voltage);
  this->battery_pub->publish(msg);
  this->integrate_usage(current);
  this->check_soc(voltage, current);
}

float INA226_sensor::calc_soc(float voltage)
{
  const auto CELL_COUNT = 3;
  voltage = voltage / CELL_COUNT;
  std::vector<std::pair<float, float>> soc_levels{
    // single cell voltages vs percentage
    {0, 0},       {3.27, 0.00}, {3.61, 0.05}, {3.69, 0.10}, {3.71, 0.15}, {3.73, 0.20},
    {3.75, 0.25}, {3.77, 0.30}, {3.79, 0.35}, {3.80, 0.40}, {3.82, 0.45}, {3.84, 0.50},
    {3.85, 0.55}, {3.87, 0.60}, {3.91, 0.65}, {3.95, 0.70}, {3.98, 0.75}, {4.02, 0.80},
    {4.08, 0.85}, {4.11, 0.90}, {4.15, 0.95}, {4.20, 1.00}, {10, 1.00}};
  for (size_t i = 1; i < soc_levels.size(); i++) {
    if (voltage < soc_levels[i].first) {  // find the first voltage level that is
                                          // higher than the current voltage
      return soc_levels[i - 1].second;    // return the soc level of the previous voltage level
    }
  }
  return 1.00;
}

void INA226_sensor::integrate_usage(float current)
{
  auto current_time = this->nh->now();
  auto duration = current_time - this->used_time;

  auto used_mA_sec = duration.seconds() * current * 1000.0;  // milliAmpere seconds
  auto used_mAh = used_mA_sec / 3600;                        // convert to mAh

  this->total_used_mAh += used_mAh;
  this->used_time = current_time;
  std_msgs::msg::Int32 msg;
  msg.data = (int32_t)this->total_used_mAh;
  this->used_pub->publish(msg);
}

void INA226_sensor::check_soc(float voltage, float current)
{
  if (!this->enable_turn_off) {
    if (voltage > 6 && current > 0.1) {
      this->enable_turn_off = true;
      this->in_power_dip = false;
      std::cout << "Enabling turn off" << std::endl;
    }
    return;
  }
  //  at start dip of too low voltage, start timer, when longer than 5s below
  //  trigger voltage, then shut down
  // this makes sure that a short dip (motor start) does not trigger it

  if (voltage != 0.1 && voltage < this->data.min_voltage) {
    if (!this->in_power_dip) {
      this->turn_off_trigger_time = this->nh->now();
      this->in_power_dip = true;
      std::cout << "Triggering turn off in " << this->data.power_low_time << "s" << std::endl;
    }
  } else {
    this->in_power_dip = false;
    this->turn_off_trigger_time = rclcpp::Time(0, 0);
  }

  if (this->in_power_dip) {
    auto current_time = this->nh->now();
    auto duration = current_time - this->turn_off_trigger_time;
    std::cout << "Triggering turn off maybe" << duration.seconds() << std::endl;
    std::cout << "you have " << (this->data.power_low_time - duration.seconds()) << "s left"
              << std::endl;
    if (duration.seconds() > this->data.power_low_time) {
      std::cout << "Turning off" << std::endl;
      // this->tmx->shutdown();
      this->shutdown_robot();
    }
  }
}

void INA226_sensor::shutdown_robot()
{
  if (this->shutdown_triggered) {
    return;
  }
  this->shutdown_triggered = true;
  std::cout << "Shutting down robot" << std::endl;
  // run shutdown command
  exec("sudo bash -c \"wall 'Shutting down.'\"");  // TODO: check if this works
                                                   // with sudo on a mirte
  exec("sudo shutdown now");
}

void INA226_sensor::shutdown_robot_cb(
  const std::shared_ptr<std_srvs::srv::SetBool::Request> req,
  std::shared_ptr<std_srvs::srv::SetBool::Response> res)
{
  if (req->data) {
    this->shutdown_robot();
  }
  res->success = true;
  res->message = "Shutting down";
}

std::vector<std::shared_ptr<INA226_sensor>> INA226_sensor::get_ina_modules(
  NodeData node_data, std::shared_ptr<Parser> parser, std::shared_ptr<tmx_cpp::Sensors> modules)
{
  std::vector<std::shared_ptr<INA226_sensor>> pca_modules;
  auto pca_data = parse_all_modules<INA226Data>(parser, node_data.board);
  for (auto pca : pca_data) {
    std::cout << "ina data" << pca.name << std::endl;
    auto pca_module = std::make_shared<INA226_sensor>(node_data, pca, modules);
    pca_modules.push_back(pca_module);
  }
  return pca_modules;
}

#ifdef WITH_GPIO
void INA226_sensor::battery_led_timer_callback()
{
  // show the SOC by blinking the led. Shorter pulse -> lower SOC
  // cycle time of 5s
  auto now = std::chrono::system_clock::now().time_since_epoch() / 1ms;
  auto time_msec = now % 5000;

  auto percentage = calc_soc(voltage_) * 5000;
  if (time_msec > percentage) {
    //turn off the led
    data.percentage_led_pin->write(0);
  } else {
    //turn on the led
    data.percentage_led_pin->write(1);
  }
}
#endif