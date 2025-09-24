#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "gridfinding_def.h"

int lastKey = -1;

enum InputState {
    IDLE,
    PICKUP_X,
    PICKUP_Y,
    DROPOFF_X,
    DROPOFF_Y,
    PICKUP_X2,
    PICKUP_Y2,
    DROPOFF_X2,
    DROPOFF_Y2,

};

enum InputState inputState = IDLE;

 enum key_pressed {
    D = 15,
    HASH = 14,
    ZERO = 13,
    STAR = 12,
    C = 11,
    NINE = 10,
    EIGHT = 9,
    SEVEN = 8,
    B = 7,
    SIX = 6,
    FIVE = 5,
    FOUR = 4,
    A = 3,
    THREE = 2,
    TWO = 1,
    ONE = 0,

};

// Positiebepaling
void xNuFinder(void) {
    if ((PIN_pos_XY & (1 << pos_X1)) == 0) {
        xNu = 1;
        printf("xNu_1\n");
    }
    if ((PIN_pos_XY & (1 << pos_X2)) == 0) {
        xNu = 2;
        printf("xNu_2\n");
    }
    if ((PIN_pos_XY & (1 << pos_X3)) == 0) {
        xNu = 3;
        printf("xNu_3\n");
    }
    if ((PIN_pos_XY & (1 << pos_X4)) == 0) {
        xNu = 4;
        printf("xNu_4\n");
    }
    if ((PIN_pos_XY & (1 << pos_X5)) == 0) {
        xNu = 5;
        printf("xNu_5\n");
    }
}

void yNuFinder(void) {
    if ((PIN_pos_XY & (1 << pos_Y1)) == 0) {
        yNu = 1;
        printf("yNu_1\n");
    }
    if ((PIN_pos_XY & (1 << pos_Y2)) == 0) {
        yNu = 2;
        printf("yNu_2\n");
    }
    if ((PIN_pos_XY & (1 << pos_Y3)) == 0) {
        yNu = 3;
        printf("yNu_3\n");
    }
    if ((PIN_pos_Y  & (1 << pos_Y4)) == 0) {
        yNu = 4;
        printf("yNu_4\n");
    }
    if ((PIN_pos_Y  & (1 << pos_Y5)) == 0) {
        yNu = 5;
        printf("yNu_5\n");
    }
}

// ---------------- HELPER ----------------
int key_to_number(int key) {
    switch (key) {
        case ONE:   return 1;
        case TWO:   return 2;
        case THREE: return 3;
        case FOUR:  return 4;
        case FIVE:  return 5;
        default:    return -1;
    }
}

// Scan keypad, return key index 0–15, or -1 if none
int keypad_getkey(void) {
    for (uint8_t row = 0; row < 4; row++) {
        // Drive one row LOW
        ROW_PORT |= 0x0F;
        ROW_PORT &= ~(1 << row);

        _delay_us(5); // settle time

        // Read columns (active LOW)
        uint8_t cols = ~COL_PIN & 0x0F;
        if (cols) {
            for (uint8_t col = 0; col < 4; col++) {
                if (cols & (1 << col)) {
                    return row * 4 + col; // key index 0–15
                }
            }
        }
    }
    return -1; // no key
}

// ---------------- STATE MACHINE ----------------
void processKey(int key) {
    switch (inputState) {
        case IDLE:
            if ((infoEindPosOpgehaald == 1) && (infoEindPosOpgehaald2 == 1)) {
                break; // already done
            }
            if (key == A) {
                printf("A pressed\n");
                inputState = PICKUP_X;
            } else if (key == B) {
                printf("B pressed\n");
                inputState = DROPOFF_X;
            } else if (key == C) {
                printf("C pressed\n");
                inputState = PICKUP_X2;
            } else if (key == D) {
                printf("D pressed\n");
                inputState = DROPOFF_X2;
            }
            break;

        case PICKUP_X: {
            int num = key_to_number(key);
            if (num != -1) {
                xEind = num;
                printf("Pickup X = %d\n", xEind);
                inputState = PICKUP_Y;
            }
            break;
        }
        case PICKUP_Y: {
            int num = key_to_number(key);
            if (num != -1) {
                yEind = num;
                printf("Pickup Y = %d\n", yEind);
                inputState = IDLE;
            }
            break;
        }

        case DROPOFF_X: {
            int num = key_to_number(key);
            if (num != -1) {
                xEindDropOf = num;
                printf("Dropoff X = %d\n", xEindDropOf);
                inputState = DROPOFF_Y;
            }
            break;
        }
        case DROPOFF_Y: {
            int num = key_to_number(key);
            if (num != -1) {
                yEindDropOf = num;
                printf("Dropoff Y = %d\n", yEindDropOf);
                infoEindPosOpgehaald = 1;
                inputState = IDLE;
            }
            break;
        }

        case PICKUP_X2: {
            int num = key_to_number(key);
            if (num != -1) {
                xEind2 = num;
                printf("Pickup2 X = %d\n", xEind2);
                inputState = PICKUP_Y2;
            }
            break;
        }
        case PICKUP_Y2: {
            int num = key_to_number(key);
            if (num != -1) {
                yEind2 = num;
                printf("Pickup2 Y = %d\n", yEind2);
                inputState = IDLE;
            }
            break;
        }

        case DROPOFF_X2: {
            int num = key_to_number(key);
            if (num != -1) {
                xEindDropOf2 = num;
                printf("Dropoff2 X = %d\n", xEindDropOf2);
                inputState = DROPOFF_Y2;
            }
            break;
        }
        case DROPOFF_Y2: {
            int num = key_to_number(key);
            if (num != -1) {
                yEindDropOf2 = num;
                printf("Dropoff2 Y = %d\n", yEindDropOf2);
                infoEindPosOpgehaald2 = 1;
                inputState = IDLE;
            }
            break;
        }
    }
}

void pickUp_and_DropOff_pos(void) {
    printf("Waiting for coordinates...\n");
    cli();
    // ✅ block until both coordinate pairs are filled
    if((PIN_SwitchTweedeCoord & (1 << pinSwitchTweedeCoord))) {
        while (!infoEindPosOpgehaald) {
            int key = keypad_getkey();
            if (key != -1 && key != lastKey) {
                processKey(key);
                lastKey = key;     // latch this key
            }
            if (key == -1) {
                lastKey = -1;      // reset latch when released
            }

        }

    }


    // ✅ block until both coordinate pairs are filled
    if(!(PIN_SwitchTweedeCoord & (1 << pinSwitchTweedeCoord))) {
        while (!(infoEindPosOpgehaald && infoEindPosOpgehaald2)) {
            int key = keypad_getkey();
            if (key != -1 && key != lastKey) {
                processKey(key);
                lastKey = key;     // latch this key
            }
            if (key == -1) {
                lastKey = -1;      // reset latch when released
            }

        }
    }

    printf("All coordinates received!\n");
    sei();
    startSlot = 1;
}



