#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <pigpio.h>

#define LCD_CLK  (10000)
#define DATA_SEND_GAP (1000000/LCD_CLK - 1)
#define LCD_CSB_HIGH_HLD (1)
#define LCD_CSB_LOW_HLD  (1)

#define LCD_SCL  (4)  // RPI GPIO number
#define LCD_SDA  (25)
#define LCD_CSB  (24)
#define LCD_INHB (23)

#define LCD_CMD_ICSET   0b11101010
#define LCD_CMD_ICSET2  0b11101000
#define LCD_CMD_BLKCTL  0b11110001
#define LCD_CMD_BLKCTL2 0b11110000
#define LCD_CMD_DISCTL  0b10100110
#define LCD_CMD_DISCTL2 0b10100111
#define LCD_CMD_ADSET   0b00000000
#define LCD_CMD_APCTL   0b11100111
#define LCD_CMD_APCTL2  0b11111100
#define LCD_CMD_MODSET  0b11001000
#define LCD_CMD_MODSET2 0b11000000

#define DATA_MAX (18)
unsigned char ALL_SET_DISP_DATA_ARRAY[DATA_MAX] = {
  0b11111111,  // LCD_DSP_DATA00
  0b11111111,  // LCD_DSP_DATA01
  0b11111111,  // LCD_DSP_DATA02
  0b11111111,  // LCD_DSP_DATA03
  0b11111111,  // LCD_DSP_DATA04
  0b11111111,  // LCD_DSP_DATA05
  0b11111111,  // LCD_DSP_DATA06
  0b11111111,  // LCD_DSP_DATA07
  0b11111111,  // LCD_DSP_DATA08
  0b11111111,  // LCD_DSP_DATA09
  0b11111111,  // LCD_DSP_DATA10
  0b11111111,  // LCD_DSP_DATA11
  0b11111111,  // LCD_DSP_DATA12
  0b11111111,  // LCD_DSP_DATA13
  0b11111111,  // LCD_DSP_DATA14
  0b11111111,  // LCD_DSP_DATA15
  0b11111111,  // LCD_DSP_DATA16
  0b11110000   // LCD_DSP_DATA17
};

unsigned char ALL_CLR_DISP_DATA_ARRAY[DATA_MAX] = {
  0b00000000,  // LCD_DSP_DATA00
  0b00000000,  // LCD_DSP_DATA01
  0b00000000,  // LCD_DSP_DATA02
  0b00000000,  // LCD_DSP_DATA03
  0b00000000,  // LCD_DSP_DATA04
  0b00000000,  // LCD_DSP_DATA05
  0b00000000,  // LCD_DSP_DATA06
  0b00000000,  // LCD_DSP_DATA07
  0b00000000,  // LCD_DSP_DATA08
  0b00000000,  // LCD_DSP_DATA09
  0b00000000,  // LCD_DSP_DATA10
  0b00000000,  // LCD_DSP_DATA11
  0b00000000,  // LCD_DSP_DATA12
  0b00000000,  // LCD_DSP_DATA13
  0b00000000,  // LCD_DSP_DATA14
  0b00000000,  // LCD_DSP_DATA15
  0b00000000,  // LCD_DSP_DATA16
  0b00000000   // LCD_DSP_DATA17
};

unsigned char FUNC_USB_DISP_DATA[DATA_MAX] = {
  0b01010000,  //A LCD_DSP_DATA00
  0b10001010,  //B LCD_DSP_DATA01
  0b01100000,  //C LCD_DSP_DATA02
  0b10011100,  //D LCD_DSP_DATA03
  0b11100000,  //E LCD_DSP_DATA04
  0b10001100,  //F LCD_DSP_DATA05
  0b00000000,  //G LCD_DSP_DATA06
  0b00000000,  //H LCD_DSP_DATA07
  0b00000000,  //I LCD_DSP_DATA08
  0b00000000,  //J LCD_DSP_DATA09
  0b00000000,  //L LCD_DSP_DATA10
  0b00000000,  //M LCD_DSP_DATA11
  0b00000000,  //N LCD_DSP_DATA12
  0b00000000,  //O LCD_DSP_DATA13
  0b00000000,  //P LCD_DSP_DATA14
  0b00000000,  //Q LCD_DSP_DATA15
  0b00000000,  //R LCD_DSP_DATA16
  0b00000000   //S LCD_DSP_DATA17
};

unsigned char FUNC_BLT_DISP_DATA[DATA_MAX] = {
  0b01110000,  //A LCD_DSP_DATA00
  0b10001100,  //B LCD_DSP_DATA01
  0b10100000,  //C LCD_DSP_DATA02
  0b10000000,  //D LCD_DSP_DATA03
  0b00000100,  //E LCD_DSP_DATA04
  0b00010000,  //F LCD_DSP_DATA05
  0b00000000,  //G LCD_DSP_DATA06
  0b00000000,  //H LCD_DSP_DATA07
  0b00000000,  //I LCD_DSP_DATA08
  0b00000000,  //J LCD_DSP_DATA09
  0b00000000,  //L LCD_DSP_DATA10
  0b00000000,  //M LCD_DSP_DATA11
  0b00000000,  //N LCD_DSP_DATA12
  0b00000000,  //O LCD_DSP_DATA13
  0b00000000,  //P LCD_DSP_DATA14
  0b00000000,  //Q LCD_DSP_DATA15
  0b00000000,  //R LCD_DSP_DATA16
  0b00000000   //S LCD_DSP_DATA17
};

unsigned char FUNC_NTK_DISP_DATA[DATA_MAX] = {
  0b01010010,  //A LCD_DSP_DATA00
  0b01001010,  //B LCD_DSP_DATA01
  0b00000100,  //C LCD_DSP_DATA02
  0b00010000,  //D LCD_DSP_DATA03
  0b00000100,  //E LCD_DSP_DATA04
  0b01100000,  //F LCD_DSP_DATA05
  0b00000000,  //G LCD_DSP_DATA06
  0b00000000,  //H LCD_DSP_DATA07
  0b00000000,  //I LCD_DSP_DATA08
  0b00000000,  //J LCD_DSP_DATA09
  0b00000000,  //L LCD_DSP_DATA10
  0b00000000,  //M LCD_DSP_DATA11
  0b00000000,  //N LCD_DSP_DATA12
  0b00000000,  //O LCD_DSP_DATA13
  0b00000000,  //P LCD_DSP_DATA14
  0b00000000,  //Q LCD_DSP_DATA15
  0b00000000,  //R LCD_DSP_DATA16
  0b00000000   //S LCD_DSP_DATA17
};

void sendData(unsigned char, int);
void funcSwitch(unsigned char data[]);
void signal_handler(int);

int main(int argc, char *argv[])
{
    int i = 0;
    int j = 0;
    int half = 0;
    if (gpioInitialise()<0) return -1;

    signal(SIGHUP,  signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGKILL, signal_handler);
    signal(SIGINT,  signal_handler);

    /* Pin mode set */
    gpioSetMode(LCD_CSB, PI_OUTPUT);
    gpioSetMode(LCD_SDA, PI_OUTPUT);
    gpioSetMode(LCD_SCL, PI_OUTPUT);
    gpioHardwareClock(LCD_SCL, LCD_CLK);
    gpioDelay(100000);

    /* Power ON sequence */
    gpioSetMode(LCD_INHB, PI_OUTPUT);
    gpioWrite(LCD_INHB, 0);
    gpioDelay(150000);
    gpioWrite(LCD_INHB, 1);
    gpioDelay(150);
    gpioWrite(LCD_CSB,  1);
    gpioDelay(LCD_CSB_HIGH_HLD);

    /* Initialize sequence */
    gpioWrite(LCD_CSB,  0);
    gpioDelay(LCD_CSB_LOW_HLD);
    sendData(LCD_CMD_ICSET, 0);
    sendData(LCD_CMD_MODSET2, 0);
    sendData(LCD_CMD_ADSET, 0);
    for(i = 0; i < DATA_MAX; i++) {
        half = (i == DATA_MAX - 1) ? 1 : 0;
        sendData(0b00000000, half);
    }
    gpioWrite(LCD_CSB,  1);
    gpioDelay(LCD_CSB_HIGH_HLD);

    /* DISPON sequence */
    gpioWrite(LCD_CSB,  0);
    gpioDelay(LCD_CSB_LOW_HLD);
    sendData(LCD_CMD_DISCTL2, 0);
    sendData(LCD_CMD_BLKCTL2, 0);
    sendData(LCD_CMD_APCTL2, 0);
    sendData(LCD_CMD_MODSET, 0);
    gpioWrite(LCD_CSB,  1);
    gpioDelay(LCD_CSB_HIGH_HLD);

    /* RAM write sequence */
    while(1) {
        if(j == 0){
            funcSwitch(FUNC_USB_DISP_DATA);
            j++;
        }else if(j == 1){
            funcSwitch(FUNC_BLT_DISP_DATA);
            j++;
        }else {
            funcSwitch(FUNC_NTK_DISP_DATA);
            j = 0;
        }
        gpioDelay(3000000);
    }
    /* DISPOFF sequence */
    // gpioWrite(LCD_CSB,  0);
    // gpioDelay(LCD_CSB_LOW_HLD);
    // sendData(LCD_CMD_MODSET2, 0);
    // gpioDelay(1000); // Wait for several ms

    /* Disable data transfer */
    // gpioWrite(LCD_CSB,  1);
    // gpioDelay(LCD_CSB_HIGH_HLD);

    gpioTerminate();
}

void sendData(unsigned char data, int half)
{
    unsigned char bits[8] = {0};
    int MAX = (half == 1) ? 3 : 7;
    int i = 0;

    bits[7] = (data & 0b00000001) >> 0;
    bits[6] = (data & 0b00000010) >> 1;
    bits[5] = (data & 0b00000100) >> 2;
    bits[4] = (data & 0b00001000) >> 3;
    bits[3] = (data & 0b00010000) >> 4;
    bits[2] = (data & 0b00100000) >> 5;
    bits[1] = (data & 0b01000000) >> 6;
    bits[0] = (data & 0b10000000) >> 7;

    // for (i=MAX; i >= 0; i--) {
    for (i=0; i <= MAX; i++) {
        gpioWrite(LCD_SDA, bits[i]);
        gpioDelay(DATA_SEND_GAP);
    }
}

void funcSwitch(unsigned char data[])
{
    int i = 0;
    int half = 0;
    gpioWrite(LCD_CSB,  0);
    gpioDelay(LCD_CSB_LOW_HLD);
    sendData(LCD_CMD_DISCTL2, 0);
    sendData(LCD_CMD_BLKCTL2, 0);
    sendData(LCD_CMD_APCTL2, 0);
    sendData(LCD_CMD_MODSET, 0);
    sendData(LCD_CMD_ADSET, 0);
    for(i = 0; i < DATA_MAX; i++) {
        half = (i == DATA_MAX - 1) ? 1 : 0;
        sendData(data[i], half);
    }
    gpioWrite(LCD_CSB,  1);
    gpioDelay(LCD_CSB_HIGH_HLD);
}

void signal_handler(int sig)
{
    switch(sig) {
    case SIGHUP:
    case SIGTERM:
    case SIGKILL:
    case SIGINT:
    case SIGABRT:
      // DISPOFF sequence
      gpioWrite(LCD_CSB,  0);
      gpioDelay(LCD_CSB_LOW_HLD);
      sendData(LCD_CMD_MODSET2, 0);
      gpioWrite(LCD_CSB,  1);
      gpioDelay(LCD_CSB_HIGH_HLD);

      gpioWrite(LCD_INHB, 0);
      gpioDelay(150000);

      gpioSetMode(LCD_INHB, PI_INPUT);
      gpioSetMode(LCD_SCL,  PI_INPUT);
      gpioSetMode(LCD_CSB,  PI_INPUT);
      gpioSetMode(LCD_SDA,  PI_INPUT);

      gpioTerminate();
      sleep(1);
      exit(0);
      break;
    }
}
