export SRC_DIR := $(shell pwd)/src
export INC_DIR := $(shell pwd)/include
export INS_DIR := $(shell pwd)/build

export CFLAGS := $(CFLAGS) -O3 -I$(INC_DIR)
_LDFLAGS := $(LDFLAGS) -L$(INS_DIR)/lib -lm
export LDFLAGS := $(_LDFLAGS) -lsndpipe


SRCS := $< $(addprefix $(SRC_DIR)/,\
	buffer.c lambda.c pcmio.c utils.c\
	source/source.c mixer/gain.c)


build:
	@echo CFLAGS=$(CFLAGS)
	@echo LDFLAGS=$(LDFLAGS)
	@mkdir -p $(INS_DIR)/bin
	@mkdir -p $(INS_DIR)/lib
	$(CC) -fPIC --shared $(SRCS)\
		$(CFLAGS) $(_LDFLAGS) -o $(INS_DIR)/lib/libsndpipe.so
	@make -C $(SRC_DIR)/source
	@make -C $(SRC_DIR)/mixer
	@make -C $(SRC_DIR)/channels
ifdef STRIP
	$(STRIP) -s $(INS_DIR)/lib/libsndpipe.so
	for bin in $(wildcard $(INS_DIR)/bin)/*; do\
		if [ -f "$$bin" ]; then $(STRIP) -s $$bin; fi;\
	done
endif

install: $(INS_DIR)
	install -m 755 $(INS_DIR)/bin/* /usr/bin
	install -m 755 $(INS_DIR)/lib/* /usr/lib

clean:
	@rm -rfv $(INS_DIR)
