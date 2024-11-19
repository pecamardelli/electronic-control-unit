/*****************************************************************************
 * | File      	:   trst.h
 * | Author      :   Waveshare team
 * | Function    :
 * | Info        :
 *
 *----------------
 * |	This version:   V1.0
 * | Date        :   2020-05-20
 * | Info        :   Basic version
 *
 ******************************************************************************/

#ifndef _TRST_H_
#define _TRST_H_

#include "DEV_Config.h"
#include "LCD_1in28.h"
#include "GUI_Paint.h"
#include "GUI_BMP.h"
#include "display.h"
#include "image.h"
#include <stdio.h>  //printf()
#include <stdlib.h> //exit()
#include <signal.h> //signal()

void LCD_0IN96_test(void);
void LCD_1IN14_test(void);
void LCD_1IN28_test(void);
void LCD_1IN3_test(void);
void LCD_1IN54_test(void);
void LCD_1IN8_test(void);
void LCD_2IN_test(void);
void LCD_2IN4_test(void);
void LCD_init_logo(char *);

#endif