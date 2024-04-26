#include "stm32f10x.h"
#include "stdint.h"

#define LEDCOUNT   30
#define GLOBAL      0  //brightness
#define RED         3
#define GREEN       2
#define BLUE        1

void setupSPI(void);
void spi_send(uint8_t data);

uint8_t LED_ARRAY[LEDCOUNT][4];

void clearArray(void);
void skSetLed(uint8_t index, uint8_t global, uint8_t red, uint8_t green, uint8_t blue);
void skUpdate(void);


int main(void)
{
	setupSPI();
	clearArray();
	
	skSetLed(0,31,0,0,0);
	skUpdate();
	
	while(1)
	{
		
		int i;
		for(i=0; i<LEDCOUNT; i++)
		{
			skSetLed(i,31,0,255,0);
			skUpdate();
			for(int x=0; x<10000; x++) //  sorry for this delay :) 
			skSetLed(i,0,0,0,0);
			skUpdate();
		}

	}
}
void setupSPI(void)
{
	//turn on the clocks
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	//gpio setup
	GPIOA->CRL |= GPIO_CRL_MODE5;      //pin5 output mode 50Mhz
	GPIOA->CRL |= GPIO_CRL_CNF5_1;     
	GPIOA->CRL &= ~GPIO_CRL_CNF5_0;    // alternate func output push-pull 
	
	GPIOA->CRL |= GPIO_CRL_MODE7;      //pin7 output mode 50Mhz
	GPIOA->CRL |= GPIO_CRL_CNF7_1;     
	GPIOA->CRL &= ~GPIO_CRL_CNF7_0;    // alternate func output push-pull 

	//configure spi
	
	SPI1->CR1 |= SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | SPI_CR1_SSM | SPI_CR1_MSTR | SPI_CR1_CPHA | SPI_CR1_CPOL;
	SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1;  //baud rate clk/16
	SPI1->CR2 |= SPI_CR2_SSOE;
	
	SPI1->CR1 |= SPI_CR1_SPE;  //enable spi
	
}

void spi_send(uint8_t data)
{
	SPI1->DR = data;
	while( (SPI1->SR & SPI_SR_TXE) != SPI_SR_TXE);

}

void skSetLed(uint8_t index, uint8_t global, uint8_t red, uint8_t green, uint8_t blue)
{
	LED_ARRAY[index][GLOBAL] = (0xE0 | global);
	LED_ARRAY[index][RED] = red;
	LED_ARRAY[index][GREEN] = green;
	LED_ARRAY[index][BLUE] = blue;
}

void clearArray(void)
{
	int i,j;
	for(i=0; i<LEDCOUNT; i++)
	{
		LED_ARRAY[i][GLOBAL] = 0xE0;
		for(j=1; j<4; j++)
		{
			LED_ARRAY[i][j] = 0x00;
		}
	}
}

void skUpdate(void)
{
	//send start frame
	uint8_t i,j;
	for(i=0; i<4; i++)
	{
		spi_send(0x00);
	}
	
	//led frames
	for(i=0; i<LEDCOUNT; i++)
	{
		for(j=0; j<4; j++)
		{
			spi_send(LED_ARRAY[i][j]);
		}
	}
	
	//end frame
	for(i=0; i<4; i++)
	{
		spi_send(0xFF);
	}
	
}