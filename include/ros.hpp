#pragma once

#define TMX_ROS_VERSION 1
#if TMX_ROS_VERSION == 2
#include <rclcpp/rclcpp.hpp>

#include "mirte_msgs/msg/keypad.hpp"
#include "mirte_msgs/srv/get_keypad.hpp"
#include "std_msgs/msg/header.hpp"
using node_handle = std::shared_ptr<rclcpp::Node>;
template <typename T> using publisher = std::shared_ptr<rclcpp::Publisher<T>>;

template <typename T>
using subscriber = std::shared_ptr<rclcpp::Subscription<T>>;

template <typename T> using service = std::shared_ptr<rclcpp::Service<T>>;

//  message types
#include "sensor_msgs/msg/range.hpp"
// #include "sensor_msgs/msg/"
using sensor_msgs_range = sensor_msgs::msg::Range;

#include "mirte_msgs/srv/get_distance.hpp"
using mirte_msgs_get_distance = mirte_msgs::srv::GetDistance;

// #include "std_msgs/msg/bool.hpp"
// using std_msgs_bool = std_msgs::msg::Bool;

#include "mirte_msgs/msg/intensity_digital.hpp"
using mirte_msgs_intensity_digital = mirte_msgs::msg::IntensityDigital;

#include "mirte_msgs/msg/intensity.hpp"
using mirte_msgs_intensity = mirte_msgs::msg::Intensity;

#include "mirte_msgs/srv/get_intensity_digital.hpp"
using mirte_msgs_get_intensity_digital = mirte_msgs::srv::GetIntensityDigital;

#include "mirte_msgs/srv/get_intensity.hpp"
using mirte_msgs_get_intensity = mirte_msgs::srv::GetIntensity;

#include "mirte_msgs/srv/get_pin_value.hpp"
using mirte_msgs_get_pin_value = mirte_msgs::srv::GetPinValue;

#include "std_msgs/msg/header.hpp"
using std_msgs_header = std_msgs::msg::Header;
#include "std_msgs/msg/int32.hpp"
using std_msgs_int32 = std_msgs::msg::Int32;

#include "mirte_msgs/srv/set_motor_speed.hpp"
using mirte_msgs_set_motor_speed = mirte_msgs::srv::SetMotorSpeed;

using ros_timer = rclcpp::TimerBase::SharedPtr;

#include "mirte_msgs/msg/keypad.hpp"	
using mirte_msgs_keypad = mirte_msgs::msg::Keypad;

#include "mirte_msgs/srv/get_keypad.hpp"
using mirte_msgs_get_keypad = mirte_msgs::srv::GetKeypad;

#elif TMX_ROS_VERSION == 1
#include <ros/ros.h>

#define create_service advertiseService
#define create_subscription subscribe

using node_handle =std::shared_ptr< ros::NodeHandle>;
template <typename T> using publisher = ros::Publisher;
template <typename T> using subscriber = ros::Subscriber;
template <typename T> using service = ros::ServiceServer;

//  message types
#include "sensor_msgs/Range.h"
using sensor_msgs_range = sensor_msgs::Range;

#include "mirte_msgs/GetDistance.h"
using mirte_msgs_get_distance = mirte_msgs::GetDistance;

#include "std_msgs/Bool.h"
using std_msgs_bool = std_msgs::Bool;

#include "mirte_msgs/IntensityDigital.h"
using mirte_msgs_intensity_digital = mirte_msgs::IntensityDigital;

#include "mirte_msgs/Intensity.h"
using mirte_msgs_intensity = mirte_msgs::Intensity;

#include "mirte_msgs/GetIntensityDigital.h"
using mirte_msgs_get_intensity_digital = mirte_msgs::GetIntensityDigital;

#include "mirte_msgs/GetIntensity.h"
using mirte_msgs_get_intensity = mirte_msgs::GetIntensity;

#include "mirte_msgs/GetPinValue.h"
using mirte_msgs_get_pin_value = mirte_msgs::GetPinValue;

#include "std_msgs/Header.h"
using std_msgs_header = std_msgs::Header;

#include "std_msgs/Int32.h"
using std_msgs_int32 = std_msgs::Int32;

#include "mirte_msgs/SetMotorSpeed.h"
using mirte_msgs_set_motor_speed = mirte_msgs::SetMotorSpeed;

using ros_timer = ros::TimerEvent;

#include "mirte_msgs/Keypad.h"
using mirte_msgs_keypad = mirte_msgs::Keypad;

#include "mirte_msgs/GetKeypad.h"
using mirte_msgs_get_keypad = mirte_msgs::GetKeypad;
#endif
