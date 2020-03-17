CFLAGS_GLIB = $(shell pkg-config --cflags --libs glib-2.0)
CFLAGS_DBUS = $(shell pkg-config --cflags --libs dbus-1)
CFLAGS_DBUS_GLIB = $(shell pkg-config --cflags --libs dbus-glib-1)
CFLAGS_GIO  = $(shell pkg-config --cflags --libs gio-2.0)
CFLAGS_SDBUS = $(shell pkg-config --cflags --libs libsystemd)

CFLAGS = -g
CFLAGS += $(CFLAGS_DBUS) $(CFLAGS_DBUS_GLIB) $(CFLAGS_GLIB) $(CFLAGS_GIO) $(CFLAGS_SDBUS)
CC = gcc
OBJ = gatt-service.o gdbus/object.o gdbus/client.o gdbus/mainloop.o gdbus/watch.o gdbus/polkit.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

gatt-service: $(OBJ)
	$(CC) -o $@  $^ $(CFLAGS)

clean:
	rm -f gatt-service
	rm -f *.o
	rm -f gdbus/*.o

.PHONY: all clean
