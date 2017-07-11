/*
 * TUI.c
 *
 * Created: 04.06.2017 14:47:55
 *  Author: wykys
 */

#include "TUI.h"

menu_t menu;
uint32_t back_tick;
profile_t user_profile, backup_profile;
machine_state_t machine_state;

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
			user_profile.sec = backup_profile.sec;
			user_profile.rpm = backup_profile.rpm;
		}
		TUI('s');
	}
}


void TUI_back_timer_init(void)
{
	TCCR2A = 0;						// normal mode
	TCCR2B = (1<<CS12) | (1<<CS10);	// clk/1024
	OCR2A = 0xFF;
	TIMSK2 = (1<<TOIE2);			// compare IT enable
	TUI_back_timer_clear();
}
void TUI_back_timer_clear(void)
{
	back_tick = 0;
	TCNT2 = 0;
}
ISR(TIMER2_OVF_vect)
{
	back_tick++;
	// nefunguje pøesnì!!!!!!!
	if (back_tick == ( (((uint32_t) 120) * ((uint32_t) F_CPU)) / (((uint32_t) 256) * ((uint32_t) 1024))) )
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
	user_profile.sec = 3610;
	user_profile.rpm = 200;
	menu = MAIN_MENU;
	machine_state = MACHINE_STOP;
	
	TUI_sec_timer_init();
	TUI_back_timer_init();
}

void TUI(char action)
{
	static main_menu_select_t main_menu_secelt = MAIN_MENU_SELECT_CONTOL;
	static item_menu_select_t item_menu_secect = ITEM_MENU_SELECT_RPM;

	// nastaveni
	switch (menu)
	{
		case MAIN_MENU:
			switch (action)
			{
				// pohyb v menu
				case '+':
				case '-':
					main_menu_secelt = (main_menu_secelt == MAIN_MENU_SELECT_CONTOL) ? MAIN_MENU_SELECT_MENU : MAIN_MENU_SELECT_CONTOL;
					break;

				// stisk tlacitka
				case 'b':
					if (main_menu_secelt == MAIN_MENU_SELECT_CONTOL)
					{
						if (machine_state == MACHINE_STOP)
						{
							machine_state = MACHINE_START;
							backup_profile.sec = user_profile.sec;
							backup_profile.rpm = user_profile.rpm;
							motor_smoothly_rmp(0, user_profile.rpm);
							TUI_sec_timer_start();
						}
						else
						{
							machine_state = MACHINE_STOP;
							motor_smoothly_rmp(user_profile.rpm, 0);
							TUI_sec_timer_stop();
							user_profile.sec = backup_profile.sec;
							user_profile.rpm = backup_profile.rpm;
						}
					}
					else
					{
						menu = ITEM_MENU;
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
							backup_profile.rpm = user_profile.rpm;
							break;
						case ITEM_MENU_SELECT_TIME:
							menu = TIME_MENU;
							backup_profile.sec = user_profile.sec;
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
					if (user_profile.rpm < 200)
						user_profile.rpm++;
					break;
				case '-':
					if (user_profile.rpm > 0)
						user_profile.rpm--;
					break;
				case  'b':
					menu = MAIN_MENU;
					break;
				default:
					;
			}
			break;

		case TIME_MENU:
			switch (action)
			{
				case '+':
					if (user_profile.sec < 36000)
						user_profile.sec+=60;
					break;
				case '-':
					if (user_profile.sec > 0)
						user_profile.sec-=60;
					break;
				case  'b':
					menu = MAIN_MENU;
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
					lcd_printf("\n%cStart", (main_menu_secelt == MAIN_MENU_SELECT_CONTOL) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE);
					break;
				default:
					lcd_printf("\n%cStop", (main_menu_secelt == MAIN_MENU_SELECT_CONTOL) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE);
					break;
			}

			lcd_printf("\n%cMenu", (main_menu_secelt == MAIN_MENU_SELECT_MENU) ?  MENU_SYMBOL_SELECT : MENU_SYMBOL_SPACE);
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
			lcd_printf("\nStara hodnota:%6d", backup_profile.rpm);
			lcd_printf("\nNova  hodnota:%6d", user_profile.rpm);
			break;

		case TIME_MENU:
			lcd_printf("\fNastaveni delky");
			lcd_printf("\nomylani [HH:MM]");
			lcd_printf("\nStary cas:     %2d:%02d", backup_profile.sec/3600, (backup_profile.sec%3600)/60);
			lcd_printf("\nNovy  cas:     %2d:%02d", user_profile.sec/3600, (user_profile.sec%3600)/60);
			break;

		default:
		;
	}
}
