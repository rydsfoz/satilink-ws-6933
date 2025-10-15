// Description of file, etc. See "eeprom.h".

#include "eeprom.h"

#include <stdio.h> // sprintf() (EEPROM_WRITE_FAKE)
#include <string.h>
#include "init.h"
#include "delay.h"
#include "lnb.h"
#include "i2c_soft.h"
#include "serial.h" // usart1_write_str() (EEPROM_WRITE_FAKE)
#include "language_data.h"
#include "font.h"
#include "windows.h"

//#define EEPROM_WRITE_FAKE
#define PRINT_USART

/*
eeprom: 32768 B (32 kB)
16 primeiros nao sao escritos (quando recebe dados pra preencher eeprom, excecao os 4 ultimos, "Password")
*/
#define ADDR_CODEWORD       0x000C // eeprom_codeword
#define ADDR_EEPROM_10H_8   0x0010 // eeprom_10h_8
#define ADDR_EEPROM_30H_43  0x0030 // eeprom_30h_43
#define ADDR_EEPROM_5DAH_41 0x05DA // eeprom_5dah_41

#define CODEWORD_SIZE       4  // eeprom_codeword
#define EEPROM_10H_8_SIZE   8  // eeprom_10h_8
#define EEPROM_30H_43_SIZE  43 // eeprom_30h_43
#define EEPROM_5DAH_41_SIZE 41 // eeprom_5dah_41

static void eeprom_addr(uint32_t addr);
static void crc32_tab_init(void);

static uint32_t crc32_tab_verify = 1;
static uint32_t crc32_tab[256];

uint32_t eeprom_codeword; // Password
eeprom_8_t eeprom_10h_8;
eeprom_43_t eeprom_30h_43;
eeprom_41_t eeprom_5dah_41;

static eeprom_8_t eeprom_10h_8_cpy;
static eeprom_43_t eeprom_30h_43_cpy;
static eeprom_41_t eeprom_5dah_41_cpy;

void eeprom_write(uint32_t addr, const uint8_t *data, uint32_t size)
{
#ifdef EEPROM_WRITE_FAKE
  #ifdef PRINT_USART
  char str[81];

  sprintf(str, "eeprom_write [addr: %04X] [size: %d]\n", addr, size);
  usart1_write_str(str);
  #endif
#else
  uint8_t check;

  eeprom_addr(addr);
  while (check = size != 0, --size, check) {
    i2c2_send(*data);
    data++;
    addr++;
    if (((addr & 0x3F) == 0) && (size != 0)) {
      i2c2_stop();
      delay_ms(20);
      eeprom_addr(addr);
    }
  }
  i2c2_stop();
  delay_ms(20);
#endif
}

void eeprom_read(uint32_t addr, uint8_t *data, uint32_t size)
{
  uint8_t check;

  eeprom_addr(addr);
  i2c2_start();
  i2c2_send(0xA1); // (dev_addr << 1) | R
  while (check = size != 0, --size, check) {
    *data = i2c2_receive();
    data++;
    if (size == 0)
      i2c2_nack(1); // nack
    else
      i2c2_nack(0); // ack
  }
  i2c2_stop();
}

void eeprom_codeword_write(const uint8_t *data)
{
  eeprom_write(ADDR_CODEWORD, data, CODEWORD_SIZE);
}

void eeprom_codeword_read(uint8_t *data)
{
  eeprom_read(ADDR_CODEWORD, data, CODEWORD_SIZE);
}

void eeprom_codeword_verify(void)
{
  static uint32_t code_s, code_s2;
  uint32_t crc32, codeword;

  crc32 = crc_32(0, s_n, 8);
  code_s2 = crc32;
  crc32 = crc_32(0x12345678, s_n, 8);
  code_s = crc32;
  eeprom_codeword_read((uint8_t *)&codeword);
  if (codeword == code_s2) {
    eeprom_codeword = codeword;
    dvb_s = DVB_S2;
  } else if (codeword == code_s) {
    eeprom_codeword = codeword;
    dvb_s = DVB_S;
  } else {
    eeprom_codeword = 0;
    dvb_s = DVB_NONE;
  }
}

void eeprom_write_10_8(void)
{
  if (memcmp(&eeprom_10h_8_cpy, &eeprom_10h_8, EEPROM_10H_8_SIZE) != 0)
    eeprom_write(ADDR_EEPROM_10H_8, (uint8_t *)&eeprom_10h_8, EEPROM_10H_8_SIZE);
}

void eeprom_read_10_8(void)
{
  eeprom_read(ADDR_EEPROM_10H_8, (uint8_t *)&eeprom_10h_8, EEPROM_10H_8_SIZE);
  memcpy(&eeprom_10h_8_cpy, &eeprom_10h_8, EEPROM_10H_8_SIZE);

  if ((eeprom_10h_8.key_tone != ON) && (eeprom_10h_8.key_tone != OFF))
    eeprom_10h_8.key_tone = ON;
  if ((eeprom_10h_8.lock_tone != ON) && (eeprom_10h_8.lock_tone != OFF))
    eeprom_10h_8.lock_tone = ON;
  if ((eeprom_10h_8.flashlight != ON) && (eeprom_10h_8.flashlight != OFF))
    eeprom_10h_8.flashlight = OFF;
  if (((eeprom_10h_8.lnb_power != PWR_OFF) && (eeprom_10h_8.lnb_power != PWR_13V)) &&
    (eeprom_10h_8.lnb_power != PWR_18V))
    eeprom_10h_8.lnb_power = PWR_13V_18V;
  if (DBM < eeprom_10h_8.pwr_unit)
    eeprom_10h_8.pwr_unit = DBUV;
  if ((99 < eeprom_10h_8.sat_idx) || (127 < eeprom_10h_8.tp_idx)) {
    eeprom_10h_8.sat_idx = 0;
    eeprom_10h_8.tp_idx = 0;
  }
  if (15 < eeprom_10h_8.language_idx)
    eeprom_10h_8.language_idx = 0;

  font_language_set(eeprom_10h_8.language_idx);
  flashlight_on_off();
  lnb_power_update();
}

void eeprom_write_30_43(void)
{
  if (memcmp(&eeprom_30h_43_cpy, &eeprom_30h_43, EEPROM_30H_43_SIZE) != 0) {
    eeprom_30h_43.version = 8;
    eeprom_write(ADDR_EEPROM_30H_43, (uint8_t *)&eeprom_30h_43, EEPROM_30H_43_SIZE);
  }
}

void eeprom_read_write_30_43(void)
{
  int i;

  eeprom_read(ADDR_EEPROM_30H_43, (uint8_t *)&eeprom_30h_43, EEPROM_30H_43_SIZE);
  memcpy(&eeprom_30h_43_cpy, &eeprom_30h_43, EEPROM_30H_43_SIZE);

  if ((((eeprom_30h_43.diseqc_mode != 0) && (eeprom_30h_43.diseqc_mode != 1)) ||
    (7 < eeprom_30h_43.idx_if_channel)) || (eeprom_30h_43.version != 8)) {
    eeprom_30h_43.diseqc_mode = 0;
    eeprom_30h_43.idx_if_channel = 0;
    for (i = 0; i < 8; i++)
      strcpy(&eeprom_30h_43.str_center_frequency[i][0], str_center_frequency[i]);
    eeprom_write_30_43();
  }
}

void eeprom_write_5da_41(void)
{
  if (memcmp(&eeprom_5dah_41_cpy, &eeprom_5dah_41, EEPROM_5DAH_41_SIZE) != 0)
    eeprom_write(ADDR_EEPROM_5DAH_41, (uint8_t *)&eeprom_5dah_41, EEPROM_5DAH_41_SIZE);
}

void eeprom_read_5da_41(void)
{
  static eeprom_41_t eeprom_5dah_41_default = {
    .sat_longitude       = "138.0",
    .sat_lon_direction   = 'E',
    .local_longitude     = "141.2",
    .local_lon_direction = 'E',
    .local_latitude      = "12.9",
    .local_lat_direction = 'N',
    .elevation           = "74.4",
    .azimuth             = "194.1",
    .polarization        = "-13.7"
  };

  eeprom_read(ADDR_EEPROM_5DAH_41, (uint8_t *)&eeprom_5dah_41, EEPROM_5DAH_41_SIZE);
  if ((eeprom_5dah_41.sat_longitude[3] != '.') || (eeprom_5dah_41.local_longitude[3] != '.'))
    memcpy(&eeprom_5dah_41, &eeprom_5dah_41_default, EEPROM_5DAH_41_SIZE);
  memcpy(&eeprom_5dah_41_cpy, &eeprom_5dah_41, EEPROM_5DAH_41_SIZE);
}

uint32_t crc_32(uint32_t crc, const uint8_t *data, uint32_t size)
{
  if (crc32_tab_verify != 0)
    crc32_tab_init();

  crc = ~crc; // crc = crc ^ 0xFFFFFFFF
  while (7 < size) {
    crc = crc32_tab[(data[0] ^ crc) & 0xFF] ^ crc >> 8;
    crc = crc32_tab[(data[1] ^ crc) & 0xFF] ^ crc >> 8;
    crc = crc32_tab[(data[2] ^ crc) & 0xFF] ^ crc >> 8;
    crc = crc32_tab[(data[3] ^ crc) & 0xFF] ^ crc >> 8;
    crc = crc32_tab[(data[4] ^ crc) & 0xFF] ^ crc >> 8;
    crc = crc32_tab[(data[5] ^ crc) & 0xFF] ^ crc >> 8;
    crc = crc32_tab[(data[6] ^ crc) & 0xFF] ^ crc >> 8;
    crc = crc32_tab[(data[7] ^ crc) & 0xFF] ^ crc >> 8;
    data += 8;
    size -= 8;
  }
  while (size != 0) {
    crc = crc32_tab[(*data ^ crc) & 0xFF] ^ crc >> 8;
    size--;
    data++;
  }

  // NOTE: Algoritmo pesquisado termina com: crc ^ 0xFFFFFFFF
  return ~(crc + 1);
}

void sat_lon_dir_update(btn_t btn)
{
  if ((btn == BTN_LEFT) || (btn == BTN_RIGHT)) {
    if (eeprom_5dah_41.sat_lon_direction == 'E')
      eeprom_5dah_41.sat_lon_direction = 'W';
    else
      eeprom_5dah_41.sat_lon_direction = 'E';
  }
}

void local_lon_dir_update(btn_t btn)
{
  if ((btn == BTN_LEFT) || (btn == BTN_RIGHT)) {
    if (eeprom_5dah_41.local_lon_direction == 'E')
      eeprom_5dah_41.local_lon_direction = 'W';
    else
      eeprom_5dah_41.local_lon_direction = 'E';
  }
}

void local_lat_dir_update(btn_t btn)
{
  if ((btn == BTN_LEFT) || (btn == BTN_RIGHT)) {
    if (eeprom_5dah_41.local_lat_direction == 'S')
      eeprom_5dah_41.local_lat_direction = 'N';
    else
      eeprom_5dah_41.local_lat_direction = 'S';
  }
}

void language_update(btn_t btn)
{
  if (btn == BTN_LEFT) {
    if (eeprom_10h_8.language_idx == 0)
      eeprom_10h_8.language_idx = 15;
    else
      eeprom_10h_8.language_idx--;
  } else if (btn == BTN_RIGHT) {
    eeprom_10h_8.language_idx++;
    if (15 < eeprom_10h_8.language_idx)
      eeprom_10h_8.language_idx = 0;
  }
  font_language_set(eeprom_10h_8.language_idx);
}

void key_tone_update(btn_t btn)
{
  if ((btn == BTN_LEFT) || (btn == BTN_RIGHT)) {
    if (eeprom_10h_8.key_tone == ON)
      eeprom_10h_8.key_tone = OFF;
    else
      eeprom_10h_8.key_tone = ON;
  }
}

void lock_tone_update(btn_t btn)
{
  if ((btn == BTN_LEFT) || (btn == BTN_RIGHT)) {
    if (eeprom_10h_8.lock_tone == ON)
      eeprom_10h_8.lock_tone = OFF;
    else
      eeprom_10h_8.lock_tone = ON;
  }
}

void pwr_update(btn_t btn)
{
  if (btn == BTN_LEFT) {
    if (eeprom_10h_8.pwr_unit == 0)
      eeprom_10h_8.pwr_unit = 2;
    else
      eeprom_10h_8.pwr_unit--;
  } else if (btn == BTN_RIGHT) {
    eeprom_10h_8.pwr_unit++;
    if (2 < eeprom_10h_8.pwr_unit)
      eeprom_10h_8.pwr_unit = 0;
  }
}

void flashlight_update(btn_t btn)
{
  if (((btn == BTN_LEFT) || (btn == BTN_RIGHT)) || (btn == BTN_FLASHLIGHT)) {
    if (eeprom_10h_8.flashlight == ON)
      eeprom_10h_8.flashlight = OFF;
    else
      eeprom_10h_8.flashlight = ON;
  }
  flashlight_on_off();
}

void flashlight_on_off(void)
{
  if (eeprom_10h_8.flashlight == ON)
    FLASHLIGHT_ON();
  else
    FLASHLIGHT_OFF();
}

static void eeprom_addr(uint32_t addr)
{
  i2c2_start();
  i2c2_send(0xA0); // (dev_addr << 1) | W
  i2c2_send((addr >> 8) & 0xFF); // addr high
  i2c2_send(addr);               // addr low
}

//#define GEN_POLY
static void crc32_tab_init(void)
{
  int i, j;
  uint32_t crc32_poly_rev, data;
#ifdef GEN_POLY
  static uint8_t gen_poly[] = {
    0x00, 0x01, 0x02, 0x04, 0x05, 0x07, 0x08, 0x0A, 0x0B, 0x0C, 0x10, 0x16, 0x17, 0x1A
  };
#endif

#ifdef GEN_POLY
  crc32_poly_rev = 0;
  for (i = 0; i < 14; i++)
    crc32_poly_rev = crc32_poly_rev | 1 << 31 - (uint32_t)gen_poly[i];
#else
  crc32_poly_rev = 0xEDB88320;
#endif

  for (i = 0; i < 256; i++) {
    data = i;
    for (j = 0; j < 8; j++) {
      if (data & 1)
        data = crc32_poly_rev ^ data >> 1;
      else
        data = data >> 1;
    }
    crc32_tab[i] = data;
  }
  crc32_tab_verify = 0;
}
