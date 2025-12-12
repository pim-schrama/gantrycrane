#include <avr/io.h>
#include <util/delay.h>
#include "gridfinding_def.h"

int coordSwitchCount = 0;
int coordSwitchCount2 = 1;
int check = 0;

float ampereSensor(int pin, int samples){
    int adc_value = adc_average(pin, samples);
    float voltageZAxis = adc_value * (5.0 / 1023.0);

    if(check < 15){check++; return 0;}
    else{return (((voltageZAxis - 2.5) / -0.185)*1000);}
}

void coord_switch(void){
    coordSwitchCount++;
                                                                                                printf("coord_switch\n");
    switch(coordSwitchCount % 2){

        case 1:
            xEnd = xEndArr[coordSwitchCount2];
            yEnd = yEndArr[coordSwitchCount2];
            backAndForth = 1;
        break;

        case 0:
            xEnd = xEndDropOffArr[coordSwitchCount2];
            yEnd = yEndDropOffArr[coordSwitchCount2];
            backAndForth = 2;
            coordSwitchCount2++;
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
    coordSwitchCount2 = 0;
    coordCount = 0;
    coordCountPickDrop = 0;
    inputEndPosRetrieved = 0;
    homeing_program();
    motors_off();
    startBlock = 0;
    homeSenderDone = 0;
    blockCountRecieved = 0;
    startButton = 0;
}

void z_axis_routine(upDown){

        check = 0;                                                                                  printf("Z_beneden\n");
        while (currentZAxis < zAxisNoResistance){
            port_HBridgeZ |= (1 << pin_HBridgeRightZ);
            currentZAxis = ampereSensor(1, adcSamples);
            int currentZAxis2 = currentZAxis;
                                                                                                    printf("ampere z-as = %d\n",currentZAxis2);
        }
        port_HBridgeZ &= ~(1 << pin_HBridgeRightZ);
        currentZAxis = 0;
        check = 0;

        pickup_program(upDown); //object vast
                                                                                                     printf("Z_omhoog\n");
        while (!(PIN_StartButton & (1 << pin_StartButton))/*currentZAxis < zAxisNoResistance*/) {
            port_HBridgeZ |= (1 << pin_HBridgeLeftZ);

            //currentZAxis = ampereSensor(adc_average(1, adcSamples));
        }
        port_HBridgeZ &= ~(1 << pin_HBridgeLeftZ);
        currentZAxis = 0;

        coord_switch();
}

int motor_z_axis(int upDown) {  //links-/rechts-om zorgen bij de z-as voor en beweging om-hoog/-laag.
                                                                                                     printf("motor_z_axis_in\n");
    if(upDown == 1) z_axis_routine(upDown);

    if(upDown == 2){

        z_axis_routine(upDown);

        if(coordSwitchCount2 == (2*blocks)) {

            end_program();
        }
    }
    return 0;
}

