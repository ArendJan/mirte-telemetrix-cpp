#include <boost/algorithm/string.hpp>
#include <parsers/actuators.hpp>

std::vector<std::shared_ptr<Servo_data>>
Servo_data::parse_servo_data(std::shared_ptr<Parser> parser,
                             std::shared_ptr<Mirte_Board> board) {
  std::vector<std::shared_ptr<Servo_data>> servos;
  for (auto name : parser->get_params_keys("servo")) {
    std::cout << name << std::endl;
    auto servo_key = parser->build_param_name("servo", name);
    auto servos_config = parser->get_params_name(servo_key);
    auto servo_keys = parser->get_params_keys(servo_key);
    Servo_data servo_data;
    servo_data.name = name;
    if (servos_config.count("connector")) {
      std::string connector = servos_config["connector"].get<std::string>();
      auto pins = board->resolveConnector(connector);
      servo_data.pin = pins["pin"];
    }
    if (servo_keys.count("pins")) {
      // pins:
      //   pin: 1
      servo_data.pin =
          board->resolvePin(parser
                                ->get_params_name(parser->build_param_name(
                                    servo_key, "pins"))["pin"]
                                .get<std::string>());
    }

    if (servo_keys.count("min_pulse")) {
      servo_data.min_pulse = servos_config["min_pulse"].get<int>();
    }
    if (servo_keys.count("max_pulse")) {
      servo_data.max_pulse = servos_config["max_pulse"].get<int>();
    }
    if (servo_data.check()) {
      servos.push_back(std::make_shared<Servo_data>(servo_data));
    }
  }
  return servos;
}

std::vector<std::shared_ptr<Motor_data>>
Motor_data::parse_motor_data(std::shared_ptr<Parser> parser,
                             std::shared_ptr<Mirte_Board> board) {
  std::vector<std::shared_ptr<Motor_data>> motors;
  for (auto name : parser->get_params_keys("motor")) {
    auto motor_key = parser->build_param_name("motor", name);
    auto motor_config = parser->get_params_name(motor_key);
    auto motor_keys = parser->get_params_keys(motor_key);

    Motor_data motor_data;
    motor_data.name = name;
    if (motor_keys.count("pins")) {
      std::cout << name << "pins" << std::endl;
      auto pins_config =
          parser->get_params_name(parser->build_param_name(motor_key, "pins"));
      for (auto pin_key : parser->get_params_keys(
               parser->build_param_name(motor_key, "pins"))) {
        boost::algorithm::to_lower(pin_key);
        if ("p1" == pin_key) {
          motor_data.P1 =
              board->resolvePin(pins_config[pin_key].get<std::string>());
        }
        if ("p2" == pin_key) {
          motor_data.P2 =
              board->resolvePin(pins_config[pin_key].get<std::string>());
        }
        if ("p1" == pin_key) {
          motor_data.D1 =
              board->resolvePin(pins_config[pin_key].get<std::string>());
        }
        if ("p2" == pin_key) {
          motor_data.D2 =
              board->resolvePin(pins_config[pin_key].get<std::string>());
        }
      }
    } else if (motor_keys.count("connector")) {
      std::string connector = motor_config["connector"].get<std::string>();
      auto conn_pins = board->resolveConnector(connector);
      motor_data.P1 = conn_pins["P1"];
      motor_data.P2 = conn_pins["P2"];
      motor_data.D1 = conn_pins["D1"];
      motor_data.D2 = conn_pins["D2"];
    }
    if (motor_keys.count("type")) {
      std::string type = motor_config["type"].get<std::string>();
      boost::algorithm::to_lower(type);
      if (type == "pp") {
        motor_data.type = Motor_data::Motor_type::PP;
      } else if (type == "dp") {
        motor_data.type = Motor_data::Motor_type::DP;
      } else if (type == "ddp") {
        motor_data.type = Motor_data::Motor_type::DDP;
      } else {
        std::cout << "Unknown motor type: " << type << std::endl;
      }
    } else {
      motor_data.type = Motor_data::Motor_type::PP;
    }
    if (motor_keys.count("inverted")) {
      motor_data.inverted = motor_config["inverted"].get<bool>();
    }
    if (motor_data.check()) {
      motors.push_back(std::make_shared<Motor_data>(motor_data));
    }
  }
  return motors;
}

// TODO: oled and module actuators
