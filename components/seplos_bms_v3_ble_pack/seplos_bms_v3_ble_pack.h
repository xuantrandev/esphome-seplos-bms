#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "../seplos_bms_v3_ble/seplos_bms_v3_ble.h"

#ifdef USE_ESP32

namespace esphome {
namespace seplos_bms_v3_ble_pack {

class SeplosBmsV3BlePack : public Component, public seplos_bms_v3_ble::SeplosBmsV3BlePack {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_pack_voltage_sensor(sensor::Sensor *sensor) { pack_voltage_sensor_ = sensor; }
  void set_pack_current_sensor(sensor::Sensor *sensor) { pack_current_sensor_ = sensor; }
  void set_pack_battery_level_sensor(sensor::Sensor *sensor) { pack_battery_level_sensor_ = sensor; }
  void set_pack_cycle_sensor(sensor::Sensor *sensor) { pack_cycle_sensor_ = sensor; }

  void set_pack_cell_voltage_sensor(uint8_t index, sensor::Sensor *sensor) {
    pack_cell_voltage_sensors_[index] = sensor;
  }
  void set_pack_temperature_sensor(uint8_t index, sensor::Sensor *sensor) { pack_temperature_sensors_[index] = sensor; }

  void set_ambient_temperature_sensor(sensor::Sensor *sensor) { ambient_temperature_sensor_ = sensor; }
  void set_mosfet_temperature_sensor(sensor::Sensor *sensor) { mosfet_temperature_sensor_ = sensor; }

  void on_frame_data(const std::vector<uint8_t> &frame) override;
  void set_fastdata(bool fastdata){this->fastdata_ = fastdata;};
  void set_data_text_sensor(text_sensor::TextSensor *data_text_sensor) { data_text_sensor_ = data_text_sensor; }

    // using for fake hina battery
  int16_t get_vbat100() { return vbat100_; };
  int16_t get_current10() { return current10_; };
  uint8_t get_soc() { return soc_; };
  uint8_t get_soh() { return soh_; };
  int16_t get_cycles() { return cycles_; };
  int16_t get_maxtemp10() { return maxtemp10_; };
  int16_t get_mintemp10() { return mintemp10_; };
  int16_t get_maxcellmv() { return maxcellmv_; };
  int16_t get_mincellmv() { return mincellmv_; };
  uint8_t get_mincellidx() { return mincellidx_; };
  uint8_t get_maxcellidx() { return maxcellidx_; };
  uint16_t get_capacity() { return capacity_; };
  bool get_online_status() { return online_status_; };
  void set_name(const std::string &name) { this->component_name_ = name; }
  const std::string &get_name() const { return this->component_name_; }
 
private:
  std::string component_name_;
 protected:
  bool online_status_{false};
  int16_t vbat100_{0};
  int16_t current10_{0};
  uint8_t soc_{0};
  uint8_t soh_{0};
  uint16_t capacity_{0};
  int16_t cycles_{0};
  int16_t maxtemp10_{0};
  int16_t mintemp10_{10000};
  int16_t maxcellmv_{0};
  int16_t mincellmv_{5000};
  uint8_t mincellidx_{0};
  uint8_t maxcellidx_{0};
  bool fastdata_{false};
  text_sensor::TextSensor *data_text_sensor_;
  void decode_pack_pia_data_(const std::vector<uint8_t> &data);
  void decode_pack_pib_data_(const std::vector<uint8_t> &data);
  void decode_pack_pic_data_(const std::vector<uint8_t> &data);
  void publish_state_(sensor::Sensor *sensor, float value);

  sensor::Sensor *pack_voltage_sensor_{nullptr};
  sensor::Sensor *pack_current_sensor_{nullptr};
  sensor::Sensor *pack_battery_level_sensor_{nullptr};
  sensor::Sensor *pack_cycle_sensor_{nullptr};
  sensor::Sensor *pack_cell_voltage_sensors_[16]{nullptr};
  sensor::Sensor *pack_temperature_sensors_[4]{nullptr};
  sensor::Sensor *ambient_temperature_sensor_{nullptr};
  sensor::Sensor *mosfet_temperature_sensor_{nullptr};
};

}  // namespace seplos_bms_v3_ble_pack
}  // namespace esphome

#endif
