// Description of file, etc. See "buzzer.h".

#include "buzzer.h"

#include "init.h"
#include "delay.h"
#include "btn_leds.h"
#include "eeprom.h"
#include "windows.h"

static uint8_t delay_buzzer_quality_a;
static uint8_t delay_buzzer_quality_b;
static uint8_t delay_key_tone_a;
static uint8_t delay_key_tone_b;

void beep(void)
{
  BUZZER_ON();
  delay_ms(50);
  BUZZER_OFF();
}

void delay_key_tone_update(void)
{
  if (eeprom_10h_8.key_tone == ON) {
    delay_key_tone_a = 1;
    delay_key_tone_b = 10;
  } else {
    delay_key_tone_a = 0;
    delay_key_tone_b = 0;
  }
}

void buzzer_loop(void)
{
  if ((eeprom_10h_8.lock_tone == ON) && (state_lock != 0)) {
    if ((delay_buzzer_quality_a == 0) && (delay_buzzer_quality_b == 0))
      delay_buzzer_quality_a = (((100 - quality_percent) * 32) / 100) + 5;
    if (delay_buzzer_quality_a == 0) {
      if (delay_buzzer_quality_b != 0) {
        BUZZER_OFF();
        delay_buzzer_quality_b--;
        if (delay_buzzer_quality_b == 0)
          delay_buzzer_quality_a = (((100 - quality_percent) * 32) / 100) + 5;
        else
          delay_buzzer_quality_a = 0;
      }
    } else {
      BUZZER_ON();
      delay_buzzer_quality_a--;
      if (delay_buzzer_quality_a == 0)
        delay_buzzer_quality_b = 5;
      else
        delay_buzzer_quality_b = 0;
    }
  } else {
    delay_buzzer_quality_a = 0;
    delay_buzzer_quality_b = 0;
    if (delay_key_tone_a == 0)
      BUZZER_OFF();
    else if (delay_key_tone_b == 0) {
      BUZZER_OFF();
      delay_key_tone_a = 0;
    } else {
      delay_key_tone_b--;
      BUZZER_ON();
    }
  }
}
