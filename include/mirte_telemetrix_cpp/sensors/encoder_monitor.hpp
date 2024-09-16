#pragma once

#include <rclcpp/rclcpp.hpp>

#include <mirte_telemetrix_cpp/sensors/base_sensor.hpp>

#include <mirte_telemetrix_cpp/parsers/p_sensors.hpp>

#include <mirte_msgs/msg/encoder.hpp>
#include <mirte_msgs/srv/get_encoder.hpp>

class EncoderMonitor : public Mirte_Sensor {
public:
  EncoderMonitor(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
                 std::shared_ptr<Mirte_Board> board,
                 std::shared_ptr<Encoder_data> encoder_data);
  void publish();
  std::shared_ptr<Encoder_data> encoder_data;
  std::shared_ptr<rclcpp::Publisher<mirte_msgs::msg::Encoder>> encoder_pub;
  static std::vector<std::shared_ptr<EncoderMonitor>> get_encoder_monitors(
      std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<TMX> tmx,
      std::shared_ptr<Mirte_Board> board, std::shared_ptr<Parser> parser);
  void callback(int16_t value);
  int16_t value;
  std::shared_ptr<rclcpp::Service<mirte_msgs::srv::GetEncoder>> encoder_service;
  bool service_callback(
      const std::shared_ptr<mirte_msgs::srv::GetEncoder::Request> req,
      std::shared_ptr<mirte_msgs::srv::GetEncoder::Response> res);
};