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

 enum KeyPressed {
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

int adc_average(uint8_t ch, int samples) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += adc_read(ch);
    _delay_ms(2); // kleine pauze tussen metingen om ruis te verminderen
  }
  return sum / samples;
}

// Positiebepaling
void x_pos_finder(void) {
    if ((PIN_pos_XY & (1 << pos_X1))) {
        _delay_ms(60);
        xNow = 1;
                                                                         printf("xNow_1\n");
    }
    if ((PIN_pos_XY & (1 << pos_X2))) {
        _delay_ms(60);
        xNow = 2;
                                                                         printf("xNow_2\n");
    }
    if ((PIN_pos_XY & (1 << pos_X3))) {
        _delay_ms(60);
        xNow = 3;
                                                                         printf("xNow_3\n");
    }
    if ((PIN_pos_XY & (1 << pos_X4))) {
        _delay_ms(60);
        xNow = 4;
                                                                         printf("xNow_4\n");
    }
    if ((PIN_pos_XY & (1 << pos_X5))) {
        _delay_ms(60);
        xNow = 5;
                                                                         printf("xNow_5\n");
    }
}

void y_pos_finder(void) {
    if ((PIN_pos_XY & (1 << pos_Y1))) {
        _delay_ms(60);
        yNow = 1;
                                                                         printf("yNow_1\n");
    }
    if ((PIN_pos_Y & (1 << pos_Y2))) {
        _delay_ms(60);
        yNow = 2;
                                                                         printf("yNow_2\n");
    }
    if ((PIN_pos_Y & (1 << pos_Y3))) {
        _delay_ms(60);
        yNow = 3;
                                                                         printf("yNow_3\n");
    }
    if ((PIN_pos_XY  & (1 << pos_Y4))) {
        _delay_ms(60);
        yNow = 4;
                                                                         printf("yNow_4\n");
    }
    if ((PIN_pos_Y  & (1 << pos_Y5))) {
        _delay_ms(60);
        yNow = 5;
                                                                         printf("yNow_5\n");
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
            if ((inputEndPosRetrieved == 1) && (inputEndPosRetrieved2 == 1)) {
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
                xEnd = num;
                                                                                     printf("Pickup X = %d\n", xEnd);
                inputState = PICKUP_Y;
            }
            break;
        }
        case PICKUP_Y: {
            int num = key_to_number(key);
            if (num != -1) {
                yEnd = num;
                                                                                     printf("Pickup Y = %d\n", yEnd);
                inputState = IDLE;
            }
            break;
        }

        case DROPOFF_X: {
            int num = key_to_number(key);
            if (num != -1) {
                xEndDropOff = num;
                                                                                     printf("Dropoff X = %d\n", xEndDropOff);
                inputState = DROPOFF_Y;
            }
            break;
        }
        case DROPOFF_Y: {
            int num = key_to_number(key);
            if (num != -1) {
                yEndDropOff = num;
                                                                                     printf("Dropoff Y = %d\n", yEndDropOff);
                inputEndPosRetrieved = 1;
                inputState = IDLE;
            }
            break;
        }

        case PICKUP_X2: {
            int num = key_to_number(key);
            if (num != -1) {
                xEnd2 = num;
                                                                                     printf("Pickup2 X = %d\n", xEnd2);
                inputState = PICKUP_Y2;
            }
            break;
        }
        case PICKUP_Y2: {
            int num = key_to_number(key);
            if (num != -1) {
                yEnd2 = num;
                                                                                     printf("Pickup2 Y = %d\n", yEnd2);
                inputState = IDLE;
            }
            break;
        }

        case DROPOFF_X2: {
            int num = key_to_number(key);
            if (num != -1) {
                xEndDropOff2 = num;
                                                                                     printf("Dropoff2 X = %d\n", xEndDropOff2);
                inputState = DROPOFF_Y2;
            }
            break;
        }
        case DROPOFF_Y2: {
            int num = key_to_number(key);
            if (num != -1) {
                yEndDropOff2 = num;
                                                                                     printf("Dropoff2 Y = %d\n", yEndDropOff2);
                inputEndPosRetrieved2 = 1;
                inputState = IDLE;
            }
            break;
        }
    }
}

void pickup_dropoff_pos(void) {
                                                                                     printf("Waiting for coordinates...\n");
    // ✅ block until both coordinate pairs are filled
    if(!(PIN_SwitchSecondCoord & (1 << pin_SwitchSecondCoord))) {
        while (!inputEndPosRetrieved) {
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
    if((PIN_SwitchSecondCoord & (1 << pin_SwitchSecondCoord))) {
        while (!(inputEndPosRetrieved && inputEndPosRetrieved2)) {
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

    startBlock = 1;
}



