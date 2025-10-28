_CFLAGS := $(CFLAGS)
_LDFLAGS := $(LDFLAGS)


build: mixer

mixer: mixer.c
	$(CC) mixer.c $(_CFLAGS) $(_LDFLAGS) -o $(INS_DIR)/bin/sp-$@