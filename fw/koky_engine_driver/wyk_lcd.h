/*
 * Knihovna pro práci se znakovým LCD
 *
 * autor: wykys
 * verze: 1.0
 * datum: 11.6.2016
 */

#ifndef WYK_LCD_H_INCLUDED
#define WYK_LCD_H_INCLUDED

#include "settings.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdarg.h>

#define LCD_RS		(1<<PC5)		// 0 - instrukce 1 - data
#define LCD_EN		(1<<PC4)

#define LCD_D7		(1<<PC0)
#define LCD_D6		(1<<PC1)
#define LCD_D5		(1<<PC2)
#define LCD_D4		(1<<PC3)

#define LCD_RS_PORT	PORTC
#define LCD_EN_PORT	PORTC

#define LCD_D7_PORT	PORTC
#define LCD_D6_PORT	PORTC
#define LCD_D5_PORT	PORTC
#define LCD_D4_PORT	PORTC

#define LCD_RS_DDR	DDRC
#define LCD_EN_DDR	DDRC

#define LCD_D7_DDR	DDRC
#define LCD_D6_DDR	DDRC
#define LCD_D5_DDR	DDRC
#define LCD_D4_DDR	DDRC

// ========================================================
#define LCD_RS0		LCD_RS_PORT &= ~LCD_RS
#define LCD_EN0		LCD_EN_PORT &= ~LCD_EN

#define LCD_D7_0	LCD_D7_PORT &= ~LCD_D7
#define LCD_D6_0	LCD_D6_PORT &= ~LCD_D6
#define LCD_D5_0	LCD_D5_PORT &= ~LCD_D5
#define LCD_D4_0	LCD_D4_PORT &= ~LCD_D4

#define LCD_RS1		LCD_RS_PORT |= LCD_RS
#define LCD_EN1		LCD_EN_PORT |= LCD_EN

#define LCD_D7_1	LCD_D7_PORT |= LCD_D7
#define LCD_D6_1	LCD_D6_PORT |= LCD_D6
#define LCD_D5_1	LCD_D5_PORT |= LCD_D5
#define LCD_D4_1	LCD_D4_PORT |= LCD_D4
// ========================================================

// ========================================================

#define LCD_CMD_INIT	0b00101000	// 001ANFxx
									// A=0 - 4b mod
									// A=1 - 8b mod
									// N=0 - jednoradkovy
									// N=1 - dvouradkovy
									// F=0 - font 5*8
									// F=1 - font 5*10

#define LCD_CMD_CURS	0b00001100	// 00001DCB
									// D=1 zapne display
									// C=1 zapne cursor podtrzitko
									// B=1 zapne blikani cursoru

#define LCD_CMD_CLR		0b00000001	// smaze display nastaví cursor na zacatek
#define LCD_CMD_W_DDRAM	0b10000000	// pamet pro text
#define LCD_CMD_W_CGRAM	0b01000000	// pamet pro znaky

// ========================================================
void lcd_init(void);
void lcd_cmd(uint8_t cmd);
void lcd_put(char znak);
char nible2hex(uint8_t nible);
void print_hex(uint16_t cislo);
void print_int(int cislo, char znaminko, char nula, int pocet_mist);
void print_double(double cislo, char znaminko, char nula, int mista_pred_carkou, int mista_za_carkou);
void lcd_printf(char str[], ...);
// ========================================================

#endif // WYK_LCD_H_INCLUDED
