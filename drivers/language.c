// Description of file, etc. See "language.h".

#include "language.h"
#include "language_data.h"

#include "eeprom.h"

static inline uint8_t get_language_idx(void);
static inline const char *get_str_gen(const char *str[], uint32_t size, uint32_t item_idx);
static inline const char *get_str_gen_1(const char *str[]);

const char *get_str_menu(idx_menu_t item_idx)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_menu[language_idx * 8 + item_idx];
  //return get_str_gen(str_menu, 8, item_idx);
}

const char *get_str_sat_setting(idx_sat_setting_t item_idx)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_sat_setting[language_idx * 13 + item_idx];
  //return get_str_gen(str_sat_setting, 13, item_idx);
}

const char *get_str_satellite(idx_satellite_t item_idx)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_satellite[language_idx * 4 + item_idx];
  //return get_str_gen(str_satellite, 4, item_idx);
}

const char *get_str_tools(idx_tools_t item_idx)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_tools[language_idx * 7 + item_idx];
  //return get_str_gen(str_tools, 7, item_idx);
}

const char *get_str_sat_manager(idx_sat_manager_t item_idx)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_sat_manager[language_idx * 4 + item_idx];
  //return get_str_gen(str_sat_manager, 4, item_idx);
}

const char *get_str_tp_manager(idx_tp_manager_t item_idx)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_tp_manager[language_idx * 4 + item_idx];
  //return get_str_gen(str_tp_manager, 4, item_idx);
}

const char *get_str_system_setup(idx_system_setup_t item_idx)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_system_setup[language_idx * 7 + item_idx];
  //return get_str_gen(str_system_setup, 7, item_idx);
}

const char *get_str_on_off(idx_on_off_t item_idx)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_on_off[language_idx * 2 + item_idx];
  //return get_str_gen(str_on_off, 2, item_idx);
}

const char *get_str_setting_changed(idx_setting_changed_t item_idx)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_setting_changed[language_idx * 2 + item_idx];
  //return get_str_gen(str_setting_changed, 2, item_idx);
}

const char *get_str_del(void)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_del[language_idx];
  //return get_str_gen_1(str_del);
}

const char *get_str_message(void)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_message[language_idx];
  //return get_str_gen_1(str_message);
}

const char *get_str_language(void)
{
  uint8_t language_idx;

  language_idx = get_language_idx();
  return str_language[language_idx];
  //return get_str_gen_1(str_language);
}

static inline uint8_t get_language_idx(void)
{
  return eeprom_10h_8.language_idx;
}

static inline const char *get_str_gen(const char *str[], uint32_t size, uint32_t item_idx)
{
  uint8_t language_idx;

  language_idx = get_language_idx();

  return str[language_idx * size + item_idx];
}

static inline const char *get_str_gen_1(const char *str[])
{
  uint8_t language_idx;

  language_idx = get_language_idx();

  return str[language_idx];
}

