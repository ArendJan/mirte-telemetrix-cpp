#pragma once
#include "mirte-actuators.hpp"
#include "mirte-board.hpp"
#include "mirte-ping.hpp"
#include "mirte-sensors.hpp"
#include "ros.hpp"
#include <memory>
#include <tmx.hpp>

#if TMX_ROS_VERSION == 2
class mirte_node : public rclcpp::Node
{
#elif TMX_ROS_VERSION == 1
class mirte_node : public ros::NodeHandle
{
#endif
private:
  /* data */

public:
  mirte_node(/* args */);
  ~mirte_node();
  void start(node_handle s_node);
  std::shared_ptr<Mirte_Board> s_board;
  std::shared_ptr<TMX> s_tmx;
  std::shared_ptr<Parser> p_s;
  std::shared_ptr<Mirte_Sensors> monitor;
  std::shared_ptr<Mirte_Actuators> actuators;
  std::shared_ptr<Mirte_Ping> ping;
};
