/*
 * Dimming a LED with the help of an analog XY joystick
 * @author: T.Warnakulnasooriya
 * @date: 23.05.2022
 */

#include  "inc/tm4c1294ncpdt.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "int_handler.h"
#include <math.h>

void configurePorts();
void configureADC();
void configureTimer();
unsigned long ADC0_InSeq0();
void wait();

int main(void) {

    double data;
    int middle = (pow(2, 12)) / 2;

    configurePorts();
    configureADC();
    configureTimer();

    while (1) {
        data = (double) ADC0_InSeq0() / 1000;
        if (data > (middle + 200) / 1000) {
            //check if duty cycle already 95%
            if ((800 - 1) == TIMER2_TAMATCHR_R) {
            } else {
                //increase match value by 5% (1/20 of ILR)
                TIMER2_TAMATCHR_R -= 800;
            }
            printf("Lever to the right: %.3lf mV", data);
            printf("\n");
        } else if (data < (middle - 200) / 1000) {
            if ((15200 - 1) == TIMER2_TAMATCHR_R) {

            } else {
                TIMER2_TAMATCHR_R += 800;
            }
            printf("Lever to the left: %.3lf mV", data);
            printf("\n");
        }
        wait(1e5);
        TIMER2_ICR_R = 0x10;
        printf("Lever in the middle: %.3lf mV", data);
        printf("\n");
    }
}


void configurePorts() {
    //Clock ports E,M
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; //Port E
    while (!(SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R4)); //check for ports ready
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11; //Port M
    while (!(SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R11)); //check for ports ready
    // busy wait loop till ports are ready
    // while(SYSCTL_PRGPIO_R & 0xC00 == 0x810);

    //set M(0) as output//M(1) input
    GPIO_PORTM_DIR_R |= 0x01;
    GPIO_PORTM_AFSEL_R |= 0x01; //alternate function
    GPIO_PORTM_PCTL_R |= 0x03; // MUX0 connects pin M(0) to timer2a

    //digital enable pins M(1:0) and digital disable E(0)
    GPIO_PORTM_DEN_R |= 0x03;

    GPIO_PORTE_AHB_DEN_R &= ~(0x01);

    GPIO_PORTM_IS_R &= ~(0x02); //edge sensitive
    //or is it ~(0x01)???M(1)?
    GPIO_PORTM_IBE_R &= ~(0x02); //sensitive to one edge
    GPIO_PORTM_IEV_R |= 0x02; // sensitive to rising
    GPIO_PORTM_ICR_R |= 0x02; //clearing at the beginning
    GPIO_PORTM_IM_R |= 0x02; // unmask interrupt of this pin
    NVIC_EN2_R |= 1 << (72 - 64); // send to nvic interrupt number 72

    //analog input functionality of E(0)
    GPIO_PORTE_AHB_AFSEL_R |= 0x01; //alternative function//doesnt seem to be necessary
    GPIO_PORTE_AHB_DEN_R &= ~(0x01);//deactivating digital branch first
    GPIO_PORTE_AHB_AMSEL_R |= 0x01; //analog enable
    GPIO_PORTE_AHB_DIR_R &= ~(0x01); //inputs
}

void configureADC() {
    SYSCTL_RCGCADC_R |= (1 << 0);//configure adc0
    while (!(SYSCTL_PRADC_R & 0x01)); //Ready?
    //magic code?
    SYSCTL_PLLFREQ0_R |= (1 << 23); // PLL Power
    while (!(SYSCTL_PLLSTAT_R & 0x01)); // until PLL has locked
    ADC0_CC_R |= 0x01;
    wait(5); // PIOSC for ADC sampling clock
    SYSCTL_PLLFREQ0_R &= ~(1 << 23); // PLL Power off

    ADC0_SSMUX0_R |= 0x00000003;//sequencer 0, channel AIN3 (Port E(0))
    ADC0_SSEMUX0_R |= 0x00; //because AIN3 is less than AIN15
    ADC0_SSCTL0_R |= 0x00000002;//END0 set ->sequence length = 1
    ADC0_ACTSS_R |= 0x01;//enable  sample sequencer ADC0_SS0
}

unsigned long ADC0_InSeq0(void) {
    unsigned long data;
    ADC0_PSSI_R |= 0x01; // Start ADC0
    // wait for FIFO "NON-EMPTY"
    while (ADC0_SSFSTAT0_R & 0x000000100);
    // Take from FIFO
    data = (unsigned long) ADC0_SSFIFO0_R & 0xFFF;
    return data;
}

void configureTimer() {
    SYSCTL_RCGCTIMER_R |= 0x04;
    while (!(SYSCTL_PRTIMER_R & 0x04));
    //16 bit
    TIMER2_CFG_R = 0x04;
    //Match enabled,downwards direction and one-shot
    TIMER2_TAMR_R = 0x2A;
    //prescaler not supported
    //TIMER2_TAPR_R = 0x00;
    //Load value 16000 to get time-out after 1ms (->1kHz duty cycle)
    TIMER2_TAILR_R = 16000 - 1;
    //default match value at 8000 for 50% duty cycle
    TIMER2_TAMATCHR_R = 8000 - 1;
    TIMER2_CTL_R = 0x01; //enable
}

void wait(int value) {
    int counter;
    for (counter = 0; counter < value; counter++);
}
