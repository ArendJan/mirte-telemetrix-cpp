#include <mirte_telemetrix_cpp/parsers/actuators/servo_data.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/node.hpp>
#include <rclcpp/parameter_value.hpp>

#include "mirte_telemetrix_cpp/mirte-board.hpp"
#include "mirte_telemetrix_cpp/parsers/device_data.hpp"

ServoData::ServoData(
  std::shared_ptr<Parser> parser, std::shared_ptr<Mirte_Board> board, std::string name,
  std::map<std::string, rclcpp::ParameterValue> parameters, std::set<std::string> & unused_keys)
: DeviceData(parser, board, name, ServoData::get_device_class(), parameters, unused_keys)
{
  auto logger = parser->nh->get_logger();

  if (unused_keys.erase("connector")) {
    auto connector = get_string(parameters["connector"]);
    auto pins = board->resolveConnector(connector);

    this->pin = pins["pin"];
  } else if (unused_keys.erase("pins")) {
    if (parameters.count("pins.pin"))
      this->pin = board->resolvePin(get_string(parameters["pins.pin"]));
  } else
    RCLCPP_ERROR(
      logger, "Device %s.%s has no a connector or pins specified.", get_device_class().c_str(),
      name.c_str());

  if (unused_keys.erase("min_pulse")) this->min_pulse = parameters["min_pulse"].get<int>();

  if (unused_keys.erase("max_pulse")) this->max_pulse = parameters["max_pulse"].get<int>();

  if (unused_keys.erase("min_angle")) this->min_angle = get_float(parameters["min_angle"]);

  if (unused_keys.erase("max_angle")) this->max_angle = get_float(parameters["max_angle"]);
}

bool ServoData::check() { return pin != (pin_t)-1 && DeviceData::check(); }