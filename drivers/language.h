// language (...)
//
// TODO: Explicacao/descricao (...)

// This program is open source. For license terms, see the LICENSE file.
// Licensed under the Academic Free License version 3.0.

// Author: Rosiney D. da Silva, 2020-2024.

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <inttypes.h>

typedef enum {
  MENU,
  N,
  SAT_SETTING,
  SIGNAL_SCAN,
  TOOLS,
  SAT_MANAGER,
  TP_MANAGER,
  SYSTEM_SETUP
} idx_menu_t;

typedef enum {
  SAT_NAME,
  LNB_TYPE,
  LOW_FREQ,
  HIGH_FREQ,
  TP_NUM,
  FREQUENCY,
  SYMBRATE,
  POLARITY,
  TONE_22k,
  DISEQC1_0,
  DISEQC1_1,
  LNB_POWER,
  AUTO
} idx_sat_setting_t;

typedef enum {
  SATELLITE,
  FREQUENCY_,
  SYMBRATE_,
  POLARITY_
} idx_satellite_t;

typedef enum {
  SATELLITE_NAME,
  SAT_LONGITUDE,
  LOCAL_LONGITUDE,
  LOCAL_LATITUDE,
  AZIMUTH,
  ELEVATION,
  POLARIZATION
} idx_tools_t;

typedef enum {
  SAT_MANAGER_,
  ADD_SATELLITE,
  EDIT_SATELLITE,
  DELETE_SATELLITE
} idx_sat_manager_t;

typedef enum {
  TP_MANAGER_,
  ADD_TRANSPONDER,
  EDIT_TRANSPONDER,
  DELETE_TRANSPONDER
} idx_tp_manager_t;

typedef enum {
  OSD_LANGUAGE,
  KEY_TONE,
  LOCK_TONE,
  FLASHLIGHT,
  PWR_UNIT,
  LOCAL_LONGITUDE_,
  LOCAL_LATITUDE_
} idx_system_setup_t;

typedef enum {
  ON,
  OFF
} idx_on_off_t;

typedef enum {
  SETTING_WERE_CHANGED,
  DO_YOU_WANT_TO_SAVE
} idx_setting_changed_t;

//language

typedef enum {
  LNB_SINGLE,
  LNB_KU,
  LNB_UNIVERSAL,
  LNB_MDU1,
  LNB_MDU2,
  LNB_MDU3,
  LNB_MDU4,
  LNB_MDU5
} lnb_type_t;

typedef enum {
  H,
  V
} idx_polarity_t;

//diseqc_mode
//if_channel
//center_frequency

const char *get_str_menu(idx_menu_t item_idx);
const char *get_str_sat_setting(idx_sat_setting_t item_idx);
const char *get_str_satellite(idx_satellite_t item_idx);
const char *get_str_tools(idx_tools_t item_idx);
const char *get_str_sat_manager(idx_sat_manager_t item_idx);
const char *get_str_tp_manager(idx_tp_manager_t item_idx);
const char *get_str_system_setup(idx_system_setup_t item_idx);
const char *get_str_on_off(idx_on_off_t item_idx);
const char *get_str_setting_changed(idx_setting_changed_t item_idx);
const char *get_str_del(void);
const char *get_str_message(void);
const char *get_str_language(void);

extern const char *str_lnb_type[];
extern const char *str_lnb_power[];
extern const char *str_polarity[];
extern const char *str_diseqc_mode[];
extern const char *str_diseqc1[];
extern const char *str_if_channel[];
extern const char *str_center_frequency[];
extern const char *str_pwr_unit[];
extern const char *str_22k[];
extern const char *str_local_oscilator[];

#endif // LANGUAGE_H

// Reference(s):
//   @1: A: B - C (D)
