#pragma once
#include "ros.hpp"
#include <functional>
#include <memory>
#include <thread>
#include <tmx.hpp>
#include <vector>
class Mirte_Ping {
public:
  Mirte_Ping(node_handle nh, std::shared_ptr<TMX> tmx,
             std::function<void()> stop_func);
  std::shared_ptr<TMX> tmx;
  node_handle nh;
  void ping_task();
  std::thread ping_thread; // TODO: jthread from c++20
  void ping_callback(const std::vector<uint8_t> pins);
  uint8_t last_ping = 0;
  uint8_t magic = 0;
  bool first_magic = true;
  std::function<void()> stop_func;
  void stop();
  bool is_stopped = false;
};
