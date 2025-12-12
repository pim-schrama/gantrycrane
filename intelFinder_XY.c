#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "gridfinding_def.h"

int lastKey = -1, key = -2;
int num2 = 0;

enum InputState {
    IDLE,
    PICKUP_X,
    PICKUP_Y,
    DROPOFF_X,
    DROPOFF_Y,
};

enum InputState inputState = IDLE;

enum InputStateBlock {
    BLOCKIDLE,
    BLOCKINPUT,
    BLOCKINPUT2,


};

enum InputStateBlock inputStateBlock = BLOCKIDLE;

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

// Positiebepal+ing
void x_pos_finder(void) {
    if ((PIN_pos_XY & (1 << pos_X1))) {
        DEBOUNCE40;
        xNow = 1;
                                                                         printf("xNow_1\n");
    }
    if ((PIN_pos_XY & (1 << pos_X2))) {
        DEBOUNCE40;
        xNow = 2;
                                                                         printf("xNow_2\n");
    }
    if ((PIN_pos_XY & (1 << pos_X3))) {
        DEBOUNCE40;
        xNow = 3;
                                                                         printf("xNow_3\n");
    }
    if ((PIN_pos_XY & (1 << pos_X4))) {
        DEBOUNCE40;
        xNow = 4;
                                                                         printf("xNow_4\n");
    }
    if ((PIN_pos_XY & (1 << pos_X5))) {
        DEBOUNCE40;
        xNow = 5;
                                                                         printf("xNow_5\n");
    }
}

void y_pos_finder(void) {
    if ((PIN_pos_Y & (1 << pos_Y1))) {
        DEBOUNCE40;
        yNow = 1;
                                                                         printf("yNow_1\n");
    }
    if ((PIN_pos_XY & (1 << pos_Y2))) {
        DEBOUNCE40;
        yNow = 2;
                                                                         printf("yNow_2\n");
    }
    if ((PIN_pos_Y & (1 << pos_Y3))) {
        DEBOUNCE40;
        yNow = 3;
                                                                         printf("yNow_3\n");
    }
    if ((PIN_pos_Y  & (1 << pos_Y4))) {
        DEBOUNCE40;
        yNow = 4;
                                                                         printf("yNow_4\n");
    }
    if ((PIN_pos_XY  & (1 << pos_Y5))) {
        DEBOUNCE40;
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
        case SIX:   return 6;
        case SEVEN: return 7;
        case EIGHT: return 8;
        case NINE:  return 9;
        case ZERO:  return 0;
        case HASH:  return 10;
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

     if(blockCountRecieved){

            switch (inputState) {

                case IDLE:
                    if (inputEndPosRetrieved) {
                        break; // already done
                    }else{
                        inputState = PICKUP_X;
                    }
                    break;

                case PICKUP_X: {
                    int num = key_to_number(key);
                    if ((num != -1) && ((0 < num) && (num < 6))) {
                        coordCount++;
                        xEndArr[coordCount] = num;
                        DEBOUNCE50;
                                                                                             printf("Pickup X = %d\n", xEndArr[coordCount]);
                        inputState = PICKUP_Y;
                    }
                    break;
                }
                case PICKUP_Y: {
                    int num = key_to_number(key);
                    if ((num != -1) && ((0 < num) && (num < 6))) {
                        yEndArr[coordCount] = num;
                        DEBOUNCE50;
                                                                                             printf("Pickup Y = %d\n", yEndArr[coordCount]);
                        inputState = DROPOFF_X;
                    }
                    break;
                }

                case DROPOFF_X: {
                    int num = key_to_number(key);
                    if ((num != -1) && ((0 < num) && (num < 6))) {
                        xEndDropOffArr[coordCount] = num;
                        DEBOUNCE50;
                                                                                             printf("Dropoff X = %d\n", xEndDropOffArr[coordCount]);
                        inputState = DROPOFF_Y;
                    }
                    break;
                }
                case DROPOFF_Y: {
                    int num = key_to_number(key);
                    if ((num != -1) && ((0 < num) && (num < 6))) {
                        yEndDropOffArr[coordCount] = num;
                        DEBOUNCE50;
                        coordCountPickDrop++;                                                printf("Dropoff Y = %d\n", yEndDropOffArr[coordCount]);
                        inputState = IDLE;
                    }
                    break;
                }
            }
        }else{

            switch (inputStateBlock) {

                case BLOCKIDLE:
                    if(blockCountRecieved){
                        break;
                    }else{
                        inputStateBlock = BLOCKINPUT;
                    }
                    break;

                case BLOCKINPUT: {
                    num2 = 0;
                    int num = key_to_number(key);
                    if((num != -1) && (num < 10)){
                        num2 = num;
                        DEBOUNCE50;
                                                                                  printf("BlockInput1 = %d\n",num2);
                        inputStateBlock = BLOCKINPUT2;
                    }
                    break;
                }

                case BLOCKINPUT2: {
                    int num = key_to_number(key);
                    if((num != -1) && (num < 10)){
                        num2 = num2 * 10;
                        blocks = num + num2;
                        DEBOUNCE50;
                                                                                  printf("BlockInput2 = %d\n",blocks);
                        blockCountRecieved = 1;
                        inputStateBlock = BLOCKIDLE;
                    }else if(num == 10){
                        blocks = num2;
                        blockCountRecieved = 1;
                        inputStateBlock = BLOCKIDLE;
                    }
                    break;
                }

                default:
                    inputStateBlock = BLOCKIDLE;
                    break;

            }
       }
}

void block_count(void) {
    key = -2;                                                                        printf("Waiting for blocks...\n");
    while(key != 14){
        key = keypad_getkey();
        if (key != -1 && key != lastKey) {
            processKey(key);
            lastKey = key;     // latch this key
        }
        if (key == -1) {
            lastKey = -1;      // reset latch when released
        }
    }
    if (blocks > MAX_BLOCKS) blocks = MAX_BLOCKS;
                                                                                     printf("Blocks: %d\n", blocks);
    blockCountRecieved = 1;
}

void pickup_dropoff_pos(void) {
                                                                                     printf("Waiting for coordinates...\n");
    // ✅ block until both coordinate pairs are filled
    while (!inputEndPosRetrieved) {
        key = keypad_getkey();
        if (key != -1 && key != lastKey) {
            processKey(key);
            lastKey = key;     // latch this key
        }
        if (key == -1) {
            lastKey = -1;      // reset latch when released
        }

        if(coordCountPickDrop == blocks){
            inputEndPosRetrieved = 1;
            coord_switch();                                                                                        printf("All coordinates received!\n");
            startBlock = 1;
        }
    }
}



