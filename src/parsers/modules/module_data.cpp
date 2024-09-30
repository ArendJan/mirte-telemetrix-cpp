#include <rcpputils/asserts.hpp>
#include <mirte_telemetrix_cpp/parsers/modules/module_data.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/format.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/optional/optional.hpp>
#include <rclcpp/parameter_value.hpp>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>

#include "mirte_telemetrix_cpp/parsers/device_data.hpp"
#include "mirte_telemetrix_cpp/parsers/parsers.hpp"

class Mirte_Board;

ModuleData::ModuleData(
  std::shared_ptr<Parser> parser, std::shared_ptr<Mirte_Board> board, std::string name,
  std::map<std::string, rclcpp::ParameterValue> parameters, std::set<std::string>& unused_keys)
: DeviceData(parser, board, name, this->get_device_class(), parameters, unused_keys)
{
  rcpputils::require_true(
    unused_keys.erase("type"),
    (boost::format("A module configuration requires a 'type' to be specifie, but no "
                   "'type' was for module '%1%'") %
     name)
      .str());
  this->type = get_string(parameters["type"]);
}

bool ModuleData::check(std::string module_type)
{
  return boost::to_lower_copy(type) == boost::to_lower_copy(module_type) && DeviceData::check();
}

bool ModuleData::check() { return false; }