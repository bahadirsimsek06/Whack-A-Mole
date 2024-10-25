/*
 * i2c-lcd.c
 *
 *  Created on: 26 Eyl 2019
 *      Author: ATSON
 */
#include "i2c-lcd.h"

extern I2C_InitTypeDef I2C_InitStruct;

uint8_t myDeviceAddress = 0x4E;

unsigned char karakter_[8][8]=
{
 /* TR Karakterler */
{ 0,14,16,16,17,14, 4, 0},//�
{ 0, 0,12, 4, 4, 4,14, 0},//I
{10, 0,14,17,17,17,14, 0},//�
{ 0,15,16,14, 1,30, 4, 0},//$
{10, 0,17,17,17,17,14, 0},//�
/* �zel isaretler */
{2, 6,14,30,14, 6,  2, 0},//<
{ 0, 4, 4, 4, 4, 4, 4, 0},//|
{ 0, 16, 8, 4, 2, 1, 0,0} //\//
};


void lcd_init(void)
{
	char cc0[] = {0, 0, 10, 21, 17, 10, 4, 0}; // kalp
	char cc1[] = {31, 31, 31, 31, 31, 31, 31, 31}; // full cell
	char cc2[] = {31, 17, 17, 17, 17, 17, 17, 31}; // OFF led
	char cc3[] = {17, 17, 21, 10, 10, 4, 10, 27}; // ON led

	lcd_send_cmd(0x02);				// return to home
	lcd_send_cmd(0x28);				// Function set	-> DB5 = 1 and DB3 = N = 1 2-line display mode
	lcd_send_cmd(0x0C);				// Display ON/OFF Control DB3 = 1 and DB2 = D = 1 display is turned on
	// if DB1 = C = 1 cursor on or DB1 = C = 0 cursor is off and DB0 = B = 1 cursor blink on or DB1 = C = 0 cursor blink off.

	lcd_send_cmd(0x40);
	for (int i=0; i<8; i++) lcd_send_data(cc0[i]);

	lcd_send_cmd(0x40+8);
	for (int i=0; i<8; i++) lcd_send_data(cc1[i]);

	lcd_send_cmd(0x40+16);
	for (int i=0; i<8; i++) lcd_send_data(cc2[i]);

	lcd_send_cmd(0x40+24);
	for (int i=0; i<8; i++) lcd_send_data(cc3[i]);

	lcd_send_cmd(0x80);
}

void lcd_send_cmd(char cmd)
{
	char data_u, data_l;
	uint8_t data_t[4];

	/*
	 * Komut g�ndermek i�in �ncelikle RS = 0 ve E = 1 yap�l�r.
	 * Ard�ndan y�ksek de�erlikli 4 bit g�nderilir.
	 * Y�ksek de�erlikli 4 bit g�nderildikten sonra E = 0 yap�l�r
	 * G�nderilen y�ksek de�erlikli 4 bit temizlenir.
	 * Ard�ndan E = 1 yap�l�r ve ilk 4 bit lcd ye g�nderilir.
	 * Ve E = 0 yap�larak, son g�nderilen d���k de�erli 4 bit porttan silinir.
	 */

	data_u = (cmd & 0xF0);			// cmd & 0x11110000 -> 0xabcdefgh & 0x11110000 = 0xabcd0000	    ( Y�ksek de�erlikli 4 bit)
	data_l = ((cmd << 4) & 0xF0);	// (cmd << 4) & 0x11110000 -> 0xefgh & 0x11110000 = 0xefgh0000  ( D���k de�erlikli 4 bit)

	data_t[0] = data_u | 0x0C; 		// [en = 1 & rs = 0] 0xabcd0000 | 0x00001100 -> 0xabcd1100
	I2C_Write(myDeviceAddress, data_t[0]);
	data_t[1] = data_u | 0x08;		// [en = 0 & rs = 0] 0xabcd0000 | 0x00001000 -> 0xabcd1000
	I2C_Write(myDeviceAddress, data_t[1]);
	data_t[2] = data_l | 0x0C;		// [en = 1 & rs = 0] 0xefgh0000 | 0x00001100 ->	0xefgh1100
	I2C_Write(myDeviceAddress, data_t[2]);
	data_t[3] = data_l | 0x08;		// [en = 0 & rs = 0] 0xefgh0000	| 0x00001000 -> 0xefgh1000
	I2C_Write(myDeviceAddress, data_t[3]);
}

void lcd_send_data(char data)
{
	char data_u, data_l;
	uint8_t data_t[4];

	data_u = (data & 0xF0);			// data & 0x11110000 -> 0xabcdefgh & 0x11110000 = 0xabcd0000
	data_l = ((data << 4) & 0xF0);	// (data << 4) & 0x11110000 -> 0xefgh & 0x11110000 = 0xefgh0000

	data_t[0] = data_u | 0x0D; 		// [en = 1 & rs = 1] 0xabcd0000 | 0x00001101 -> 0xabcd1101
	I2C_Write(myDeviceAddress, data_t[0]);
	data_t[1] = data_u | 0x09;		// [en = 0 & rs = 1] 0xabcd0000 | 0x00001001 -> 0xabcd1001
	I2C_Write(myDeviceAddress, data_t[1]);
	data_t[2] = data_l | 0x0D;		// [en = 1 & rs = 1] 0xefgh0000 | 0x00001100 ->	0xefgh1101
	I2C_Write(myDeviceAddress, data_t[2]);
	data_t[3] = data_l | 0x09;		// [en = 0 & rs = 1] 0xefgh0000	| 0x00001000 -> 0xefgh1001
	I2C_Write(myDeviceAddress, data_t[3]);
}

void lcd_sende_string(char *str)
{
	while(*str)
		lcd_send_data(*str++);
}
