#include <mirte_telemetrix_cpp/parsers/sensors/base_sensor_data.hpp>
#include <rclcpp/parameter_value.hpp>
#include <map>
#include <memory>
#include <set>
#include <string>

#include "mirte_telemetrix_cpp/parsers/device_data.hpp"

class Mirte_Board;
class Parser;

SensorData::SensorData(
  std::shared_ptr<Parser> parser, std::shared_ptr<Mirte_Board> board, std::string name,
  std::string sensor_type, std::map<std::string, rclcpp::ParameterValue> parameters, std::set<std::string>& unused_keys)
: DeviceData(parser, board, name, sensor_type, parameters, unused_keys)
{
}