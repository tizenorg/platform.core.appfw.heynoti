CC ?= gcc

TARGETS = utc_ApplicationFW_heynoti_init_func \
	utc_ApplicationFW_heynoti_close_func \
	utc_ApplicationFW_heynoti_subscribe_func \
	utc_ApplicationFW_heynoti_unsubscribe_func \
	utc_ApplicationFW_heynoti_subscribe_file_func \
	utc_ApplicationFW_heynoti_unsubscribe_file_func \
	utc_ApplicationFW_heynoti_publish_func \
	utc_ApplicationFW_heynoti_poll_event_func \
	utc_ApplicationFW_heynoti_attach_handler_func \
	utc_ApplicationFW_heynoti_detach_handler_func \
	utc_ApplicationFW_heynoti_get_snoti_name_func \
	utc_ApplicationFW_heynoti_get_pnoti_name_func

PKGS = glib-2.0 dlog heynoti

LDFLAGS = `pkg-config --libs $(PKGS)`
LDFLAGS += $(TET_ROOT)/lib/tet3/tcm_s.o
LDFLAGS += -L$(TET_ROOT)/lib/tet3 -ltcm_s
LDFLAGS += -L$(TET_ROOT)/lib/tet3 -lapi_s
LDFLAGS += -L/usr/lib -lpthread

CFLAGS = -I. `pkg-config --cflags $(PKGS)`
CFLAGS += -I$(TET_ROOT)/inc/tet3
CFLAGS += -Wall

all: $(TARGETS)

$(TARGETS): %: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGETS)
