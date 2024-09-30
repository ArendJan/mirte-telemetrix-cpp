#pragma once

#include <mirte_telemetrix_cpp/device.hpp>
#include <mirte_telemetrix_cpp/parsers/modules/module_data.hpp>
#include <vector>

#include "mirte_telemetrix_cpp/parsers/parsers.hpp"

class ModuleData;
struct NodeData;

class Mirte_module : public TelemetrixDevice
{
public:
  // ??? Pins was added
  Mirte_module(NodeData node_data, std::vector<pin_t> pins, ModuleData module_data);
};