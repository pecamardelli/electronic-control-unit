#include "test.h"
#include <math.h>
#include <stdlib.h>     //exit()
#include <stdio.h>




int main(int argc, char *argv[])
{
    if (argc != 2){
        printf("please input LCD type!\r\n");
        printf("example: sudo ./main -1.3\r\n");
        exit(1);
    }
	
	double size;
    sscanf(argv[1],"%lf",&size);
    size = (fabs(size));
	
	if(size<0.1||size>10){
		printf("error: bad size\r\n");
		exit(1);
	}
	else printf("%.2lf inch LCD Moudule\r\n",size);
	
	if(size==0.96)LCD_0IN96_test();
	else if(size==1.14)LCD_1IN14_test();
	else if(size==1.28)LCD_1IN28_test();
	else if(size==1.3)LCD_1IN3_test();
	else if(size==1.54)LCD_1IN54_test();
	else if(size==1.8)LCD_1IN8_test();
	else if(size==2)LCD_2IN_test();
	else if(size==2.4)LCD_2IN4_test();
	else	printf("error: can not find the LCD\r\n");
    return 0;
}
