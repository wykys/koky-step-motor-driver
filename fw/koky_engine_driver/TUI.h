 /*
 * Knihovna zajištující textové rozhranní pro ovládání motoru
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
#include <avr/interrupt.h>
#include "wyk_lcd.h"
#include "wyk_step_stick.h"

#define MENU_SYMBOL_SELECT	'>'
#define MENU_SYMBOL_SPACE	' '

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

typedef struct
{
	uint16_t sec;
	uint16_t rpm;
} profile_t;

extern menu_t menu;
extern uint32_t back_tick;
extern profile_t user_profile, backup_profile;
extern machine_state_t machine_state;

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