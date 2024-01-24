#include "parsers/parsers.hpp"
#include <set>

  


Parser::Parser(node_handle nh)
{
  this->nh = nh;
  #if TMX_ROS_VERSION == 2
  auto node_parameters_iface = nh->get_node_parameters_interface();
  auto parameter_overrides =
    node_parameters_iface->get_parameter_overrides();
  #else
  std::vector<std::string> keys;
nh.getParamNames(keys);
  std::map<std::string, std::string> parameter_overrides;
  for(auto key : keys) {
    std::string val;
    nh->getParam(key, val);
    parameter_overrides[key] = val;
  }
  #endif
  this->params = parameter_overrides;
}

/**
 * get the parameters starting with name
 * removes the name from the key, including the dot
 *
*/
std::map<std::string, rclcpp::ParameterValue> Parser::get_params_name(std::string name)
{
  std::map<std::string, rclcpp::ParameterValue> out_params;
  for (auto & servo_it : this->params) {
    if (starts_with(servo_it.first, name) ) {
      std::string key = servo_it.first.substr(name.length() + 1);
      out_params[key] = servo_it.second;
    }
  }
  return out_params;
}

std::set<std::string> Parser::get_params_keys(std::string name)
{
  std::set<std::string> out_params;
  for (auto & servo_it : this->params) {
    if (starts_with(servo_it.first, name) ) {
      std::string key = servo_it.first.substr(name.length() + 1);
      auto next_dot = key.find(".");
      key = key.substr(0, next_dot);
      out_params.insert(key);
    }
  }
  return out_params;
}

std::string Parser::build_param_name(std::string name, std::string key)
{
  return name + "." + key;
}


std::string get_string(rclcpp::ParameterValue param) {
  if (param.get_type() == rclcpp::ParameterType::PARAMETER_STRING) {
    return param.get<std::string>();
  } else {
    return rclcpp::to_string(param);
  }
} 