#include <avr/io.h>
#include <util/delay.h>
#include "gridfinding_def.h"

int coordSwitchCount = 0;

int ampereSensor(int ampereSensor){
    float voltageZAxis = ampereSensor * (5.0 / 1023.0);
    return ((voltageZAxis - 2.5) / -0.185);
}

void coord_switch(void){
                                                                                         printf("coord_switch\n");
    switch(coordSwitchCount){

        case 0:
            xEnd = xEndDropOff;
            yEnd = yEndDropOff;
            if(!(PIN_SwitchSecondCoord & (1 << pin_SwitchSecondCoord))) {
                // switch pressed = ON = two coordinates
                coordSwitchCount = 1;
                secondObjectDown = 1;
            }
            backAndForth = 2;
        break;

        case 1:
            xEnd = xEnd2;
            yEnd = yEnd2;
            coordSwitchCount = 2;
            backAndForth = 1;
        break;

        case 2:
            xEnd = xEndDropOff2;
            yEnd = yEndDropOff2;
            coordSwitchCount = 1;
            secondObjectDown = 0;
            backAndForth = 2;
        break;
    }
}


int pickup_program(enum MagnetState state){
    if(state == Get) { port_Magnet |= (1 << pin_Magnet);                                             printf("magneet_aan\n");}
    if(state == Drop) { port_Magnet &= ~(1 << pin_Magnet);                                           printf("magneet_uit\n");}
    return 0;
}

void end_program(void){
                                                                                                     printf("end_program\n");
    backAndForth = 1;
    coordSwitchCount = 0;
    secondObjectDown = 0;
    inputEndPosRetrieved = 0;
    inputEndPosRetrieved2 = 0;
    homeing_program();
    motors_off();
    startBlock = 0;
    homeSenderDone = 0;
    startButton = 0;
}

void z_axis_routine(upDown){
                                                                                                    printf("Z_beneden\n");
        while (currentZAxis < resistanceCurrentZAxis){
            currentZAxis = ampereSensor(adc_average(1, adcSamples));
                                                                                                    printf("ampere z-as = %.2f\n",currentZAxis);
            port_HBridgeZRight |= (1 << pin_HBridgeRightZ);
        }
        port_HBridgeZRight &= ~(1 << pin_HBridgeRightZ);
        currentZAxis = 0;

        pickup_program(upDown); //object vast
                                                                                                     printf("Z_omhoog\n");
        while (currentZAxis < resistanceCurrentZAxis) {
            currentZAxis = ampereSensor(adc_average(1, adcSamples));
                                                                                                    printf("ampere z-as = %.2f\n",currentZAxis);
            port_HBridgeZLeft |= (1 << pin_HBridgeLeftZ);
        }
        port_HBridgeZLeft &= ~(1 << pin_HBridgeLeftZ);
        currentZAxis = 0;

        coord_switch();

}

int motor_z_axis(int upDown) {  //links-/rechts-om zorgen bij de z-as voor en beweging om-hoog/-laag.
                                                                                                     printf("motor_z_axis_in\n");
    if(upDown == 1) z_axis_routine(upDown);

    if(upDown == 2){

        z_axis_routine(upDown);

        if((PIN_SwitchSecondCoord & (1 << pin_SwitchSecondCoord))) {

            end_program();
        } else if((secondObjectDown == 0) && (!(PIN_SwitchSecondCoord & (1 << pin_SwitchSecondCoord)))) {

            end_program();
        }
    }
    return 0;
}

