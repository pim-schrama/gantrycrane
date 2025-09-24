#include <avr/io.h>
#include <util/delay.h>
#include "gridfinding_def.h"

int coordSwitchCount = 0;

void coordSwitch(void){
    printf("coord_switch\n");
    switch(coordSwitchCount){

        case 0:
            xEind = xEindDropOf;
            yEind = yEindDropOf;
            if(PIN_SwitchTweedeCoord & (1 << pinSwitchTweedeCoord)){
                coordSwitchCount = 1;
                tweedeBlokjeNeer = 1;
            }
            heenTerug = 2;
        break;

        case 1:
            xEind = xEind2;
            yEind = yEind2;
            coordSwitchCount = 2;
            heenTerug = 1;
        break;

        case 2:
            xEind = xEindDropOf2;
            yEind = yEindDropOf2;
            coordSwitchCount = 1;
            tweedeBlokjeNeer = 0;
            heenTerug = 2;
        break;
    }

}

int oppakProgramma(enum MagnetState state){
    if(state == Get) { portMagneet |= (1 << pinMagneet); printf("magneet_aan\n");}
    if(state == Drop) { portMagneet &= ~(1 << pinMagneet); printf("magneet_uit\n");}

    return 0;
}

void eindProgramma(void){
    printf("Eindprogramma\n");
    heenTerug = 1;
    coordSwitchCount = 0;
    infoEindPosOpgehaald = 0;
    infoEindPosOpgehaald2 = 0;
    homeSender();
    motorenUit();
    startSlot = 0;
    homeSenderDone = 0;
    startKnop = 0;
}

int motorZ(int opNeer) {  //links-/rechts-om zorgen bij de z-as voor en beweging om-hoog/-laag.
    printf("motorZ_in\n");
    if(opNeer == 1){
        printf("Z_oppak_beneden\n");
        while ((PIN_pos_Z & (1 << pos_Z))) portHBrug_Z &= ~(1 << pinHBrug_RechtsOm_Z);
        portHBrug_Z |=  (1 << pinHBrug_RechtsOm_Z);

        oppakProgramma(opNeer); //object vast

        printf("Z_oppak_omhoog\n");
        while ((PIN_pos_Z & (1 << pos_Z2))) portHBrug_Z &= ~(1 << pinHBrug_LinksOm_Z);
        portHBrug_Z |=  (1 << pinHBrug_LinksOm_Z);

        coordSwitch();
    }

    if(opNeer == 2){
        printf("Z_neerzet_beneden\n");
        while ((PIN_pos_Z & (1 << pos_Z))) portHBrug_Z &= ~(1 << pinHBrug_LinksOm_Z);
        portHBrug_Z |=  (1 << pinHBrug_LinksOm_Z);

        oppakProgramma(opNeer); //object los

        printf("Z_neerzet_omhoog\n");
        while ((PIN_pos_Z & (1 << pos_Z2))) portHBrug_Z &= ~(1 << pinHBrug_RechtsOm_Z);
        portHBrug_Z |=  (1 << pinHBrug_RechtsOm_Z);

        if((PIN_SwitchTweedeCoord & (1 << pinSwitchTweedeCoord))){
           eindProgramma();
        }else  if((tweedeBlokjeNeer == 0) && !(PIN_SwitchTweedeCoord & (1 << pinSwitchTweedeCoord))){
                    eindProgramma();
        }


    }
    return 0;
}
