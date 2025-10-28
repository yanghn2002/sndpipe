_CFLAGS := $(CFLAGS)
_LDFLAGS := $(LDFLAGS)

SIGNALS_TARGETS := sine square saw


build: $(SIGNALS_TARGETS) constand

$(SIGNALS_TARGETS): signals.c
	$(CC) signals.c $(_CFLAGS) $(_LDFLAGS) -o $(INS_DIR)/bin/sp-$@\
		-DWAVE_LAMBDA=wave_lambda_$@
		 
constand: %: constand.c
	$(CC) constand.c $(_CFLAGS) $(_LDFLAGS) -o $(INS_DIR)/bin/sp-$@