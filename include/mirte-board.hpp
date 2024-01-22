#include <map>
#include <string>
using pin_map = std::map<std::string, std::string>;
using connector_map = std::map<std::string, pin_map>;

#ifndef MIRTE_BOARD_HPP
#define MIRTE_BOARD_HPP
#include <ros.hpp>

#include <tmx.hpp>
#include <memory>
#include <vector>
#include "util.hpp"
#include "pcbs/v06.hpp"
#include "pcbs/v08.hpp"
#include "parsers/parsers.hpp"

class Mirte_Board
{
public:
  // Mirte_Board(
  //   // std::shared_ptr<TMX> tmx, std::shared_ptr<rclcpp::Node> nh
  //   );
  // std::shared_ptr<TMX> tmx;
  // std::shared_ptr<rclcpp::Node> nh;
  virtual int get_adc_bits() = 0;
  // std::vector<uint8_t> resolvePins(std::string pin);
  virtual int resolvePin(std::string pin) = 0;
  virtual std::map<std::string, int> resolveConnector(std::string connector) = 0;
  static std::shared_ptr<Mirte_Board> create(std::shared_ptr<Parser> parser);
};

class Mirte_Board_atmega328p : public Mirte_Board
{
public:
  Mirte_Board_atmega328p();
  std::map<std::string, int> resolveConnector(std::string connector);
  int resolvePin(std::string pin);
  int get_adc_bits();
};
class Mirte_Board_pico : public Mirte_Board
{
public:
  // Mirte_Board_pico(
  //   // std::shared_ptr<TMX> tmx, std::shared_ptr<rclcpp::Node> nh
  //   );
  Mirte_Board_pico();
  std::map<std::string, int> resolveConnector(std::string connector);
  int resolvePin(std::string pin);
  int get_adc_bits() {return 12;}
};

class Mirte_Board_pcb : public Mirte_Board
{
public:
  Mirte_Board_pcb(std::shared_ptr<Mirte_Board> mcu, std::string version);
  // Mirte_Board_pcb(
  //   // std::shared_ptr<TMX> tmx, std::shared_ptr<rclcpp::Node> nh,
  //   std::shared_ptr<Mirte_Board> mcu);
  std::shared_ptr<Mirte_Board> mcu; // to look up pins
  std::map<std::string, int> resolveConnector(std::string connector);
  int resolvePin(std::string pin);
  int get_adc_bits() {return mcu->get_adc_bits();}
  std::string version;
  connector_map connectors = mirte_pico_pcb_map08;
};

#endif