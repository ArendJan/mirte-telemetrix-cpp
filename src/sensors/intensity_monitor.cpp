#include <mirte_telemetrix_cpp/sensors/intensity_monitor.hpp>
#include <rcl/context.h>
#include <mirte_msgs/msg/detail/intensity__struct.hpp>
#include <mirte_msgs/msg/detail/intensity_digital__struct.hpp>
#include <mirte_msgs/srv/detail/get_intensity__struct.hpp>
#include <mirte_msgs/srv/detail/get_intensity_digital__struct.hpp>
#include <rclcpp/node.hpp>
#include <rclcpp/publisher.hpp>
#include <rclcpp/qos_event.hpp>
#include <rclcpp/service.hpp>
#include <memory>
#include <vector>
#include <array>
#include <cstdint>
#include <functional>
#include <set>
#include <string>
#include <variant>

#include "mirte_telemetrix_cpp/node_data.hpp"
#include "mirte_telemetrix_cpp/parsers/device_data.hpp"
#include "mirte_telemetrix_cpp/parsers/parsers.hpp"
#include "mirte_telemetrix_cpp/parsers/sensors/base_sensor_data.hpp"
#include "mirte_telemetrix_cpp/parsers/sensors/intensity_data.hpp"
#include "mirte_telemetrix_cpp/sensors/base_sensor.hpp"
#include "rclcpp/node.hpp"
#include "tmx_cpp/tmx.hpp"

IntensityMonitor::IntensityMonitor(
  NodeData node_data, std::vector<pin_t> pins, IntensityData intensity_data)
: Mirte_Sensor(node_data, pins, (SensorData)intensity_data), intensity_data(intensity_data)
{
}

std::vector<std::shared_ptr<IntensityMonitor>> IntensityMonitor::get_intensity_monitors(
  NodeData node_data, std::shared_ptr<Parser> parser)
{
  std::vector<std::shared_ptr<IntensityMonitor>> sensors;
  auto ir_sensors = parse_all<IntensityData>(parser, node_data.board);
  for (auto ir_data : ir_sensors) {
    if (ir_data.a_pin != (pin_t)-1) {
      sensors.push_back(std::make_shared<AnalogIntensityMonitor>(node_data, ir_data));
      // std::cout << "Add Analog Intensity: " << ir_data.name << std::endl;
    }
    if (ir_data.d_pin != (pin_t)-1) {
      sensors.push_back(std::make_shared<DigitalIntensityMonitor>(node_data, ir_data));
      // std::cout << "Add Digital Intensity: " << ir_data.name << std::endl;
    }
  }
  return sensors;
}

void DigitalIntensityMonitor::callback(uint16_t value)
{
  this->value = value;
  this->update();
}

void DigitalIntensityMonitor::update()
{
  mirte_msgs::msg::IntensityDigital msg;

  msg.header = get_header();
  msg.value = value;

  intensity_pub->publish(msg);
}

DigitalIntensityMonitor::DigitalIntensityMonitor(NodeData node_data, IntensityData intensity_data)
: IntensityMonitor(node_data, {intensity_data.d_pin}, intensity_data)
{
  intensity_pub = nh->create_publisher<mirte_msgs::msg::IntensityDigital>(
    "intensity/" + intensity_data.name + "_digital", 1);

  intensity_service = nh->create_service<mirte_msgs::srv::GetIntensityDigital>(
    "get_intensity_" + intensity_data.name + "_digital",
    std::bind(
      &DigitalIntensityMonitor::service_callback, this, std::placeholders::_1,
      std::placeholders::_2));

  tmx->setPinMode(intensity_data.d_pin, tmx_cpp::TMX::PIN_MODES::DIGITAL_INPUT, true, 0);
  tmx->add_digital_callback(
    intensity_data.d_pin, [this](auto pin, auto value) { this->callback(value); });
}

AnalogIntensityMonitor::AnalogIntensityMonitor(NodeData node_data, IntensityData intensity_data)
: IntensityMonitor(node_data, {intensity_data.a_pin}, intensity_data)
{
  intensity_pub =
    nh->create_publisher<mirte_msgs::msg::Intensity>("intensity/" + intensity_data.name, 1);

  intensity_service = nh->create_service<mirte_msgs::srv::GetIntensity>(
    "get_intensity_" + intensity_data.name, std::bind(
                                              &AnalogIntensityMonitor::service_callback, this,
                                              std::placeholders::_1, std::placeholders::_2));

  tmx->setPinMode(intensity_data.a_pin, tmx_cpp::TMX::PIN_MODES::ANALOG_INPUT, true, 0);
  tmx->add_analog_callback(
    intensity_data.a_pin, [this](auto pin, auto value) { this->callback(value); });
}

void AnalogIntensityMonitor::callback(uint16_t value)
{
  this->value = value;
  this->update();
}

void AnalogIntensityMonitor::update()
{
  mirte_msgs::msg::Intensity msg;

  msg.header = get_header();
  msg.value = value;

  intensity_pub->publish(msg);
}

bool DigitalIntensityMonitor::service_callback(
  const std::shared_ptr<mirte_msgs::srv::GetIntensityDigital::Request> req,
  std::shared_ptr<mirte_msgs::srv::GetIntensityDigital::Response> res)
{
  res->data = value;
  return true;
}

bool AnalogIntensityMonitor::service_callback(
  const std::shared_ptr<mirte_msgs::srv::GetIntensity::Request> req,
  std::shared_ptr<mirte_msgs::srv::GetIntensity::Response> res)
{
  res->data = value;
  return true;
}
