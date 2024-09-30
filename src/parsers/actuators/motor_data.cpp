#include <mirte_telemetrix_cpp/parsers/actuators/motor_data.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/node.hpp>
#include <rclcpp/parameter_value.hpp>
#include <map>
#include <memory>
#include <set>
#include <string>

#include "mirte_telemetrix_cpp/mirte-board.hpp"
#include "mirte_telemetrix_cpp/parsers/device_data.hpp"
#include "mirte_telemetrix_cpp/parsers/parsers.hpp"

MotorData::MotorData(
  std::shared_ptr<Parser> parser, std::shared_ptr<Mirte_Board> board, std::string name,
  std::map<std::string, rclcpp::ParameterValue> parameters, std::set<std::string> & unused_keys)
: DeviceData(parser, board, name, MotorData::get_device_class(), parameters, unused_keys)
{
  auto logger = parser->nh->get_logger();

  if (unused_keys.erase("connector")) {
    auto connector = get_string(parameters["connector"]);
    auto pins = board->resolveConnector(connector);

    this->P1 = pins["P1"];
    this->P2 = pins["P2"];
    this->D1 = pins["D1"];
    this->D2 = pins["D2"];
  } else if (unused_keys.erase("pins")) {
    if (parameters.count("pins.p1"))
      this->P1 = board->resolvePin(get_string(parameters["pins.p1"]));

    if (parameters.count("pins.p2"))
      this->P2 = board->resolvePin(get_string(parameters["pins.p2"]));

    if (parameters.count("pins.d1"))
      this->D1 = board->resolvePin(get_string(parameters["pins.d1"]));

    if (parameters.count("pins.p2"))
      this->D2 = board->resolvePin(get_string(parameters["pins.d2"]));
  } else
    RCLCPP_ERROR(
      logger, "Device %s.%s has no a connector or pins specified.", get_device_class().c_str(),
      name.c_str());

  if (unused_keys.erase("type")) {
    std::string motor_type = get_string(parameters["type"]);
    boost::algorithm::to_lower(motor_type);
    if (motor_type == "pp")
      this->type = MotorType::PP;
    else if (motor_type == "dp")
      this->type = MotorType::DP;
    else if (motor_type == "ddp")
      this->type = MotorType::DDP;
    else
      RCLCPP_ERROR(
        logger, "Unknown Motor type '%s' for Motor %s | Defaulting to PP Motor", motor_type.c_str(),
        name.c_str());
  } else {
    RCLCPP_WARN(logger, "No motor type found for %s defaulting to PP motor", name.c_str());
  }

  if (unused_keys.erase("inverted")) this->inverted = parameters["inverted"].get<bool>();
}

bool MotorData::check()
{
  bool device_ok = DeviceData::check();

  switch (type) {
    case MotorType::PP:
      return P1 != (pin_t)-1 && P2 != (pin_t)-1 && device_ok;
    case MotorType::DP:
      return P1 != (pin_t)-1 && D1 != (pin_t)-1 && device_ok;
    case MotorType::DDP:
      return P1 != (pin_t)-1 && D1 != (pin_t)-1 && D2 != (pin_t)-1 && device_ok;
    default:
      return false;
  }
}
