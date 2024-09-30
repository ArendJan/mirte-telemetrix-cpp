#include <mirte_telemetrix_cpp/sensors/base_sensor.hpp>
#include <cstdint>

#include "mirte_telemetrix_cpp/device.hpp"
#include "mirte_telemetrix_cpp/node_data.hpp"
#include "mirte_telemetrix_cpp/parsers/device_data.hpp"
#include "mirte_telemetrix_cpp/parsers/sensors/base_sensor_data.hpp"

Mirte_Sensor::Mirte_Sensor(NodeData node_data, std::vector<uint8_t> pins, SensorData data)
: TelemetrixDevice(node_data, pins, (DeviceData)data)
{
}