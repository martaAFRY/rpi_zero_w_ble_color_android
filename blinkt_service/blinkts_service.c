/*
gcc blinkts_service.c blinkts.c -o blinkts_service `pkg-config --cflags --libs libsystemd` -lbcm2835

busctl --user call com.example.Blinkts /com/example/Blinkts com.example.Blinkts SetRed "y" 255


*/
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <systemd/sd-bus.h>

#include "blinkts.h"

// TODO change to some state thingy maybe
uint8_t curr_red = 0;
uint8_t curr_green = 0;
uint8_t curr_blue = 0;

uint8_t is_inited = 0;

int keep_running = 1;

void set_split(uint32_t col) {

  if (!is_inited) {
    blinkts_init();
    is_inited = 1;
  }
  blinkts_split_color(col);
}

void update_colors()
{
  if (!is_inited) {
    blinkts_init();
    is_inited = 1;
  }
    blinkts_color(curr_red, curr_green, curr_blue);
}

static int method_setred(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  uint8_t col;
  int ret;

  ret = sd_bus_message_read(m, "y", &col);
  if (ret < 0) {
    syslog(LOG_ERR, "Failed to set red: %s\n", strerror(-ret));
    return ret;
  }
  curr_red = col;
  update_colors();
  return sd_bus_reply_method_return(m, NULL);
}

static int method_setgree(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  uint8_t col;
  int ret;

  ret = sd_bus_message_read(m, "y", &col);
  if (ret < 0) {
    syslog(LOG_ERR, "Failed to set red: %s\n", strerror(-ret));
    return ret;
  }
  curr_green = col;
  update_colors();
  return sd_bus_reply_method_return(m, NULL);
}

static int method_setblue(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  uint8_t col;
  int ret;

  ret = sd_bus_message_read(m, "y", &col);
  if (ret < 0) {
    syslog(LOG_ERR, "Failed to set red: %s\n", strerror(-ret));
    return ret;
  }
  curr_blue = col;
  update_colors();
  return sd_bus_reply_method_return(m, NULL);
}

static int method_clear(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  blinkts_clear();
  blinkts_exit();
  is_inited = 0;
  return sd_bus_reply_method_return(m, NULL);
}

static int method_split(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
  uint32_t col;
  int ret;

  ret = sd_bus_message_read(m, "u", &col);

 if (ret < 0) {
    syslog(LOG_ERR, "Failed to set split: %s\n", strerror(-ret));
    return ret;
  }
 set_split(col);
  return sd_bus_reply_method_return(m, NULL);

}

static const sd_bus_vtable blinkt_vtable[] = {
        SD_BUS_VTABLE_START(0),
        SD_BUS_METHOD("SetRed", "y", NULL, method_setred, SD_BUS_VTABLE_UNPRIVILEGED),
        SD_BUS_METHOD("SetGreen", "y", NULL, method_setgree,   SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("SetBlue", "y", NULL, method_setblue,   SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("Clear", NULL, NULL, method_clear,   SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("Split", "u", NULL, method_split,   SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_VTABLE_END
};

void signal_handler(int i) {
  keep_running = 0;
}

int main(int argc, char *argv[]) {

  sd_bus_slot *slot = NULL;
  sd_bus *bus = NULL;
  int ret = 0;
  struct sigaction action;
  keep_running = 1;
  action.sa_handler = signal_handler;
  sigaction(SIGINT, &action, NULL);

  blinkts_init();
  is_inited = 1;
  /* Connect to the user bus this time */
  // TODO change to system
  ret = sd_bus_open_user(&bus);
  if (ret < 0) {
    syslog(LOG_ERR, "Failed to connect to system bus: %s\n", strerror(-ret));
    goto exit;
  }

  ret = sd_bus_add_object_vtable(bus,
				 &slot,
				 "/com/example/Blinkts",  /* object path */
				 "com.example.Blinkts",   /* interface name */
				 blinkt_vtable,
				 NULL);
  if (ret < 0) {
    syslog(LOG_ERR, "Failed to add vtable: %s\n", strerror(-ret));
    goto exit;
  }
  ret = sd_bus_request_name(bus, "com.example.Blinkts", 0);
  if (ret < 0) {
    syslog(LOG_ERR, "Failed to get name: %s\n", strerror(-ret));
    goto exit;
  }
  //TODO create a thread instead
  for (;;) {
    if (!keep_running)
      goto exit;
    /* Process requests */
    ret = sd_bus_process(bus, NULL);
    if (ret < 0) {
      syslog(LOG_ERR, "Failed to process bus: %s\n", strerror(-ret));
      goto exit;
    }
    if (ret > 0) /* we processed a request, try to process another one, right-away */
      continue;
    /*Wait for the next request to process */
    ret = sd_bus_wait(bus, (uint64_t) -1);
    if (ret < 0) {
      syslog(LOG_ERR, "Failed to wait fro next: %s\n", strerror(-ret));
      goto exit;
    }
  }

 exit:
  blinkts_clear();
  blinkts_exit();
  sd_bus_slot_unref(slot);
  sd_bus_unref(bus);

  return ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
