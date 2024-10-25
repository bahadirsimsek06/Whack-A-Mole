#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "i2c-lcd.h"
#include <stdio.h>

int led0=0;
int led1=0;
int led2=0;

int button0=0;
int button1=0;
int button2=0;

int score0=0;
int score1=0;
int score2=0;

int score_total = 0 ;

int x =1;
int y =1;

int lvl = 1;
int try_count = 3;
int hile_counter = 1;
int h = 1;
int time_counter = 0;

uint32_t random1;
uint32_t random2;


GPIO_InitTypeDef GPIO_InitStruct;
EXTI_InitTypeDef EXTI_InitStruct;
NVIC_InitTypeDef NVIC_InitStruct;
I2C_InitTypeDef I2C_InitStruct;

void delay(uint32_t time){
	while(time--);
}

void GPIO_Config(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
			GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOE, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3 ;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOD, &GPIO_InitStruct);


	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);	// SCL pin
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); // SDA pin

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // Pin6 SCL, Pin7 SDA
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void I2C_Config()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	I2C_InitStruct.I2C_Ack = ENABLE;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = 400000;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;

	I2C_Init(I2C1, &I2C_InitStruct);

	I2C_Cmd(I2C1, ENABLE);
}

void I2C_Write(uint8_t address, uint8_t data)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));  // I2C Meþgul olduðu sürece bekle

	I2C_GenerateSTART(I2C1, ENABLE);	// I2C Haberleþmesini baþlattýk.

	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, address, I2C_Direction_Transmitter);

	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ));

	I2C_SendData(I2C1, data);

	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTOP(I2C1, ENABLE);
}

void lcd_send_int(int num)
{
	char str[10];
	delay(10);
	sprintf(str, "%d", num);
	lcd_sende_string(str);
}

void RNG_Config()
{
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

	RNG_Cmd(ENABLE);
}


void EXTI_Config()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, GPIO_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, GPIO_PinSource1);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, GPIO_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, GPIO_PinSource3);

	EXTI_InitStruct.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line2 | EXTI_Line3;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;

	EXTI_Init(&EXTI_InitStruct);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruct);
}

void EXTI0_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){

		button0 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
		led0 = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_0);

		if(led0==1){
			if (button0 ==led0) {
			score0 = score0 + (1*h);

			}
		}
		else if(button0 !=led0) {
			score0 = score0 - 1;
			try_count = try_count - 1 ;
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void EXTI1_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line1) != RESET){

		button1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
		led1 = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1);



		if(led1==1){



			  if (button1 ==led1) {
			  score1 = score1 + (1*h);

			  }

		}
		else if(button1 !=led1) {
				score1 = score1 - 1;
				try_count -- ;
		}



		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

void EXTI2_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line2) != RESET){

		button2 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);
		led2 = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_2);



		if(led2==1){



			  if (button2 ==led2) {
			  score2 = score2 + (1*h);

			  }

		}
		else if(button2 !=led2) {
				score2 = score2 - 1;
				try_count -- ;
		}



		EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

void EXTI3_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line3) != RESET){

		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3))
			  {
				  while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3));
				  delay(1680000);

				  hile_counter++;
			  }





	}

	EXTI_ClearITPendingBit(EXTI_Line3);
}

void greeting();
void showLED(int i) ;



int main(void){
  	GPIO_Config();
	EXTI_Config();
	RNG_Config();
	I2C_Config();
	lcd_init();

	//ssssslcd_send_cmd(0x01);

	greeting();


	lcd_send_cmd(0x80);
	lcd_sende_string("SCR");
	lcd_send_cmd(0x8D);
	lcd_sende_string("LVL");

	lcd_send_cmd(0x86);///??????????????????????????????????
	lcd_send_data(0);
	lcd_send_cmd(0x87);
	lcd_send_data(0);
	lcd_send_cmd(0x88);
	lcd_send_data(0);

  while (1)
  {
	  random1 = RNG_GetRandomNumber();
	  random2= random1 %8;

	  if(try_count > 0){



	  switch (random2){

	      case 0://leds=[0,0,0,0];
	    	  GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	    	  showLED(0) ;
	    	  break;

	      case 1://leds=[0,0,0,1]
	    	  GPIO_SetBits( GPIOB, GPIO_Pin_0);
	    	  delay(10000000*y*x);
	    	  GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	    	  showLED(1) ;
	    	  break;

	      case 2 ://leds=[0,0,1,0]
	    	  GPIO_SetBits(GPIOB,  GPIO_Pin_1);
	    	  delay(10000000*y*x);
	    	  GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	    	  showLED(2) ;

	    	  break;

	      case 3 ://leds=[0,0,1,1]
	    	  GPIO_SetBits(GPIOB,  GPIO_Pin_0| GPIO_Pin_1);
	    	  delay(10000000*x*y);
	    	  GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	    	  showLED(3) ;
	    	  break;

	      case 4 ://leds=[0,1,0,0]
	      	  GPIO_SetBits(GPIOB,  GPIO_Pin_2);
	      	  delay(10000000*x*y);
	      	  GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	    	  showLED(4) ;
	      	  break;

	      case 5 ://leds=[0,1,0,1]
	      	 GPIO_SetBits(GPIOB,  GPIO_Pin_0| GPIO_Pin_2);
	      	 delay(10000000*x*y);
	      	 GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	    	 showLED(5) ;
	      	 break;

	      case 6 ://leds=[0,1,1,0]
	      	 GPIO_SetBits(GPIOB,  GPIO_Pin_1| GPIO_Pin_2);
	      	 delay(10000000*x*y);
	      	 GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	      	 showLED(6) ;
	      	 break;

	      case 7 ://leds=[0,1,1,1]
			  GPIO_SetBits(GPIOB,  GPIO_Pin_0| GPIO_Pin_1| GPIO_Pin_2);
			  delay(10000000*x*y);
			  GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
			  showLED(7) ;
			  break;


/*
		  case 0 :leds=[0,1,0,0] break;
	      case 1 :leds=[0,1,0,0] break;
	      case 2 :leds=[0,1,0,0] break;
	      case 3 :leds=[0,1,0,0] break;
	      case 4 :leds=[0,1,0,0] break;

	      case 5 :leds=[0,1,0,1] break;
	      case 6 :leds=[0,1,1,0] break;
	      case 7 :leds=[0,1,1,1] break;

*/
	  default:
		  //led[]={0,0,0,0};// default statements
		  GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
		  delay(10000000*x*y);
		  showLED(0) ;
	  }

	  }
	  //**************************************

	  switch(h)
	 	  {
	 	  	  case 1:
	 	  	  {
	 	  		  GPIO_SetBits(GPIOD, GPIO_Pin_0);
	 	  		  GPIO_SetBits(GPIOE, GPIO_Pin_1 | GPIO_Pin_2);
	 	  		  GPIO_ResetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
	 	  		  delay(16800);
	 	  		  GPIO_ResetBits(GPIOD, GPIO_Pin_0);
	 	  		  GPIO_SetBits(GPIOD, GPIO_Pin_1);
	 	  		  GPIO_SetBits(GPIOE, GPIO_Pin_1 | GPIO_Pin_2);
	 	  		  GPIO_ResetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
	 	  		  delay(16800);
	 	  		  GPIO_ResetBits(GPIOD, GPIO_Pin_1);
	 	  		  GPIO_SetBits(GPIOD, GPIO_Pin_2);
	 	  		  GPIO_SetBits(GPIOE, GPIO_Pin_1 | GPIO_Pin_2);
	 	  		  GPIO_ResetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
	 	  		  delay(16800);
	 	  		 GPIO_ResetBits(GPIOD, GPIO_Pin_2);
	 	  		  GPIO_SetBits(GPIOD, GPIO_Pin_3);
	 	  		  GPIO_SetBits(GPIOE, GPIO_Pin_1 | GPIO_Pin_2);
	 	  		  GPIO_ResetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
	 	  		  delay(16800);
	 	  		 GPIO_ResetBits(GPIOD, GPIO_Pin_3);


	 	  		  break;
	 	  	  }
	 	  	  case 2:
	 	  	  {
	 	  		  	  	  	  GPIO_SetBits(GPIOD, GPIO_Pin_0);
	 	  			  		  GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5);
	 	  			  		  GPIO_ResetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_6);
	 	  			  		  delay(16800);
	 	  			  		GPIO_ResetBits(GPIOD, GPIO_Pin_0);
	 	  			  		  GPIO_SetBits(GPIOD, GPIO_Pin_1);
	 	  			  		  GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5);
	 	  			  		  GPIO_ResetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_6);
	 	  			  		  delay(16800);
	 	  			  		GPIO_ResetBits(GPIOD, GPIO_Pin_1);
	 	  			  		  GPIO_SetBits(GPIOD, GPIO_Pin_2);
	 	  			  		  GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5);
	 	  			  		  GPIO_ResetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_6);
	 	  			  		  delay(16800);
	 	  			  		GPIO_ResetBits(GPIOD, GPIO_Pin_2);
	 	  			  		  GPIO_SetBits(GPIOD, GPIO_Pin_3);
	 	  			  		  GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5);
	 	  			  		  GPIO_ResetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_6);
	 	  			  		  delay(16800);
	 	  			  		GPIO_ResetBits(GPIOD, GPIO_Pin_3);



	 	  		  break;
	 	  	  }
	 	  	  case 3:
	 	  	  {
	 	  		  GPIO_SetBits(GPIOD, GPIO_Pin_0);
	 	  		  GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_5);
	 	  		  GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_6);
	 	  		  delay(16800);
	 	  		GPIO_ResetBits(GPIOD, GPIO_Pin_0);
	 	  		  GPIO_SetBits(GPIOD, GPIO_Pin_1);
	 	  		GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_5);
	 	  		GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_6);
	 	  		  delay(16800);
	 	  		GPIO_ResetBits(GPIOD, GPIO_Pin_1);
	 	  		  GPIO_SetBits(GPIOD, GPIO_Pin_2);
	 	  		GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_5);
	 	  		GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_6);
	 	  		  delay(16800);
	 	  		GPIO_ResetBits(GPIOD, GPIO_Pin_2);
	 	  		  GPIO_SetBits(GPIOD, GPIO_Pin_3);
	 	  		GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_5);
	 	  		GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_6);
	 	  		  delay(16800);
	 	  		GPIO_ResetBits(GPIOD, GPIO_Pin_3);
	 	  		  break;
	 	  	  }
	 	  	  default:
	 	  		  GPIO_ResetBits(GPIOE, GPIO_Pin_All);

	 	  }

	  //**************************************
	  if(score_total <= 5){
		  lvl=1;
		  // Level 1
		 x  = 1.5;
	  }
	  else if(score_total <= 10 && score_total>5){
		  lvl = 2;
		  // Level 2
		  x  = 1.25;
	  }
	  else if(score_total <= 15 && score_total>10){
		  lvl = 3;
		  // Level 3
		  x  = 1.00;
	  }
	  else if (score_total>15){
		  lvl = 4;
		  //Level 4
		  x = 0.75;
	  }
	  //****************************************
	  switch(hile_counter){
	    case 1:
	    	h=1;
	    break;
	    case 2:
	    	h=2;
	    break;
	    case 3:
	    	h=3;
	    break;
	    case 4:
	    	h=2;
	    break;
	    case 5:
	    	h=1;
	    	time_counter=0;
	    break;
	    default:
	    hile_counter = 1;
	    time_counter=0;
	  }

	  if(h==2 | h==3){
    	 time_counter++;

      }

      if(time_counter >20){
    	  hile_counter=1;
    	  time_counter=0;
      }


	  //****************************************
	  //****************************************

	 score_total = score0 + score1 + score2 ;

//**********************
	 if(score_total <= 0) {
		 score_total=0;
	 }


	 if(try_count <= 0){
		 try_count=0;
		 }


// ***** score -> LCD *****
	  if(score_total >= 10){
		  lcd_send_cmd(0xC0);
		  lcd_send_int(score_total);
	  }
	  else  {
		  lcd_send_cmd(0xC0);
		  lcd_sende_string("0");
		  lcd_send_cmd(0xC1);
		  lcd_send_int(score_total);
	  }

// ***** LVL -> LCD *****
	  lcd_send_cmd(0xCE);
	  lcd_send_int(lvl);

// ***** try_count
	  switch(try_count){
	  case (3) :
	  	lcd_send_cmd(0x86);
	  	lcd_send_data(0);
	  	lcd_send_cmd(0x87);
	  	lcd_send_data(0);
	  	lcd_send_cmd(0x88);
	  	lcd_send_data(0);
	  	break;

	  case (2) :
		lcd_send_cmd(0x86);
		lcd_send_data(0);
		lcd_send_cmd(0x87);
		lcd_send_data(0);
		lcd_send_cmd(0x88);
		lcd_sende_string(" ");
		break;

	  case (1) :
		lcd_send_cmd(0x86);
		lcd_send_data(0);
		lcd_send_cmd(0x87);
		lcd_sende_string(" ");
		lcd_send_cmd(0x88);
		lcd_sende_string(" ");
		break;

	  case (0) :
		lcd_send_cmd(0x85);
		lcd_sende_string("GAME");
		lcd_send_cmd(0xC5);
		lcd_sende_string(" OVER");
		break;

	  default :
		  ;
	  }
  }
}

void greeting() {
	// straight flow
	for(int i=0; i<10 ; i++){
		if(i==0){
			lcd_send_cmd(0x80 + i);
			lcd_send_data(1);
			lcd_send_cmd(0x8F - i);
			lcd_send_data(1);
		}
		if(i<10 &&  0 < i){
			lcd_send_cmd(0x80 + i);
			lcd_send_data(1);
			lcd_send_cmd(0x8F - i);
			lcd_send_data(1);
			// bottom line
			lcd_send_cmd(0xC0 + (i-1));
			lcd_send_data(1);
			lcd_send_cmd(0xCF - (i-1));
			lcd_send_data(1);
		}
		if(i>2 && i < 11){
			lcd_send_cmd(0x80 +(i-3));
			lcd_sende_string(" ");
			lcd_send_cmd(0x8F - (i-3));
			lcd_sende_string(" ");
			// bottom line
			if(i==10){
				lcd_send_cmd(0xC0 + (i-1));
				lcd_send_data(1);
				lcd_send_cmd(0xCF - (i-1));
				lcd_send_data(1);
			}
			lcd_send_cmd(0xC0 +(i-4));
			lcd_sende_string(" ");
			lcd_send_cmd(0xCF - (i-4));
			lcd_sende_string(" ");
		}
		if(i==11){
			lcd_send_cmd(0xC0 +(i-4));
			lcd_sende_string(" ");
			lcd_send_cmd(0xCF - (i-4));
			lcd_sende_string(" ");
		}
		delay(2100000);
	}
	// inverse flow
	for(int i=9; i<20 ; i++) {
		if(i==9){
			lcd_send_cmd(0xC0 +(i-3));
			lcd_sende_string(" ");
			lcd_send_cmd(0xCF - (i-3));
			lcd_sende_string(" ");
		}

		if(i<16){
			lcd_send_cmd(0x80 + i);
			lcd_send_data(1);
			lcd_send_cmd(0x8F - i);
			lcd_send_data(1);
			// bottom line
			lcd_send_cmd(0xC0 + (i-1));
			lcd_send_data(1);
			lcd_send_cmd(0xCF - (i-1));
			lcd_send_data(1);
		}
		if(i == 16){
			lcd_send_cmd(0xC0 + (i-1));
			lcd_send_data(1);
			lcd_send_cmd(0xCF - (i-1));
			lcd_send_data(1);
		}

		if(i>10 && i < 19){
			lcd_send_cmd(0x80 +(i-3));
			lcd_sende_string(" ");
			lcd_send_cmd(0x8F - (i-3));
			lcd_sende_string(" ");
			// bottom line
			lcd_send_cmd(0xC0 + (i-4));
			lcd_sende_string(" ");
			lcd_send_cmd(0xCF - (i-4));
			lcd_sende_string(" ");
		}

		if(i == 19){
			lcd_send_cmd(0xC0 + (i-4));
			lcd_sende_string(" ");
			lcd_send_cmd(0xCF - (i-4));
			lcd_sende_string(" ");
		}

		if(i == 11){
			lcd_send_cmd(0x88);
			lcd_sende_string("A");
			lcd_send_cmd(0x87);
			lcd_sende_string("H");
		}

		if(i == 12){
			lcd_send_cmd(0x89);
			lcd_sende_string("C");
			lcd_send_cmd(0x86);
			lcd_sende_string("W");
			//bottom line
			lcd_send_cmd(0xC8);
			lcd_sende_string("O");
			lcd_send_cmd(0xC7);
			lcd_sende_string("M");
		}

		if(i == 13){
			lcd_send_cmd(0xC9);
			lcd_sende_string("L");
			lcd_send_cmd(0xC6);
			lcd_sende_string(" ");
		}

		if(i == 14){
			lcd_send_cmd(0xCA);
			lcd_sende_string("E");
			lcd_send_cmd(0xC5);
			lcd_sende_string("A");
		}

		if (!(i == 11)) delay(2100000);
	}
	// blink
	for(int i= 0; i < 3; i++){
	lcd_send_cmd(0x86);
	lcd_sende_string("WHAC");
	lcd_send_cmd(0xC5);
	lcd_sende_string("A MOLE");
	delay(3000000);
	lcd_send_cmd(0x01);
	delay(3000000);
	}
}

void showLED(int i) {

	switch(i){
	case(0):
		for(int k = 0; k<3 ; k++){
		lcd_send_cmd(0xC5 + 2*k);
		lcd_send_data(2);
		}
	break;

	case (1):
		for(int k = 0; k<3 ; k++){
			if(k!=0){
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(2); 				// OFF LED
			}
			else{
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(3);				// ON LED
			}
		}
		break;

	case (2):
		for(int k = 0; k<3 ; k++){
			if(k!=1){
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(2);
			}
			else{
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(3);
			}
		}
		break;

	case (3):
		for(int k = 0; k<3 ; k++){
			if(k == 2){
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(2);
			}
			else{
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(3);
			}
		}
		break;

	case (4):
			for(int k = 0; k<3 ; k++){
			if(k!=2){
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(2);
			}
			else{
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(3);
			}
		}
		break;

	case (5):
			for(int k = 0; k<3 ; k++){
			if(k == 1){
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(2);
			}
			else{
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(3);
			}
		}
		break;

	case (6):
			for(int k = 0; k<3 ; k++){
			if(k == 0){
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(2);
			}
			else{
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(3);
			}
		}
		break;

	case (7):
			for(int k = 0; k<3 ; k++){
			lcd_send_cmd(0xC9 - 2*k);
			lcd_send_data(3);
			}
		}
	}


void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size)
{
  /* TODO, implement your code here */
  return;
}

uint16_t EVAL_AUDIO_GetSampleCallBack(void)
{
  /* TODO, implement your code here */
  return -1;
}
