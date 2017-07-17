 /*
 * Knihovna zajistujici textove rozhranni pro ovladani motoru
 *
 * autor: wykys
 * verze: 1.0
 * datum: 04.06.2017
 */

#ifndef WYK_TUI_H_INCLUDED
#define WYK_TUI_H_INCLUDED

#include "settings.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "wyk_lcd.h"
#include "wyk_step_stick.h"

#define MENU_SYMBOL_SELECT	8
#define MENU_SYMBOL_SPACE	' '
#define MENU_BAR_SYMBOL		9

typedef enum
{
	MAIN_MENU,
	ITEM_MENU,
	RPM_MENU,
	TIME_MENU,
	LOAD_MENU,
	SAVE_MENU
} menu_t;

typedef enum
{
	MACHINE_START,
	MACHINE_STOP
} machine_state_t;

typedef enum
{
	MAIN_MENU_SELECT_CONTOL,
	MAIN_MENU_SELECT_MENU
} main_menu_select_t;

typedef enum
{
	ITEM_MENU_SELECT_RPM,
	ITEM_MENU_SELECT_TIME,
	ITEM_MENU_SELECT_LOAD,
	ITEM_MENU_SELECT_SAVE
} item_menu_select_t;

typedef enum
{
	LOAD_MENU_SELECT_PROFILE1,
	LOAD_MENU_SELECT_PROFILE2,
	LOAD_MENU_SELECT_PROFILE3
} load_menu_select_t;

typedef enum
{
	SAVE_MENU_SELECT_PROFILE1,
	SAVE_MENU_SELECT_PROFILE2,
	SAVE_MENU_SELECT_PROFILE3
} save_menu_select_t;

typedef struct
{
	uint16_t rpm;
	uint16_t sec;
} profile_t;

typedef struct
{
	uint8_t profile_id;
	profile_t profile[3];
} eeprom_data_t;

extern menu_t menu;
extern uint32_t back_tick;
extern uint16_t actual_rmp;
extern profile_t user_profile, backup_profile;
extern machine_state_t machine_state;
extern eeprom_data_t eeprom_data;

// ========================================================

void TUI_sec_timer_init(void);
void TUI_sec_timer_start(void);
void TUI_sec_timer_stop(void);

void TUI_back_timer_init(void);
void TUI_back_timer_clear(void);

void TUI_inti(void);
void TUI(char action);
// ========================================================

#endif // WYK_TUI_H_INCLUDED
