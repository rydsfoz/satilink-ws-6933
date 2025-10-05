// Description of file, etc. See "lcd.h".

#include "lcd.h"

#include "init.h"
#include "delay.h"

// LCD_CS: Chip select input pin ("Low" enable).
// This pin can be permanently fixed "Low" in MPU interface mode only.
#define MPU_INTERFACE_MODE_ONLY

// Commands

// *: used
#define LCD_NOP                0x00 // No Operation
#define LCD_SW_RESET           0x01 // Software Reset *
#define LCD_READ_ID            0x04 // Read Display Identification Information *
#define LCD_READ_STATUS        0x09 // Read Display Status
#define LCD_READ_PW_MODE       0x0A // Read Display Power Mode
#define LCD_READ_MACTRL        0x0B // Read Display 'Memory Data Access Control' (MACTRL)
#define LCD_READ_PIXEL_FORMAT  0x0C // Read Display Pixel Format
#define LCD_READ_IMAGE_MODE    0x0D // Read Display Image Mode
#define LCD_READ_SIGNAL_MODE   0x0E // Read Display Signal Mode
#define LCD_READ_SELF_DIAG     0x0F // Read Display Self-Diagnostic Result
#define LCD_SLEEP_IN           0x10 // Sleep in *
#define LCD_SLEEP_OUT          0x11 // Sleep Out *
#define LCD_PARTIAL_ON         0x12 // Partial Display Mode On *
#define LCD_NORMAL_ON          0x13 // Normal Display Mode On *
#define LCD_INVERSION_OFF      0x20 // Display Inversion Off *
#define LCD_INVERSION_ON       0x21 // Display Inversion On *
#define LCD_GAMMA_SET          0x26 // Gamma Set
#define LCD_DISPLAY_OFF        0x28 // Display Off *
#define LCD_DISPLAY_ON         0x29 // Display On *
#define LCD_COLUMN_ADDR_SET    0x2A // Column Address Set *
#define LCD_ROW_ADDR_SET       0x2B // Row Address Set *
#define LCD_MEMORY_WRITE       0x2C // Memory Write *
#define LCD_MEMORY_READ        0x2E // Memory Read
#define LCD_PARTIAL_AREA       0x30 // Partial Area *
#define LCD_VERT_SCROL_DEF     0x33 // Vertical Scrolling Definition
#define LCD_TEARING_OFF        0x34 // Tearing Effect Line Off
#define LCD_TEARING_ON         0x35 // Tearing Effect Line On
#define LCD_MEMORY_ACCESS_CTRL 0x36 // Memory Data Access Control (MACTRL) *
#define LCD_VERT_SCROL_START   0x37 // Vertical Scroll Start Address of RAM
#define LCD_IDLE_OFF           0x38 // Idle Mode Off *
#define LCD_IDLE_ON            0x39 // Idle Mode On *
#define LCD_PIXEL_FORMAT_SET   0x3A // Interface Pixel Format *
#define LCD_WRITE_MEMORY_CONT  0x3C // Write Memory Continue *
#define LCD_READ_MEMORY_CONT   0x3E // Read Memory Continue
#define LCD_SET_TEAR_SCANLINE  0x44 // Set Tear Scanline
#define LCD_GET_SCANLINE       0x45 // Get Scanline
#define LCD_WRITE_BRIGHT       0x51 // Write Display Brightness *
#define LCD_READ_BRIGHT        0x52 // Read Display Brightness Value
#define LCD_WRITE_CTRL_DISP    0x53 // Write CTRL(Control) Display *
#define LCD_READ_CTRL_DISP     0x54 // Read CTRL(Control) Value Display
#define LCD_WRITE_ABRIGHT_CTRL 0x55 // Write Content Adaptive Brightness Control(CABC) and Color Enhancement *
#define LCD_READ_CONTENT_ADAP  0x56 // Read Content Adaptive Brightness Control(CABC)
#define LCD_WRITE_CABC_MINIMUM 0x5E // Write CABC Minimum Brightness
#define LCD_READ_CABC_MINIMUM  0x5F // Read CABC Minimum Brightness
#define LCD_READ_BRIGHT_DIAG   0x68 // Read Automatic Brightness Control Self-Diagnostic Result
#define LCD_READ_ID1           0xDA // Read ID1
#define LCD_READ_ID2           0xDB // Read ID2
#define LCD_READ_ID3           0xDC // Read ID3

// Write Content Adaptive Brightness Control(CABC) and Color Enhancement
// LCD_WRITE_ABRIGHT_CTRL: CECTRL  0  CE1  CE0  0  0  C1  C0
//
// This command is used to set parameters for image content based adaptive brightness control functionality
//   and Color Enhancement function.
//
// CECTRL: Color Enhancement Control Bit:
#define LCD_ABRIGHT_CECTRL_OFF (0 << 7) // 0: Color Enhancement Off.
#define LCD_ABRIGHT_CECTRL_ON  (1 << 7) // 1: Color Enhancement On.
// There are three color enhancement levels can be set.
//                                           CE1  CE0  Color enhancement level
#define LCD_ABRIGHT_LEVEL_LOW    (0 << 4) // 0    0    Low enhancement
#define LCD_ABRIGHT_LEVEL_MEDIUM (1 << 4) // 0    1    Medium enhancement
#define LCD_ABRIGHT_LEVEL_HIGH   (3 << 4) // 1    1    High enhancement
//                                                  C1  C0  Function
#define LCD_ABRIGHT_MODE_OFF            (0 << 0) // 0   0   Off
#define LCD_ABRIGHT_MODE_USER_INTERFACE (1 << 0) // 0   1   User Interface Mode
#define LCD_ABRIGHT_MODE_STILL_PICTURE  (2 << 0) // 1   0   Still Picture
#define LCD_ABRIGHT_MODE_MOVING_IMAGE   (3 << 0) // 1   1   Moving Image

// Interface Pixel Format
// LCD_PIXEL_FORMAT_SET: 0  D6  D5  D4  0  D2  D1  D0
//
// D6 D5 D4: RGB interface color format:
#define LCD_PIXEL_FORMAT_RGB_65K  (5 << 4) // '101' = 65 K of RGB interface
#define LCD_PIXEL_FORMAT_RGB_262K (6 << 4) // '110' = 262 K of RGB interface
// D2 D1 D0: Control interface color format:
#define LCD_PIXEL_FORMAT_BP_12  (3 << 0) // '011' = 12 bit/pixel
#define LCD_PIXEL_FORMAT_BP_16  (5 << 0) // '101' = 16 bit/pixel
#define LCD_PIXEL_FORMAT_BP_18  (6 << 0) // '110' = 18 bit/pixel
#define LCD_PIXEL_FORMAT_BP_16M (7 << 0) // '111' = 16 M truncated
// Note1: In 12-bit/Pixel, 16-bit/Pixel or 18-bit/Pixel mode, the LUT is applied to transfer data into the Frame Memory.
// Note2: The Command 3Ah should be set at 55h when writing 16-bit/pixel data into frame memory, but 3Ah should be
//   re-set to 66h when reading pixel data from frame memory.

// Memory Data Access Control (MACTRL)
// LCD_MEMORY_ACCESS_CTRL: MY  MX  MV  ML  RGB MH  -   -
//
// This command defines read/write scanning direction of frame memory.
//
// MY, MX, MV: These 3 bits control MCU to memory write/read direction.
// MY - Row Address Order
#define LCD_MACTRL_MY_TOP_BOTTOM (0 << 7) // 0: Top to Bottom
#define LCD_MACTRL_MY_BOTTOM_TOP (1 << 7) // 1: Bottom to Top
// MX - Column Address Order
#define LCD_MACTRL_MX_LEFT_RIGHT (0 << 6) // 0: Left to Right
#define LCD_MACTRL_MX_RIGHT_LEFT (1 << 6) // 1: Right to Left
// MV - Row/Column Order
#define LCD_MACTRL_MV_NORMAL     (0 << 5) // 0: Normal Mode
#define LCD_MACTRL_MV_REVERSE    (1 << 5) // 1: Reverse Mode
// ML - Vertical Refresh Order
#define LCD_MACTRL_ML_TOP_BOTTOM (0 << 4) // 0: LCD Refresh Top to Bottom
#define LCD_MACTRL_ML_BOTTOM_TOP (1 << 4) // 1: LCD Refresh Bottom to Top
// RGB - RGB/BGR Order - Color selector switch control
#define LCD_MACTRL_COLOR_RGB (0 << 3) // 0: RGB color filter panel
#define LCD_MACTRL_COLOR_BGR (1 << 3) // 1: BGR color filter panel
// MH - Horizontal Refresh Order
#define LCD_MACTRL_MH_LEFT_RIGHT (0 << 2) // 0: LCD Refresh Left to Right
#define LCD_MACTRL_MH_RIGHT_LEFT (1 << 2) // 1: LCD Refresh Right to Left
// Default
#define LCD_MACTRL_MYXV_DEFAULT (LCD_MACTRL_MY_TOP_BOTTOM | LCD_MACTRL_MX_LEFT_RIGHT | LCD_MACTRL_MV_NORMAL)
#define LCD_MACTRL_MLH_DEFAULT (LCD_MACTRL_ML_TOP_BOTTOM | LCD_MACTRL_MH_LEFT_RIGHT)
// ML, MH: Default; MY, MX, MV:
// Rotated    0 degrees: Top to Bottom, Left to Right, Normal Mode
// Rotated  +90 degrees: Bottom to Top, Left to Right, Reverse Mode
// Rotated +180 degrees: Bottom to Top, Right to Left, Normal Mode
// Rotated +270 degrees: Top to Bottom, Right to Left, Reverse Mode

static inline void lcd_hw_reset(void);
static inline void lcd_column_addr_set(uint16_t xs, uint16_t xe);
static inline void lcd_column_addr_xs_set(uint16_t xs);
static inline void lcd_row_addr_set(uint16_t ys, uint16_t ye);
static inline void lcd_row_addr_ys_set(uint16_t ys);
static inline void lcd_write_bus(uint16_t data);
static inline uint16_t lcd_read_bus(void);
static inline void lcd_write_cmd(uint16_t data);
static inline void lcd_write_data(uint16_t data);
static inline void lcd_write_data_n(const uint16_t *data, uint32_t n);
static inline void lcd_read_data8_n(uint8_t *data, uint32_t n);
static inline void lcd_write_data_rep_n(uint16_t data, int32_t n);

uint8_t image_logo[4113]; // 4113 = 4096 + 17

void lcd_init(void)
{
  lcd_hw_reset();

#ifdef MPU_INTERFACE_MODE_ONLY
  LCD_CS_ON();
#endif // MPU_INTERFACE_MODE_ONLY

  lcd_sleep_out();

  lcd_write_cmd(LCD_WRITE_ABRIGHT_CTRL);
  lcd_write_data(LCD_ABRIGHT_CECTRL_ON | LCD_ABRIGHT_LEVEL_MEDIUM | LCD_ABRIGHT_MODE_OFF);

  lcd_write_cmd(LCD_PIXEL_FORMAT_SET);
  lcd_write_data(LCD_PIXEL_FORMAT_RGB_65K | LCD_PIXEL_FORMAT_BP_16);

  lcd_write_cmd(LCD_MEMORY_ACCESS_CTRL);
  // Rotated +90 degrees
  lcd_write_data(LCD_MACTRL_MY_BOTTOM_TOP | LCD_MACTRL_MX_LEFT_RIGHT | LCD_MACTRL_MV_REVERSE |
                LCD_MACTRL_MLH_DEFAULT | LCD_MACTRL_COLOR_RGB);

  lcd_display_area_set(0, 0, LCD_PIXEL_WIDTH - 1, LCD_PIXEL_HEIGHT - 1);
}

void lcd_software_reset(void)
{
  // If software reset is sent during sleep in mode, it will be necessary to
  // wait 120 ms before sending sleep out command.
  // Software reset command cannot be sent during sleep out sequence.
  lcd_write_cmd(LCD_SW_RESET);
  //delay_ms(120);
}

uint32_t lcd_read_id(void)
{
  uint32_t ret;

  // This read byte returns 24-bit display identification information.
  // The 1st parameter is dummy data
  // The 2nd parameter: LCD module's manufacturer ID.
  // The 3rd parameter: LCD module/driver version ID
  // The 4th parameter: LCD module/driver ID.

  // Status            Default Value
  //                   ID1  ID2  ID3
  // Power On Sequence 0x85 0x85 0x52
  // S/W Reset         0x85 0x85 0x52
  // H/W Reset         0x85 0x85 0x52
  lcd_write_cmd(LCD_READ_ID);
  lcd_read_data8_n((uint8_t *)&ret, 4);

  // ID1 | (ID2 << 8) | (ID3 << 16)
  return ret & 0xFFFFFF00;
}

void lcd_sleep_in(void)
{
  // This command causes the LCD module to enter the minimum power consumption mode.
  // In this mode the DC/DC converter is stopped, internal oscillator is stopped, and panel scanning is stopped.
  // MCU interface and memory are still working and the memory keeps its contents.

  // It will be necessary to wait 5 ms before sending any new commands to a display module following
  //   this command to allow time for the supply voltages and clock circuits to stabilize.
  // It will be necessary to wait 120 ms after sending sleep out command (when in sleep in mode)
  //   before sending an sleep in command.
  lcd_write_cmd(LCD_SLEEP_IN);
  //delay_ms(120);
}

void lcd_sleep_out(void)
{
  // This command turn off sleep mode.

  // It will be necessary to wait 5 ms before sending any new commands to a display module following
  //   this command to allow time for the supply voltages and clock circuits to stabilize.
  // It will be necessary to wait 120 ms after sending sleep out command (when in sleep in mode)
  //   before sending an sleep in command.
  // The display module runs the self-diagnostic functions after this command is received.
  lcd_write_cmd(LCD_SLEEP_OUT);
  //delay_ms(120);
  delay_ms(12);
}

void lcd_partial_on(void)
{
  // This command turns on Partial mode. The partial mode window is described by
  //   the Partial Area command.
  // To leave Partial mode, the Normal Display Mode On command should be written.
  lcd_write_cmd(LCD_PARTIAL_ON);
}

void lcd_normal_on(void)
{
  // This command turns the display to normal mode.
  // Normal display mode on means partial mode off.
  // Exit from 'LCD_NORMAL_ON' by the partial mode on command.
  lcd_write_cmd(LCD_NORMAL_ON);
}

void lcd_inversion_off(void)
{
  // This command is used to recover from display inversion mode.
  lcd_write_cmd(LCD_INVERSION_OFF);
}

void lcd_inversion_on(void)
{
  // This command is used to display inversion mode.
  lcd_write_cmd(LCD_INVERSION_ON);
}

void lcd_display_off(void)
{
  // This command is used to enter into DISPLAY OFF mode. In this mode, the output from Frame Memory
  //   is disabled and blank page inserted.
  // This command makes no change of contents of frame memory.
  // This command does not change any other status.
  // There will be no abnormal visible effect on the display.
  // Exit from this command by Display On.
  lcd_write_cmd(LCD_DISPLAY_OFF);
}

void lcd_display_on(void)
{
  // This command is used to recover from DISPLAY OFF mode.
  // Output from the Frame Memory is enabled.
  // This command makes no change of contents of frame memory.
  // This command does not change any other status.
  lcd_write_cmd(LCD_DISPLAY_ON);
}

// xs: x-start; ys: y-start; xe: x-end; ye: y-end
void lcd_display_area_set(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye)
{
  lcd_column_addr_set(xs, xe);
  lcd_row_addr_set(ys, ye);
}

void lcd_display_start_pos_set(uint16_t x, uint16_t y) // x: x-start; y: y-start
{
  lcd_column_addr_xs_set(x);
  lcd_row_addr_ys_set(y);
}

void lcd_partial_area(uint16_t ys, uint16_t ye) // ys: y-start; ye: y-end
{
  // This command defines the partial mode's display area.
  // 'Start Row' (ys) and 'End Row' (ye) refer to the Frame Memory row address counter.
  // If 'End Row' > 'Start Row': Partial display area [ys..ye]
  // If 'End Row' < 'Start Row', when MACTRL ML = '0': Non-display area [ye..ys]
  lcd_write_cmd(LCD_PARTIAL_AREA);
  lcd_write_data(ys >> 8);
  lcd_write_data(ys);
  lcd_write_data(ye >> 8);
  lcd_write_data(ye);
}

void lcd_idle_off(void)
{
  // This command is used to recover from Idle mode on.
  // In the idle off mode,
  //   1. LCD can display 4096, 65k or 262k colors.
  //   2. Normal frame frequency is applied
  lcd_write_cmd(LCD_IDLE_OFF);
}

void lcd_idle_on(void)
{
  // This command is used to enter into Idle mode on.
  // There will be no abnormal visible effect on the display mode change transition.
  // In the idle on mode,
  //   1. Color expression is reduced. The primary and the secondary colors using MSB
  //      of each R,G and B in the Frame Memory, 8 color depth data is displayed.
  //   2. 8-Color mode frame frequency is applied.
  //   3. Exit from "Idle Mode On" by "Idle Mode Off" command.
  lcd_write_cmd(LCD_IDLE_ON);
}

void lcd_write_brightness(uint8_t brightness)
{
  // This command is used to adjust the brightness value of the display.
  lcd_write_cmd(LCD_WRITE_BRIGHT);
  lcd_write_data(brightness);
}

void lcd_brightness_control(uint8_t bctrl, uint8_t dd, uint8_t bl)
{
  // This command is used to control display brightness.
  lcd_write_cmd(LCD_WRITE_CTRL_DISP);
  lcd_write_data(bctrl | dd | bl);
}

void lcd_draw_pixel_begin(void)
{
  lcd_write_cmd(LCD_MEMORY_WRITE);
#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_ON();
#endif // ! MPU_INTERFACE_MODE_ONLY
}

void lcd_draw_pixel_next(uint32_t color)
{
  lcd_write_bus(color);
}

void lcd_draw_pixel_end(void)
{
#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_OFF();
#endif // ! MPU_INTERFACE_MODE_ONLY
}

void lcd_draw_pixel(uint16_t x, uint16_t y, uint32_t color)
{
  lcd_display_start_pos_set(x, y);

  lcd_write_cmd(LCD_MEMORY_WRITE);
  lcd_write_data(color);
}

void lcd_write_color_n(uint16_t color, uint32_t n)
{
  lcd_write_cmd(LCD_MEMORY_WRITE);
  lcd_write_data_rep_n(color, n);
}

static inline void lcd_hw_reset(void)
{
  //LCD_RST_OFF();
  delay_ms(10);
  LCD_RST_ON();
  delay_ms(10); // 10 us
  LCD_RST_OFF();
  delay_ms(20); // 120 ms
  // Reset Timing
  // 0 tRW - Reset pulse duration - min  10 us
  // 1 tRT - Reset cancel         - max 120 ms
}

static inline void lcd_column_addr_set(uint16_t xs, uint16_t xe)
{
  // This command is used to define area of frame memory where MCU can access.
  // The values of 'xs' and 'xe' are referred when 'LCD_MEMORY_WRITE' command comes.
  // Each value represents one column line in the Frame Memory.
  //
  // 'xs' always must be equal to or less than 'xe'.
  // When 'xs' or 'xe' is greater than maximum address, data of out of range will be ignored.
  lcd_write_cmd(LCD_COLUMN_ADDR_SET);
  lcd_write_data(xs >> 8);
  lcd_write_data(xs);
  lcd_write_data(xe >> 8);
  lcd_write_data(xe);
}

static inline void lcd_column_addr_xs_set(uint16_t xs)
{
  // This command is used to define area of frame memory where MCU can access.
  // The values of 'xs' and 'xe' are referred when 'LCD_MEMORY_WRITE' command comes.
  // Each value represents one column line in the Frame Memory.
  //
  // 'xs' always must be equal to or less than 'xe'.
  // When 'xs' or 'xe' is greater than maximum address, data of out of range will be ignored.
  lcd_write_cmd(LCD_COLUMN_ADDR_SET);
  lcd_write_data(xs >> 8);
  lcd_write_data(xs);
  // NOTE: 'xe' unchanged
}

static inline void lcd_row_addr_set(uint16_t ys, uint16_t ye)
{
  // This command is used to define area of frame memory where MCU can access.
  // The values of 'ys' and 'ye' are referred when 'LCD_MEMORY_WRITE' command comes.
  // Each value represents one page line in the Frame Memory.
  //
  // 'ys' always must be equal to or less than 'ye'.
  // When 'ys' or 'ye' is greater than maximum address, data of out of range will be ignored.
  lcd_write_cmd(LCD_ROW_ADDR_SET);
  lcd_write_data(ys >> 8);
  lcd_write_data(ys);
  lcd_write_data(ye >> 8);
  lcd_write_data(ye);
}

static inline void lcd_row_addr_ys_set(uint16_t ys)
{
  // This command is used to define area of frame memory where MCU can access.
  // The values of 'ys' and 'ye' are referred when 'LCD_MEMORY_WRITE' command comes.
  // Each value represents one page line in the Frame Memory.
  //
  // 'ys' always must be equal to or less than 'ye'.
  // When 'ys' or 'ye' is greater than maximum address, data of out of range will be ignored.
  lcd_write_cmd(LCD_ROW_ADDR_SET);
  lcd_write_data(ys >> 8);
  lcd_write_data(ys);
  // NOTE: 'ye' unchanged
}

// CS  D/C  RD    WR    Operation
// 0   0    1     0->1  Write command
// 0   1    0->1  1     Read parameter
// 0   1    1     0->1  Write parameter

static inline void lcd_write_bus(uint16_t data)
{
  LCD_SET_BUS(data);
  LCD_WR_0();
  LCD_WR_1();
  // Write cycle                    - min 66 ns
  // Write Control pulse H duration - min 15 ns
  // Write Control pulse L duration - min 15 ns
}

static inline uint16_t lcd_read_bus(void)
{
  uint16_t ret;

  // LCD_RD - The read pin is an active low signal. When /RD is set to logic 0, the ILI9341
  //          will place the contents of the currently selected command on DATA[7:0]. We will
  //          not read data from the ILI9341, so we will set this signal to a 1.
  LCD_RD_0();
  //delay_us(400);
  delay_ms(1);
  ret = LCD_GET_BUS();
  LCD_RD_1();
  //delay_us(100);
  delay_ms(1);
  // Read Cycle (FM)              - min 450 ns
  // Read Control H duration (FM) - min  90 ns
  // Read Control L duration (FM) - min 355 ns
  //
  // Read access time         -        max 340 ns
  // Read output disable time - min 20 max  80 ns

  return ret;
}

static inline void lcd_write_cmd(uint16_t data)
{
#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_ON();
#endif // ! MPU_INTERFACE_MODE_ONLY
  LCD_DC_ON();
  lcd_write_bus(data);
  LCD_DC_OFF();
#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_OFF();
#endif // ! MPU_INTERFACE_MODE_ONLY
}

static inline void lcd_write_data(uint16_t data)
{
#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_ON();
#endif // ! MPU_INTERFACE_MODE_ONLY
  lcd_write_bus(data);
#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_OFF();
#endif // ! MPU_INTERFACE_MODE_ONLY
}

static inline void lcd_write_data_n(const uint16_t *data, uint32_t n)
{
  uint32_t i;

#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_ON();
#endif // ! MPU_INTERFACE_MODE_ONLY
  for (i = 0; i < n; i++)
    lcd_write_bus(data[i]);
#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_OFF();
#endif // ! MPU_INTERFACE_MODE_ONLY
}

static inline void lcd_read_data8_n(uint8_t *data, uint32_t n)
{
  uint32_t i;
  uint16_t aux;
  uint32_t crl = GPIOB->CRL;
  uint32_t crh = GPIOB->CRH;

  // input
  GPIOB->CRL = 0x44444444;
  GPIOB->CRH = 0x44444444;

#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_ON();
#endif // ! MPU_INTERFACE_MODE_ONLY
  for (i = 0; i < n; i++) {
    aux = lcd_read_bus();
    data[i] = aux & 0xFF;
  }
#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_OFF();
#endif // ! MPU_INTERFACE_MODE_ONLY

  // output
  GPIOB->CRL = crl;
  GPIOB->CRH = crh;
}

static inline void lcd_write_data_rep_n(uint16_t data, int32_t n)
{
#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_ON();
#endif // ! MPU_INTERFACE_MODE_ONLY
  while (n != 0) {
    lcd_write_bus(data);
    n--;
  }
#ifndef MPU_INTERFACE_MODE_ONLY
  LCD_CS_OFF();
#endif // ! MPU_INTERFACE_MODE_ONLY
}
