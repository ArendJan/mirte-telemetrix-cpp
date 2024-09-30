#pragma once
#include <rclcpp/rclcpp.hpp>
#include <mirte_telemetrix_cpp/mirte-board.hpp>
#include <mirte_telemetrix_cpp/device.hpp>
#include <mirte_telemetrix_cpp/node_data.hpp>
#include <tmx_cpp/tmx.hpp>
#include <mirte_msgs/srv/set_pin_value.hpp>
#include <std_msgs/msg/header.hpp>
#include <std_msgs/msg/int32.hpp>
#include <mirte_msgs/srv/detail/set_pin_value__struct.hpp>
#include <rclcpp/service.hpp>
#include <memory>
#include <string>
#include <vector>

#include "mirte_telemetrix_cpp/parsers/parsers.hpp"

class Mirte_Actuator;
class DeviceData;
class Mirte_Board;
namespace rclcpp {
class Node;
}  // namespace rclcpp
namespace tmx_cpp {
class TMX;
}  // namespace tmx_cpp
struct NodeData;

class Mirte_Actuators
{
public:
  Mirte_Actuators(NodeData data, std::shared_ptr<Parser> parser);

  std::shared_ptr<tmx_cpp::TMX> tmx;
  std::shared_ptr<rclcpp::Node> nh;
  std::shared_ptr<Mirte_Board> board;
  std::vector<std::shared_ptr<Mirte_Actuator>> actuators;

  rclcpp::Service<mirte_msgs::srv::SetPinValue>::SharedPtr set_pin_value_service;

private:
  void set_pin_value_service_callback(
    const mirte_msgs::srv::SetPinValue::Request::ConstSharedPtr req,
    mirte_msgs::srv::SetPinValue::Response::SharedPtr res);
};

class Mirte_Actuator: public TelemetrixDevice
{
public:
  Mirte_Actuator(NodeData node_data, std::vector<pin_t> pins, DeviceData data);
  virtual ~Mirte_Actuator() {}
};
