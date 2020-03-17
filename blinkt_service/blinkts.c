#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>

#define MOSI 23
#define SCLK 24
#define APA_SOF 0b11100000
#define DEFAULT_BRIGHTNESS 7
#define NUM_LEDS 8

uint32_t leds[NUM_LEDS] = {};


inline static void write_byte(uint8_t byte){
  int n;
  for(n = 0; n < NUM_LEDS; n++){
    bcm2835_gpio_write(MOSI, (byte & (1 << (7-n))) > 0);
    bcm2835_gpio_write(SCLK, HIGH);
    bcm2835_gpio_write(SCLK, LOW);
  }
}

void show(void){
  write_byte(0);
  write_byte(0);
  write_byte(0);
  write_byte(0);
  for(int x = 0; x < NUM_LEDS; x++){
    write_byte(APA_SOF | (leds[x] & 0b11111));
    write_byte((leds[x] >> 8 ) & 0xFF);
    write_byte((leds[x] >> 16) & 0xFF);
    write_byte((leds[x] >> 24) & 0xFF);
  }
  write_byte(0xff);
}
//#define TEST_STUFF 1
void run_test();

int blinkts_init() {
  int ret = -1;

  ret = bcm2835_init();
  if (ret <= 0) {
    printf("failed to init\n");
    return -1;
  }
  bcm2835_gpio_fsel(MOSI, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(MOSI, LOW);
  bcm2835_gpio_fsel(SCLK, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(SCLK, LOW);

  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = 0;
#ifdef TEST_STUFF
  run_test();
#endif
  return 0;
}

void blinkts_exit(){
  bcm2835_spi_end();
  bcm2835_close();
}

void send_clocks(int count)
{
  int i;

  bcm2835_gpio_write(MOSI, 0);

  for (i = 0; i < count; i++) {
    bcm2835_gpio_write(SCLK, HIGH);
    bcm2835_gpio_write(SCLK, LOW);
  }
}

void send_byte(uint8_t x)
{
  int i;
  int data_state = 0;
  for (i = 0; i < 8; i++) {
    int new_state = (x & 0b10000000) != 0;
    if (data_state != new_state) {
      // only send to data pin if value has changed
      bcm2835_gpio_write(MOSI, new_state);
      data_state = new_state;
    }
    bcm2835_gpio_write(SCLK, 1);
    bcm2835_gpio_write(SCLK, 0);
    x = x << 1;
  }
}

void blinkts_clear() {
  send_clocks(32);
  for (int i = 0; i < NUM_LEDS; i++) {
    send_byte(224);
    send_byte(0);
    send_byte(0);
    send_byte(0);
  }
  send_clocks(36);
}

void blinkts_color(uint8_t r, uint8_t g, uint8_t b) {
  uint32_t col = 0;

  col = (DEFAULT_BRIGHTNESS & 0b11111);
  col |= ((uint32_t)r << 24);
  col |= ((uint32_t)g << 16);
  col |= ((uint16_t)b << 8);
#ifdef TEST_STUFF
  printf("col 0x%x\n", col);
#endif
  for(int i = 0; i< NUM_LEDS; i++)
    leds[i] = col;
  // dont know why but I need to show twice
  show();
  usleep(1000);
  show();
}

void blinkts_split_color(uint32_t col) {

  leds[0] = (col & 0xFF0000FF);
  leds[1] = col & 0xFF0000FF;
  leds[2] = col;
  leds[3] = (col & 0x00FF00FF);
  leds[4] = col & 0x00FF00FF;
  leds[5] = col;
  leds[6] = (col & 0x0000FFFF);
  leds[7] = col & 0x0000FFFF;
  show();
  usleep(1000);
  show();
}

void run_test() {
  printf("run test\n");
  for(int i = 0; i< 10; i++) {
    blinkts_color(25*i, 0, 0);
    usleep(1000);
  }
}
