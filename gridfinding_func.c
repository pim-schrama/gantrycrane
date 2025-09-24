#include <avr/io.h>
#include "gridfinding_def.h"

// Vergelijkingen
int xNu_TOV_xEind(int nu, int eind) {
    if (nu > eind)  return 1;   // boven eind
    if (nu < eind)  return 0;   // onder eind
    return 2;                   // gelijk
}

int yNu_TOV_yEind(int nu, int eind) {
    if (nu > eind)  return 1;
    if (nu < eind)  return 0;
    return 2;
}

// Motorsturing
int motorX(int richting) {
    printf("motorX_in\n");
    switch (richting) {
        case 0: // rechtsom
            while (xNu != xEind)portHBrug_X &= ~(1 << pinHBrug_RechtsOm_X);
            portHBrug_X |=  (1 << pinHBrug_RechtsOm_X);
            return 1;

        case 1: // linksom
            while (xNu != xEind) portHBrug_X &= ~(1 << pinHBrug_LinksOm_X);
            portHBrug_X |=  (1 << pinHBrug_LinksOm_X);
            return 1;

        case 2: // stil
        default:
            return 0;
    }
}

int motorY(int richting) {
    printf("motorY_in\n");
    switch (richting) {
        case 0: // rechtsom
            while (yNu != yEind) portHBrug_Y &= ~(1 << pinHBrug_RechtsOm_Y);
            portHBrug_Y |=  (1 << pinHBrug_RechtsOm_Y);
            return 1;

        case 1: // linksom
            while (yNu != yEind) portHBrug_Y &= ~(1 << pinHBrug_LinksOm_Y);
            portHBrug_Y |=  (1 << pinHBrug_LinksOm_Y);
            return 1;

        case 2: // stil
        default:
            return 0;
    }
}

// Machine naar (1,1) sturen
void homeSender(void) {
    printf("homesender_in\n");
    while (xNu != 1) portHBrug_X &= ~(1 << pinHBrug_LinksOm_X);
    portHBrug_X |=  (1 << pinHBrug_LinksOm_X);

    while (yNu != 1) portHBrug_Y &= ~(1 << pinHBrug_LinksOm_Y);
    portHBrug_Y |=  (1 << pinHBrug_LinksOm_Y);

    portHBrug_X &=  ~(1 << pinHBrug_LinksOm_X);
    portHBrug_X &=  ~(1 << pinHBrug_LinksOm_Y);

    homeSenderDone = 1;

    printf("homesender_out\n");
}

void motorenUit(void){
    portHBrug_X |=  (1 << pinHBrug_RechtsOm_X);
    portHBrug_X |=  (1 << pinHBrug_LinksOm_X);
    portHBrug_Y |=  (1 << pinHBrug_RechtsOm_Y);
    portHBrug_Y |=  (1 << pinHBrug_LinksOm_Y);
    portHBrug_Z |=  (1 << pinHBrug_RechtsOm_Z);
    portHBrug_Z |=  (1 << pinHBrug_LinksOm_Z);

    portMagneet &= ~(1 << pinMagneet);
}
