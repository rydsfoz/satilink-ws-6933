// eeprom (...)
//
// TODO: Explicacao/descricao (...)

// This program is open source. For license terms, see the LICENSE file.
// Licensed under the Academic Free License version 3.0.

// Author: Rosiney D. da Silva, 2020-2024.

#ifndef EEPROM_H
#define EEPROM_H

#include <inttypes.h>
#include "btn_leds.h"
#include "language.h" // "idx_on_off_t"

typedef enum lnb_power_t {
  PWR_13V_18V,
  PWR_OFF,
  PWR_13V,
  PWR_18V
} lnb_power_t;

typedef enum pwr_unit_t {
  DBUV,
  DBMV,
  DBM
} pwr_unit_t;

typedef struct {
  idx_on_off_t key_tone;
  idx_on_off_t lock_tone;
  idx_on_off_t flashlight;
  lnb_power_t lnb_power;
  pwr_unit_t pwr_unit;
  uint8_t sat_idx;
  uint8_t tp_idx;
  uint8_t language_idx;
} eeprom_8_t;

typedef struct {
  uint8_t diseqc_mode;
  uint8_t idx_if_channel;
  char str_center_frequency[8][5];
  uint8_t version;
} eeprom_43_t;

typedef struct {
  char sat_longitude[6];
  char sat_lon_direction;
  char local_longitude[6];
  char local_lon_direction;
  char local_latitude[5];
  char local_lat_direction;
  char elevation[7];
  char azimuth[7];
  char polarization[7];
} eeprom_41_t;

void eeprom_write(uint32_t addr, const uint8_t *data, uint32_t size);
void eeprom_read(uint32_t addr, uint8_t *data, uint32_t size);

void eeprom_codeword_write(const uint8_t *data);
void eeprom_codeword_read(uint8_t *data);
void eeprom_codeword_verify(void);
void eeprom_write_10_8(void);
void eeprom_read_10_8(void);
void eeprom_write_30_43(void);
void eeprom_read_write_30_43(void);
void eeprom_write_5da_41(void);
void eeprom_read_5da_41(void);

uint32_t crc_32(uint32_t crc, const uint8_t *data, uint32_t size);
void sat_lon_dir_update(btn_t btn);
void local_lon_dir_update(btn_t btn);
void local_lat_dir_update(btn_t btn);
void language_update(btn_t btn);
void key_tone_update(btn_t btn);
void lock_tone_update(btn_t btn);
void pwr_update(btn_t btn);
void flashlight_update(btn_t btn);
void flashlight_on_off(void);

extern uint32_t eeprom_codeword;
extern eeprom_8_t eeprom_10h_8;
extern eeprom_43_t eeprom_30h_43;
extern eeprom_41_t eeprom_5dah_41;

#endif // EEPROM_H

// Reference(s):
//   @1: A: B - C (D)
