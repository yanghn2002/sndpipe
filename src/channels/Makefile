_CFLAGS := $(CFLAGS)
_LDFLAGS := $(LDFLAGS)

TARGETS := split combine

build: channels.c
	$(CC) channels.c $(_CFLAGS) $(_LDFLAGS) -o $(INS_DIR)/bin/sp-split\
		-DCHANNELS_TASK=1
	$(CC) channels.c $(_CFLAGS) $(_LDFLAGS) -o $(INS_DIR)/bin/sp-combine\
		-DCHANNELS_TASK=2