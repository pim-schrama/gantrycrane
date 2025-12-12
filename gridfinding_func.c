#include <avr/io.h>
#include <util/delay.h>
#include "gridfinding_def.h"

int prevDirectionX = 0, prevDirectionY = 0;

// Vergelijkingen
int xNow_to_xEnd_comp(int now, int end) {
    if (now > end)  return 1;   // boven eind
    if (now < end)  return 0;   // onder eind
    return 2;                   // gelijk
}

int yNow_to_yEnd_comp(int now, int end) {
    if (now > end)  return 1;
    if (now < end)  return 0;
    return 2;
}

int motor_x_axis(int direction) {

    if(direction != prevDirectionX){                                                    // Beide kanten eerst uit
        port_HBridgeX &= ~((1 << pin_HBridgeRightX) | (1 << pin_HBridgeLeftX));
        _delay_us(5);                                                                   // delay om snelschakelen te voorkomen
    }

    prevDirectionX = direction;

    switch (direction) {
        case 0: // rechtsom
            port_HBridgeX |= (1 << pin_HBridgeRightX);
            break;

        case 1: // linksom
            port_HBridgeX |= (1 << pin_HBridgeLeftX);
            break;

        case 2: // stil
            port_HBridgeX &= ~((1 << pin_HBridgeRightX) | (1 << pin_HBridgeLeftX));
            break;

        default: // alles al uit, niets doen / stil
            port_HBridgeX &= ~((1 << pin_HBridgeRightX) | (1 << pin_HBridgeLeftX));
            return 0;
    }
    return 0;
}

int motor_y_axis(int direction) {

    if(direction != prevDirectionY){                                                    // Beide kanten eerst uit
        port_HBridgeY &= ~((1 << pin_HBridgeRightY) | (1 << pin_HBridgeLeftY));
        _delay_us(5);                                                                       // delay om snelschakelen te voorkomen
    }

    prevDirectionY = direction;

    switch (direction) {
        case 0: // rechtsom
            port_HBridgeY |= (1 << pin_HBridgeRightY);
            break;

        case 1: // linksom
            port_HBridgeY |= (1 << pin_HBridgeLeftY);
            break;

        case 2: // stil
            port_HBridgeY &= ~((1 << pin_HBridgeRightY) | (1 << pin_HBridgeLeftY));
            break;

        default: // alles al uit, niets doen / stil
            port_HBridgeY &= ~((1 << pin_HBridgeRightY) | (1 << pin_HBridgeLeftY));
            return 0;
    }
    return 0;
}


// Machine naar (1,1) sturen
void homeing_program(void) {
                                                                            printf("homeing_program_in\n");
    int homeX = 0 , homeY = 0;

    x_pos_finder();
    y_pos_finder();

    while(homeX != 1 || homeY != 1){

        if (xNow != 1){ port_HBridgeX |= (1 << pin_HBridgeLeftX);}


        if (xNow == 1) {
            port_HBridgeX &= ~(1 << pin_HBridgeLeftX);
            homeX = 1;
        }

        if (yNow != 1){ port_HBridgeY |= (1 << pin_HBridgeLeftY);}

        if (yNow == 1) {
            port_HBridgeY &= ~(1 << pin_HBridgeLeftY);
            //  Tegen overshoot van de  switch
            port_HBridgeY |= (1 << pin_HBridgeRightY);
            _delay_ms(25);
            port_HBridgeY &= ~(1 << pin_HBridgeRightY);
            homeY = 1;
        }

    }
    port_HBridgeX &=  ~(1 << pin_HBridgeLeftX);
    port_HBridgeY &=  ~(1 << pin_HBridgeLeftY);

    homeSenderDone = 1;

                                                                             printf("homeing_program_out\n");
}

void motors_off(void){
    port_HBridgeX &= ~((1 << pin_HBridgeRightX) | (1 << pin_HBridgeLeftX));
    port_HBridgeY &= ~((1 << pin_HBridgeRightY) | (1 << pin_HBridgeLeftY));
    port_HBridgeZ &= ~((1 << pin_HBridgeRightZ) | (1 << pin_HBridgeLeftZ));
    pwmX_stop();
    pwmY_stop();

    //port_Magnet &= ~(1 << pin_Magnet);
}

