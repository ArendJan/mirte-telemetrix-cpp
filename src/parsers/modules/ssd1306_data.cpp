#include <mirte_telemetrix_cpp/parsers/modules/ssd1306_data.hpp>

SSD1306Data::SSD1306Data(
  std::shared_ptr<Parser> parser, std::shared_ptr<Mirte_Board> board, std::string name,
  std::map<std::string, rclcpp::ParameterValue> parameters, std::set<std::string> & unused_keys)
: I2CModuleData(parser, board, name, parameters, unused_keys)
{
  // Set default for address
  if ((!parameters.count("addr")) && this->addr == 0xFF) this->addr = 0x3C;

  // TODO: Read in size [BLOCKED BY FIXED SIZE FIRMWARE]

  if (unused_keys.erase("legacy")) this->legacy = parameters["legacy"].get<bool>();

  if (unused_keys.erase("default_image_path"))
    this->default_image_path = parameters["default_image_path"].get<std::string>();
}

bool SSD1306Data::check() { return I2CModuleData::check(get_module_type()); }