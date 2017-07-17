/*
 * TUI.c
 *
 * Created: 04.06.2017 14:47:55
 *  Author: wykys
 */

#include "TUI.h"

menu_t menu;
uint32_t back_tick;
uint16_t actual_rmp;
profile_t user_profile, backup_profile;
machine_state_t machine_state;
eeprom_data_t eeprom_data = { 0, {{100, 30*60}, {50, 20*60}, {20, 90*60}} };

#define EEP_ADDR		(void *)0x10
#define eep_read()		eeprom_read_block(&eeprom_data, EEP_ADDR, sizeof(eeprom_data_t))
#define eep_update()	eeprom_update_block(&eeprom_data, EEP_ADDR, sizeof(eeprom_data_t))

void TUI_sec_timer_init(void)
{
	TUI_sec_timer_stop();
	TCCR1A = 0;
	OCR1A = F_CPU / 1024;							// TOP value for 1 sec
	TIMSK1 = (1<<OCIE1A);							// compare IT enable
}
void TUI_sec_timer_start(void)
{
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);	// CTC mode clk/1024
}
void TUI_sec_timer_stop(void)
{
	TCCR1B = (1<<WGM12);							// CTC mode clk = 0
}
ISR(TIMER1_COMPA_vect)
{
	if (machine_state == MACHINE_START)
	{
		if (user_profile.sec > 0)
		{
			user_profile.sec--;
		}
		if (!user_profile.sec)
		{
			motor_smoothly_rmp(user_profile.rpm, 0);
			TUI_sec_timer_stop();
			machine_state = MACHINE_STOP;
			user_profile = backup_profile;
		}
		TUI('s');
	}
}


void TUI_back_timer_init(void)
{
	TCCR0A = 0;									// normal mode
	TCCR0B = (1<<CS02) | (1<<CS00);				// clk/1024
	TIMSK0 = (1<<TOIE0);						// compare IT enable
	TUI_back_timer_clear();
}
void TUI_back_timer_clear(void)
{
	back_tick = 0;
	TCNT0 = 0;
}
ISR(TIMER0_OVF_vect)
{
	const uint8_t sec_to_return = 3;
	back_tick++;
	if (back_tick == ( (((uint32_t) sec_to_return) * ((uint32_t) F_CPU)) / (((uint32_t) 256) * ((uint32_t) 1024))) )
	{
		TUI_back_timer_clear();
		if (menu != MAIN_MENU)
		{
			menu = MAIN_MENU;
			TUI('s');
		}
	}
}

void TUI_inti(void)
{
	menu = MAIN_MENU;
	machine_state = MACHINE_STOP;
	actual_rmp = 0;

	TUI_sec_timer_init();
	TUI_back_timer_init();

	// EEPROM is empty
	if (eeprom_read_byte(EEP_ADDR) == 0xFF)
	{
		eep_update();
	}
	eep_read();

	user_profile = eeprom_data.profile[eeprom_data.profile_id];
	backup_profile = user_profile;
}

void TUI(char action)
{
	static main_menu_select_t main_menu_secelt = MAIN_MENU_SELECT_CONTOL;
	static item_menu_select_t item_menu_secect = ITEM_MENU_SELECT_RPM;
	static load_menu_select_t load_menu_select = LOAD_MENU_SELECT_PROFILE1;
	static save_menu_select_t save_menu_select = SAVE_MENU_SELECT_PROFILE1;

	// nastaveni
	switch (menu)
	{
		case MAIN_MENU:
			switch (action)
			{
				// pohyb v menu
				case '+':
				case '-':
					// deactivate menu when motor start
					if (machine_state == MACHINE_START)
						break;
					main_menu_secelt = (main_menu_secelt == MAIN_MENU_SELECT_CONTOL) ? MAIN_MENU_SELECT_MENU : MAIN_MENU_SELECT_CONTOL;
					break;

				// stisk tlacitka
				case 'b':
					if (main_menu_secelt == MAIN_MENU_SELECT_CONTOL)
					{
						if (machine_state == MACHINE_STOP)
						{
							machine_state = MACHINE_START;
							backup_profile = user_profile;
							motor_smoothly_rmp(actual_rmp, user_profile.rpm);
							TUI_sec_timer_start();
						}
						else
						{
							machine_state = MACHINE_STOP;
							motor_smoothly_rmp(user_profile.rpm, 0);
							TUI_sec_timer_stop();
							user_profile = backup_profile;
						}
					}
					else
					{
						menu = ITEM_MENU;
						backup_profile = user_profile;
					}
					break;

				default:
					;
			}
			break;

		case ITEM_MENU:
			switch (action)
			{
				case '+':
					switch (item_menu_secect)
					{
						case ITEM_MENU_SELECT_RPM:
							item_menu_secect = ITEM_MENU_SELECT_TIME;
							break;
						case ITEM_MENU_SELECT_TIME:
							item_menu_secect = ITEM_MENU_SELECT_LOAD;
							break;
						case ITEM_MENU_SELECT_LOAD:
							item_menu_secect = ITEM_MENU_SELECT_SAVE;
							break;
						default:
							item_menu_secect = ITEM_MENU_SELECT_RPM;
					}
					break;

				case  '-':
					switch (item_menu_secect)
					{
						case ITEM_MENU_SELECT_RPM:
							item_menu_secect = ITEM_MENU_SELECT_SAVE;
							break;
						case ITEM_MENU_SELECT_TIME:
							item_menu_secect = ITEM_MENU_SELECT_RPM;
							break;
						case ITEM_MENU_SELECT_LOAD:
							item_menu_secect = ITEM_MENU_SELECT_TIME;
							break;
						default:
							item_menu_secect = ITEM_MENU_SELECT_LOAD;
					}
					break;

				case 'b':
					switch (item_menu_secect)
					{
						case ITEM_MENU_SELECT_RPM:
							menu = RPM_MENU;
							break;
						case ITEM_MENU_SELECT_TIME:
							menu = TIME_MENU;
							break;
						case ITEM_MENU_SELECT_LOAD:
							menu = LOAD_MENU;
							break;
						default:
							menu = SAVE_MENU;
					}
					break;

				default:
					;
			}
			break;

		case RPM_MENU:
			switch (action)
			{
				case '+':
					if (backup_profile.rpm < 300)
						backup_profile.rpm++;
					break;
				case '-':
					if (backup_profile.rpm > 0)
						backup_profile.rpm--;
					break;
				case  'b':
					menu = MAIN_MENU;
					user_profile = backup_profile;
					break;
				default:
					;
			}
			break;

		case TIME_MENU:
			switch (action)
			{
				case '+':
					if (backup_profile.sec < 36000)
						backup_profile.sec+=60;
					break;
				case '-':
					if (backup_profile.sec > 0)
						backup_profile.sec-=60;
					break;
				case  'b':
					menu = MAIN_MENU;
					user_profile = backup_profile;
					break;
				default:
					;
			}
			break;

		case LOAD_MENU:
			switch (action)
			{
				case '+':
					switch (load_menu_select)
					{
						case LOAD_MENU_SELECT_PROFILE1:
							load_menu_select = LOAD_MENU_SELECT_PROFILE2;
							break;
						case LOAD_MENU_SELECT_PROFILE2:
							load_menu_select = LOAD_MENU_SELECT_PROFILE3;
							break;
						default:
							load_menu_select = LOAD_MENU_SELECT_PROFILE1;
					}
					break;
				case '-':
					switch (load_menu_select)
					{
						case LOAD_MENU_SELECT_PROFILE1:
							load_menu_select = LOAD_MENU_SELECT_PROFILE3;
							break;
						case LOAD_MENU_SELECT_PROFILE2:
							load_menu_select = LOAD_MENU_SELECT_PROFILE1;
							break;
						default:
							load_menu_select = LOAD_MENU_SELECT_PROFILE2;
					}
					break;
				case  'b':
					menu = MAIN_MENU;
					eeprom_data.profile_id = load_menu_select;
					user_profile = eeprom_data.profile[eeprom_data.profile_id];
					eep_update();
					break;
				default:
				;
			}
			break;

		case SAVE_MENU:
			switch (action)
			{
				case '+':
					switch (save_menu_select)
					{
						case SAVE_MENU_SELECT_PROFILE1:
							save_menu_select = SAVE_MENU_SELECT_PROFILE2;
							break;
						case SAVE_MENU_SELECT_PROFILE2:
							save_menu_select = SAVE_MENU_SELECT_PROFILE3;
							break;
						default:
							save_menu_select = SAVE_MENU_SELECT_PROFILE1;
					}
					break;
				case '-':
					switch (save_menu_select)
					{
						case SAVE_MENU_SELECT_PROFILE1:
							save_menu_select = SAVE_MENU_SELECT_PROFILE3;
							break;
						case SAVE_MENU_SELECT_PROFILE2:
							save_menu_select = SAVE_MENU_SELECT_PROFILE1;
							break;
						default:
							save_menu_select = SAVE_MENU_SELECT_PROFILE2;
					}
					break;
				case  'b':
					menu = MAIN_MENU;
					eeprom_data.profile_id = save_menu_select;
					eeprom_data.profile[eeprom_data.profile_id] = user_profile;
					eep_update();
					break;
				default:
				;
			}
			break;


		default:
			;
	}

	// zobrazeni
	switch (menu)
	{
		case MAIN_MENU:

			if (user_profile.sec/3600)
				lcd_printf("\fCas:        %2d:%02d:%02d", user_profile.sec/3600, (user_profile.sec%3600)/60,  (user_profile.sec%3600)%60);
			else
				lcd_printf("\fCas:           %2d:%02d", (user_profile.sec%3600)/60,  (user_profile.sec%3600)%60);
			lcd_printf("\nOtacky:         %4d", user_profile.rpm);

			switch (machine_state)
			{
				case MACHINE_STOP:
					lcd_printf("\n\n%cStart        %cMenu ", (main_menu_secelt == MAIN_MENU_SELECT_CONTOL) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE, (main_menu_secelt == MAIN_MENU_SELECT_MENU) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE);
					break;
				default:
					lcd_printf("\n");
					// bar to end
					uint32_t bar;
					bar = backup_profile.sec - user_profile.sec;
					if (bar)	// div zero
					{
						bar *= 20;
						bar = bar / backup_profile.sec;
					}
					for (; bar > 0; bar--)
					{
						lcd_printf("%c", MENU_BAR_SYMBOL);
					}

					lcd_printf("\n%cStop", (main_menu_secelt == MAIN_MENU_SELECT_CONTOL) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE);
					break;
			}
			break;

		case ITEM_MENU:
			lcd_printf("\f%cRychlost omylani", (item_menu_secect == ITEM_MENU_SELECT_RPM) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE);
			lcd_printf("\n%cDelka omylani", (item_menu_secect == ITEM_MENU_SELECT_TIME) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE);
			lcd_printf("\n%cNacist profil", (item_menu_secect == ITEM_MENU_SELECT_LOAD) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE);
			lcd_printf("\n%cUlozit profil", (item_menu_secect == ITEM_MENU_SELECT_SAVE) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE);
			break;

		case RPM_MENU:
			lcd_printf("\fNastaveni otacek");
			lcd_printf("\nza minutu");
			lcd_printf("\nStara hodnota:%6d", user_profile.rpm);
			lcd_printf("\nNova  hodnota:%6d", backup_profile.rpm);
			break;

		case TIME_MENU:
			lcd_printf("\fNastaveni delky");
			lcd_printf("\nomylani [HH:MM]");
			lcd_printf("\nStary cas:     %2d:%02d", user_profile.sec/3600, (user_profile.sec%3600)/60);
			lcd_printf("\nNovy  cas:     %2d:%02d", backup_profile.sec/3600, (backup_profile.sec%3600)/60);
			break;

		case LOAD_MENU:
			lcd_printf("\fNacteni profilu");
			lcd_printf("\n%c1. %2dh %2dm %4dot/m", (load_menu_select == LOAD_MENU_SELECT_PROFILE1) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE, eeprom_data.profile[0].sec/3600, (eeprom_data.profile[0].sec%3600)/60, eeprom_data.profile[0].rpm);
			lcd_printf("\n%c2. %2dh %2dm %4dot/m", (load_menu_select == LOAD_MENU_SELECT_PROFILE2) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE, eeprom_data.profile[1].sec/3600, (eeprom_data.profile[1].sec%3600)/60, eeprom_data.profile[1].rpm);
			lcd_printf("\n%c3. %2dh %2dm %4dot/m", (load_menu_select == LOAD_MENU_SELECT_PROFILE3) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE, eeprom_data.profile[2].sec/3600, (eeprom_data.profile[2].sec%3600)/60, eeprom_data.profile[2].rpm);
			break;

		case SAVE_MENU:
			lcd_printf("\fUlozeni profilu");
			lcd_printf("\n%c1. %2dh %2dm %4dot/m", (save_menu_select == SAVE_MENU_SELECT_PROFILE1) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE, eeprom_data.profile[0].sec/3600, (eeprom_data.profile[0].sec%3600)/60, eeprom_data.profile[0].rpm);
			lcd_printf("\n%c2. %2dh %2dm %4dot/m", (save_menu_select == SAVE_MENU_SELECT_PROFILE2) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE, eeprom_data.profile[1].sec/3600, (eeprom_data.profile[1].sec%3600)/60, eeprom_data.profile[1].rpm);
			lcd_printf("\n%c3. %2dh %2dm %4dot/m", (save_menu_select == SAVE_MENU_SELECT_PROFILE3) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE, eeprom_data.profile[2].sec/3600, (eeprom_data.profile[2].sec%3600)/60, eeprom_data.profile[2].rpm);
			break;

		default:
		;
	}
}
