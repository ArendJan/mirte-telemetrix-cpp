#include <mirte_telemetrix_cpp/sensors/encoder_monitor.hpp>
#include <rcl/context.h>
#include <mirte_msgs/msg/detail/encoder__struct.hpp>
#include <mirte_msgs/srv/detail/get_encoder__struct.hpp>
#include <rclcpp/node.hpp>
#include <rclcpp/publisher.hpp>
#include <rclcpp/qos_event.hpp>
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <variant>
#include <vector>

#include "mirte_telemetrix_cpp/node_data.hpp"
#include "mirte_telemetrix_cpp/parsers/device_data.hpp"
#include "mirte_telemetrix_cpp/parsers/sensors/base_sensor_data.hpp"
#include "mirte_telemetrix_cpp/parsers/sensors/encoder_data.hpp"
#include "mirte_telemetrix_cpp/sensors/base_sensor.hpp"
#include "rclcpp/node.hpp"
#include "tmx_cpp/tmx.hpp"

class Parser;

EncoderMonitor::EncoderMonitor(NodeData node_data, EncoderData encoder_data)
: Mirte_Sensor(node_data, {encoder_data.pinA, encoder_data.pinB}, (SensorData)encoder_data),
  encoder_data(encoder_data)
{
  encoder_pub = nh->create_publisher<mirte_msgs::msg::Encoder>("encoder/" + encoder_data.name, 1);
  encoder_service = nh->create_service<mirte_msgs::srv::GetEncoder>(
    "get_encoder_" + encoder_data.name,
    std::bind(
      &EncoderMonitor::service_callback, this, std::placeholders::_1, std::placeholders::_2));

  tmx->attach_encoder(
    encoder_data.pinA, encoder_data.pinB, [this](auto pin, auto value) { this->callback(value); });
}

void EncoderMonitor::callback(int16_t value)
{
  value += value;
  update();
}

void EncoderMonitor::update()
{
  mirte_msgs::msg::Encoder msg;

  msg.header = get_header();
  msg.value = value;

  encoder_pub->publish(msg);
}

bool EncoderMonitor::service_callback(
  const std::shared_ptr<mirte_msgs::srv::GetEncoder::Request> req,
  std::shared_ptr<mirte_msgs::srv::GetEncoder::Response> res)
{
  res->data = value;
  return true;
}

std::vector<std::shared_ptr<EncoderMonitor>> EncoderMonitor::get_encoder_monitors(
  NodeData node_data, std::shared_ptr<Parser> parser)
{
  std::vector<std::shared_ptr<EncoderMonitor>> sensors;
  auto encoders = parse_all<EncoderData>(parser, node_data.board);
  for (auto encoder : encoders) {
    sensors.push_back(std::make_shared<EncoderMonitor>(node_data, encoder));
    // std::cout << "Add Encoder: " << encoder.name << std::endl;
  }
  return sensors;
}
