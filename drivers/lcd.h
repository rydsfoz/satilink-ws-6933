// lcd driver (...)
//
// TODO: Explicacao/descricao (...)

// This program is open source. For license terms, see the LICENSE file.
// Licensed under the Academic Free License version 3.0.

// Author: Rosiney D. da Silva, 2020-2024.

#ifndef LCD_H
#define LCD_H

#include <inttypes.h>

#define LCD_PIXEL_WIDTH  ((uint16_t)320)
#define LCD_PIXEL_HEIGHT ((uint16_t)240)

// Brightness Control
// 0 0 BCTRL 0 DD BL 0 0
// BCTRL: Brightness Control Block On/Off, This bit is always used to switch brightness for display.
#define LCD_BRIGHT_BCTRL_OFF (0 << 5) // Brightness Control Block Off
#define LCD_BRIGHT_BCTRL_ON  (1 << 5) // Brightness Control Block On
// DD: Display Dimming (Only for manual brightness setting)
#define LCD_BRIGHT_DD_OFF    (0 << 3) // Display Dimming Off
#define LCD_BRIGHT_DD_ON     (1 << 3) // Display Dimming On
// BL: Backlight Control On/Off
#define LCD_BRIGHT_BL_OFF    (0 << 2) // Backlight Control Off
#define LCD_BRIGHT_BL_ON     (1 << 2) // Backlight Control On
// Dimming function is adapted to the brightness registers for display when bit BCTRL is changed at DD = 1.
// When BL bit changed from 'on' to 'off', backlight is turned off without gradual dimming, even if
//   dimming-on (DD = 1) are selected.

void lcd_init(void);
void lcd_software_reset(void);
uint32_t lcd_read_id(void);
void lcd_sleep_in(void);
void lcd_sleep_out(void);
void lcd_partial_on(void);
void lcd_normal_on(void);
void lcd_inversion_off(void);
void lcd_inversion_on(void);
void lcd_display_off(void);
void lcd_display_on(void);
void lcd_display_area_set(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye);
void lcd_display_start_pos_set(uint16_t x, uint16_t y);
void lcd_partial_area(uint16_t ys, uint16_t ye);
void lcd_idle_off(void);
void lcd_idle_on(void);
void lcd_write_brightness(uint8_t brightness);
void lcd_brightness_control(uint8_t bctrl, uint8_t dd, uint8_t bl);

void lcd_draw_pixel_begin(void);
void lcd_draw_pixel_next(uint32_t color);
void lcd_draw_pixel_end(void);
void lcd_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
void lcd_write_color_n(uint16_t color, uint32_t n);

extern uint8_t image_logo[];

#endif // LCD_H

// Reference(s):
//   @1: Sitronix ST7789V: 240RGBx320 dot 262K Color with Frame Memory Single-Chip
//       TFT Controller/Driver - Datasheet (Version 1.6, 2017/09)
//   @2: ILITEK ILI9341: a-Si TFT LCD Single Chip Driver 240RGBx320 Resolution and
//       262K color - Datasheet (Version: V1.11)
