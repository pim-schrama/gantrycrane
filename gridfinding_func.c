#include <avr/io.h>
#include "gridfinding_def.h"

// Vergelijkingen
int xNow_to_xEnd_comp(int nu, int eind) {
    if (nu > eind)  return 1;   // boven eind
    if (nu < eind)  return 0;   // onder eind
    return 2;                   // gelijk
}

int yNow_to_yEnd_comp(int nu, int eind) {
    if (nu > eind)  return 1;
    if (nu < eind)  return 0;
    return 2;
}

int motor_x_axis(int richting) {
    // Beide kanten eerst uit
    port_HBridgeX &= ~((1 << pin_HBridgeRightX) | (1 << pin_HBridgeLeftX));

    switch (richting) {
        case 0: // rechtsom
            port_HBridgeX |= (1 << pin_HBridgeRightX);
            break;

        case 1: // linksom
            port_HBridgeX |= (1 << pin_HBridgeLeftX);
            break;

        case 2: // stil
            port_HBridgeX &= ~((1 << pin_HBridgeRightX) | (1 << pin_HBridgeLeftX));
            break;

        default:
            // alles al uit, niets doen
            return 0;
    }
}

int motor_y_axis(int richting) {
    // Beide kanten eerst uit
    port_HBridgeY &= ~((1 << pin_HBridgeRightY) | (1 << pin_HBridgeLeftY));

    switch (richting) {
        case 0: // rechtsom
            port_HBridgeY |= (1 << pin_HBridgeRightY);
            break;

        case 1: // linksom
            port_HBridgeY |= (1 << pin_HBridgeLeftY);
            break;

        case 2: // stil
            port_HBridgeY &= ~((1 << pin_HBridgeRightY) | (1 << pin_HBridgeLeftY));
            break;

        default:
            // alles al uit, niets doen
            return 0;
    }
}


// Machine naar (1,1) sturen
void homeing_program(void) {

    int homeX = 0 , homeY = 0;

    while(homeX != 1 || homeY != 1){
                                                                              printf("homeing_program_in\n");

        if (xNow != 1){ port_HBridgeX |= (1 << pin_HBridgeLeftX);}


        if (xNow == 1) {
            port_HBridgeX &= ~(1 << pin_HBridgeLeftX);
            homeX = 1;
        }

        if (yNow != 1){ port_HBridgeY |= (1 << pin_HBridgeLeftY);}

        if (yNow == 1) {
            port_HBridgeY &= ~(1 << pin_HBridgeLeftY);
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
    port_HBridgeZRight &= ~(1 << pin_HBridgeRightZ);
    port_HBridgeZLeft &= ~(1 << pin_HBridgeLeftZ);
    pwmX_stop();
    pwmY_stop();

    //port_Magnet &= ~(1 << pin_Magnet);
}

