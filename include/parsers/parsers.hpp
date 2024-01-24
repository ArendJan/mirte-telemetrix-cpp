#pragma once
#include "util.hpp"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <set>
#include "ros.hpp"
using pin_t = uint8_t;
 
class Parser {
public:
  node_handle nh;
  std::map<std::string, std::string> params;
  Parser(node_handle nh);
  std::map<std::string, std::string> get_params_name(std::string name);
  std::set<std::string> get_params_keys(std::string name);
  std::string build_param_name(std::string name, std::string key);
  int get_frequency() {
    return 50; // TODO: make this configurable
  }
};
  