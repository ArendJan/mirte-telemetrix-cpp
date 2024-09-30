#include <mirte_telemetrix_cpp/parsers/sensors/keypad_data.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/node.hpp>
#include <rclcpp/parameter_value.hpp>

#include "mirte_telemetrix_cpp/mirte-board.hpp"
#include "mirte_telemetrix_cpp/parsers/sensors/base_sensor_data.hpp"

KeypadData::KeypadData(
  std::shared_ptr<Parser> parser, std::shared_ptr<Mirte_Board> board, std::string name,
  std::map<std::string, rclcpp::ParameterValue> parameters, std::set<std::string> & unused_keys)
: SensorData(parser, board, name, KeypadData::get_device_class(), parameters, unused_keys)
{
  auto logger = parser->nh->get_logger();

  if (unused_keys.erase("connector")) {
    auto connector = get_string(parameters["connector"]);
    auto pins = board->resolveConnector(connector);
    this->pin = pins["pin"];
  } else if (unused_keys.erase("pins")) {
    if (parameters.count("pins")) this->pin = board->resolvePin(get_string(parameters["pins.pin"]));
  } else
    RCLCPP_ERROR(
      logger, "Device %s.%s has no a connector or pins specified.", get_device_class().c_str(),
      name.c_str());
}

bool KeypadData::check() { return pin != (pin_t)-1 && SensorData::check(); }