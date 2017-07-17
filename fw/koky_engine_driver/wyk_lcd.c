#include "wyk_lcd.h"

// inicializuje lcd
void lcd_init(void)
{
	LCD_RS_DDR |= LCD_RS;
	LCD_RS0;

	LCD_EN_DDR |= LCD_EN;
	LCD_EN0;

	LCD_D7_DDR |= LCD_D7;
	LCD_D6_DDR |= LCD_D6;
	LCD_D5_DDR |= LCD_D5;
	LCD_D4_DDR |= LCD_D4;

	LCD_D7_0;
	LCD_D6_0;
	LCD_D5_0;
	LCD_D4_0;


	lcd_set_nibble(LCD_CMD_INIT>>4);
	LCD_EN1;
	_delay_ms(2);
	LCD_EN0;
	lcd_cmd(LCD_CMD_INIT);
	lcd_cmd(LCD_CMD_CURS);
	lcd_cmd(LCD_CMD_CLR);

	// přidání vlastních znaků
	lcd_cmd(LCD_CMD_W_CGRAM);
	// trojuhelnik
	lcd_put(0b00010000);
	lcd_put(0b00011000);
	lcd_put(0b00011100);
	lcd_put(0b00011110);
	lcd_put(0b00011100);
	lcd_put(0b00011000);
	lcd_put(0b00010000);
	lcd_put(0b00000000);
	// bar
	lcd_put(0b00000000);
	lcd_put(0b00000000);
	lcd_put(0b00011111);
	lcd_put(0b00011111);
	lcd_put(0b00011111);
	lcd_put(0b00011111);
	lcd_put(0b00000000);
	lcd_put(0b00000000);
	
	// přepnutí na zápis do zobrazovací paměti
	lcd_cmd(LCD_CMD_W_DDRAM);
}

// posílat napřed msn a pak lsn
void lcd_set_nibble(uint8_t nibble)
{
	if (nibble & 0x08) LCD_D7_1; else LCD_D7_0;
	if (nibble & 0x04) LCD_D6_1; else LCD_D6_0;
	if (nibble & 0x02) LCD_D5_1; else LCD_D5_0;
	if (nibble & 0x01) LCD_D4_1; else LCD_D4_0;
}

// pošle lcd instrukci
void lcd_cmd(uint8_t cmd)
{
	lcd_set_nibble(cmd>>4);
	LCD_EN1;
	_delay_ms(3);
	LCD_EN0;	
	lcd_set_nibble(cmd);
	LCD_EN1;
	_delay_ms(3);
	LCD_EN0;
}

// pošle lcd znak
void lcd_put(char znak)
{
	LCD_RS1;
	lcd_set_nibble(znak>>4);
	LCD_EN1;
	_delay_us(40);
	LCD_EN0;
	lcd_set_nibble(znak);
	LCD_EN1;
	_delay_us(40);
	LCD_EN0;
	LCD_RS0;
}

char nible2hex(uint8_t nible)
{
	if (nible < 10)
		return '0' + nible;
	else
		return 'A' + nible - 10;
}

// vytiskne číslo int 2B
void print_hex(uint16_t cislo)
{
	uint16_t i;
	uint16_t nible;

	for (i=0; i < 4; i++)
	{
		nible = (uint8_t) ((cislo >> 12) & 0x000F);
		cislo <<= 4;
		lcd_put(nible2hex(nible));
	}
}

// vytiskne číslo int 2B
void print_int(int cislo, char znaminko, char nula, int pocet_mist)
{
	int poc_cislo = cislo;
	int8_t i, len = 16, state = 1;
	char buf[16];

	// pokud máme vypsat před číslo znamínko
	if (znaminko)
	{
		lcd_put((cislo >= 0) ? '+' : '-');
	}

	// pokud je vypnutá predikce nul
	if (nula)
		state = 0;


	// když je zadána délka čísla
	if (pocet_mist >= 0)
		len = pocet_mist;

	// absolutní hodnota čísla
	cislo = (cislo >= 0) ? cislo : -1 * cislo;

	// převod čísla na řětězec
	for (i=0; i < len; i++)
	{
		buf[i] = '0' + cislo % 10;
		cislo /= 10;
	}
	// výpis řetězce
	for (i--; i >= 0; i--)
	{
		if (buf[i] == '0' && state)
		{
			if (pocet_mist != -1 && (poc_cislo || (i+1) != len))
			{
				lcd_put(' ');
			}
		}
		else
		{
			state = 0;
			lcd_put(buf[i]);
		}

	}
	if (state)
	{
		lcd_put('0');
	}
}

// vytiskne double
void print_double(double cislo, char znaminko, char nula, int mista_pred_carkou, int mista_za_carkou)
{
	int i, cela, desetinna;
	char znam = '+';

	if (mista_za_carkou == -1)
		mista_za_carkou = 3;

	if (cislo < 0)
	{
		cislo *= -1;
		znam = '-';
	}

	cela = (int) cislo;
	cislo -= cela;

	for (i=0; i < mista_za_carkou; i++)
		cislo *= 10;

	desetinna = (int) cislo;
	cela = (znam == '-') ? -1 * cela : cela;

	print_int(cela, znaminko, nula, mista_pred_carkou);
	lcd_put(',');
	print_int(desetinna, '\0', '0', mista_za_carkou);

}

// printf funkce
void lcd_printf(char str[], ...)
{
	static uint8_t line = 1;

	uint8_t i, j;
	char *str_p;
	int tmp_int;

	char znaminko = '\0';
	char nula = '\0';
	char desetina_carka = '\0';
	int mista_pred_carkou = -1;
	int mista_za_carkou = -1;



	 __gnuc_va_list ap;
	 //va_list ap;
	 va_start(ap, str);



	for (i=0; str[i] != '\0'; i++)
	{
		// nová stránka
		if (str[i] == '\f')
		{
			line = 1;
			lcd_cmd(LCD_CMD_CLR);
			lcd_cmd(LCD_CMD_W_DDRAM);
		}
		// nový řádek
		else if (str[i] == '\n')
		{
			// jestli jsme na 1.
			if (line == 1)
			{
				line++;
				lcd_cmd(LCD_CMD_W_DDRAM | 0x40); // go 2 line
			}
			// jestli jsme na 2.
			else if (line == 2)
			{
				line++;
				lcd_cmd(LCD_CMD_W_DDRAM | 0x14); // go 3 line
			}
			// jestli jsme na 3.
			else if (line == 3)
			{
				line++;
				lcd_cmd(LCD_CMD_W_DDRAM | 0x54); // go 4 line
			}
			// jestli jsme na 4.
			else
			{
				line = 1;
				lcd_cmd(LCD_CMD_W_DDRAM | 0x00); // go 1 line
			}
		}
		// návrat vozíku
		else if (str[i] == '\r')
		{
			// pokud jsme na 2. řádku
			if (line)
			{
				lcd_cmd(LCD_CMD_W_DDRAM | 0x40);
			}
			// jestli jsme na 1.
			else
			{
				lcd_cmd(LCD_CMD_W_DDRAM);
			}
		}
		// tabulátor
		else if (str[i] == '\t')
		{
			lcd_put(' ');
			lcd_put(' ');
			lcd_put(' ');
			lcd_put(' ');
		}
		// znak lomeno
		else if (str[i] == '\\')
		{
			lcd_put(0xA4);
		}
		else if (str[i] == '%')
		{
			// pokud jde o znak %
			if (str[i+1] == '%')
			{
				i++;
				lcd_put('%');
			}
			// když jde o proměnnou
			else
			{
				// zjistáme jakého typu jsou data
				for (j=i+1;								\
					str[j] != 'c' && 					\
					str[j] != 'H' &&					\
					str[j] != 'd' &&					\
					str[j] != 'f' &&					\
					str[j] != 's'; 						\
					j++)
				{
					if (str[j] == '+' || str[j] == '-')
					{
						znaminko = '+';
					}
					else if (str[j] == '.')
					{
						desetina_carka = ',';
					}
					else if (str[j] == '0')
					{
						if (desetina_carka == '\0')
						{
							if (nula == '0')
								mista_pred_carkou *= 10;
							else
								nula = '0';
						}
						else
						{
							mista_za_carkou = 0;
						}

					}
					else if (str[j] >= '1' && str[j] <= '9')
					{
						if (desetina_carka == ',')
						{
							if (mista_za_carkou == -1)
								mista_za_carkou = 0;

							mista_za_carkou *= 10;
							mista_za_carkou += str[j] - '0';
						}
						else
						{
							if (mista_pred_carkou == -1)
								mista_pred_carkou = 0;

							mista_pred_carkou *= 10;
							mista_pred_carkou += str[j] - '0';
						}
					}
				}

				// posuneme index procházení pole
				i = j;

				// podle typu s nimi naložíme
				if (str[j] == 'c')
				{
					lcd_put(va_arg(ap, int));
				}
				else if (str[j] == 'H')
				{
					print_hex(va_arg(ap, uint16_t));
				}
				else if (str[j] == 'd')
				{
					tmp_int = va_arg(ap, int);
					print_int(tmp_int, znaminko, nula, mista_pred_carkou);
				}
				else if (str[j] == 'f')
				{
					print_double(va_arg(ap, double), znaminko, nula, mista_pred_carkou, mista_za_carkou);
				}
				else if (str[j] == 's')
				{
					str_p = va_arg(ap, char *);
					for (j=0; *(str_p + j) != '\0'; j++)
						lcd_put(*(str_p + j));
				}

				// vynulovní formátovacích proměnných
				znaminko = '\0';
				nula = '\0';
				desetina_carka = '\0';
				mista_pred_carkou = -1;
				mista_za_carkou = -1;
			}
		}
		else
		{
			lcd_put(str[i]);
		}
	}
	va_end(ap);
}
