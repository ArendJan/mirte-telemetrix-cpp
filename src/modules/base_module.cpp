#include <mirte_telemetrix_cpp/modules/base_module.hpp>
#include <cstdint>

#include "mirte_telemetrix_cpp/device.hpp"
#include "mirte_telemetrix_cpp/node_data.hpp"
#include "mirte_telemetrix_cpp/parsers/device_data.hpp"
#include "mirte_telemetrix_cpp/parsers/modules/module_data.hpp"

Mirte_module::Mirte_module(NodeData node_data, std::vector<pin_t> pins, ModuleData module_data)
: TelemetrixDevice(node_data, pins, (DeviceData)module_data)
{
}