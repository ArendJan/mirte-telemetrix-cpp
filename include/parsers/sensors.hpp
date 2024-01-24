#pragma once

#include "parsers.hpp"
#include "mirte-board.hpp"

class Sonar_data
{
public:
  std::string name = "";
  pin_t trigger = -1;
  pin_t echo = -1;
  Sonar_data(std::string name, pin_t trigger, pin_t echo)
  {
    this->name = name;
    this->trigger = trigger;
    this->echo = echo;
  }
  Sonar_data() {}
  static std::vector<std::shared_ptr<Sonar_data>> parse_sonar_data(
    std::shared_ptr<Parser> parser,
    std::shared_ptr<Mirte_Board> board);
  bool check()
  {
    return trigger != -1 && echo != -1 && name != "";
  }
};

class Intensity_data
{
public:
  std::string name = "";
  pin_t a_pin = -1;
  pin_t d_pin = -1;
  Intensity_data(std::string name, pin_t a_pin, pin_t d_pin)
  {
    this->name = name;
    this->a_pin = a_pin;
    this->d_pin = d_pin;
  }
  Intensity_data() {}
  static std::vector<std::shared_ptr<Intensity_data>> parse_intensity_data(
    std::shared_ptr<Parser> parser,
    std::shared_ptr<Mirte_Board> board);
  bool check()
  {
    return (a_pin != -1 || d_pin != -1) && name != "";
  }
};


class Keypad_data {
public:
  std::string name;
  pin_t pin = -1;
  Keypad_data(std::string name, pin_t pin)
  {
    this->name = name;
    this->pin = pin;
  }
  Keypad_data() {}
  static std::vector<std::shared_ptr<Keypad_data>> parse_keypad_data(
    std::shared_ptr<Parser> parser,
    std::shared_ptr<Mirte_Board> board);
  bool check()
  {
    return pin!=-1 && name != "";
  }
};