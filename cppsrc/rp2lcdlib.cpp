// Simple I2C LCD implementation for rp2 series microcontrollers

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "rp2lcdlib.hpp"

uint8_t LCD_Addr = 0x27;
i2c_inst* LCD_I2C_INST = i2c_default;
uint16_t LCD_CLK_FREQ = 100;
uint8_t LCD_SDA_PIN = PICO_DEFAULT_I2C_SDA_PIN;
uint8_t LCD_SCL_PIN = PICO_DEFAULT_I2C_SCL_PIN;


LCD  :: LCD(uint8_t addr, i2c_inst_t* i2c_inst,  uint16_t CLK_FREQ, uint8_t SDApin, uint8_t SCLpin)
{
    LCD_Addr = addr;
    LCD_I2C_INST = i2c_inst;
    LCD_CLK_FREQ = CLK_FREQ;
    LCD_SDA_PIN = SDApin;
    LCD_SCL_PIN = SCLpin;

};

LCD  :: LCD(){
    // Overload to init LCD with defaults
};

// LCD Commands

const int LCD_CLEARDISPLAY = 0x01;
const int LCD_RETURNHOME = 0x02;
const int LCD_ENTRYMODESET = 0x04;
const int LCD_DISPLAYCONTROL = 0x08;
const int LCD_CURSORSHIFT = 0x10;
const int LCD_FUNCTIONSET = 0x20;
const int LCD_SETCGRAMADDR = 0x40;
const int LCD_SETDDRAMADDR = 0x80;

// Flags for Display Entry Mode

const int LCD_ENTRYSHIFTINCREMENT = 0x01;
const int LCD_ENTRYLEFT = 0x02;

// Flags for Display and Cursor Control

const int LCD_BLINKON = 0x01;
const int LCD_CURSORON = 0x02;
const int LCD_DISPLAYON = 0x04;

// Flags for Display and Cursor Shift

const int LCD_MOVERIGHT = 0x04;
const int LCD_DISPLAYMOVE = 0x08;

// Flags for Function Set

const int LCD_5x10DOTS = 0x04;
const int LCD_2LINE = 0x08;
const int LCD_8BITMODE = 0x10;

// Flags for Backlight Control

const int LCD_BACKLIGHT = 0x08;

// LCD Enable bit

const int LCD_ENABLE_BIT = 0x04;

// Modes for lcd_send_byte

#define LCD_CHARACTER  1
#define LCD_COMMAND    0

#define MAX_LINES      2
#define MAX_CHARS      16



/* Quick helper function for single byte transfers */

void i2c_write_byte(uint8_t val) {

    #ifdef i2c_default
        i2c_write_blocking(i2c_default, LCD_Addr, &val, 1, false);
    #endif

}

void lcd_toggle_enable(uint8_t val){

    #define DELAY_us 600

    sleep_us(DELAY_us);
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(DELAY_us);
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(DELAY_us);

}

// The display is sent a byte as two separate nibble transfers
void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}

static inline void lcd_char(char val) {
    lcd_send_byte(val, LCD_CHARACTER);
}


void LCD::clear(){
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

void LCD::setCursor(int line, int position){

    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND);
}

void LCD::print(const char *s){
    while (*s){
        lcd_char(*s++);
    }
}

void LCD::init(){

    i2c_init(i2c_default, LCD_CLK_FREQ * 1000);
    gpio_set_function(LCD_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(LCD_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(LCD_SDA_PIN);
    gpio_pull_up(LCD_SCL_PIN);


    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    clear();
}
