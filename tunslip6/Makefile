LIB_SRCS = tools-utils.c
DEPEND = tools-utils.h

ifeq ($(PREFIX),)
PREFIX := /usr/local
endif

all: tunslip6

CFLAGS += -Wall -Werror -O2 -s

tunslip6: % : %.c $(LIB_SRCS) $(DEPEND)
	$(CC) $(CFLAGS) $< $(LIB_SRCS) -o $@

install:
	@install -s tunslip6 $(PREFIX)/bin/tunslip6
	@if command -v systemctl > /dev/null 2>&1; then \
		echo "systemd detected! installing 'tunslip6' service..."; \
		install utils/service/systemd/tunslip6.conf \
			/etc/tunslip6.conf; \
		install utils/service/systemd/tunslip6.service \
			/etc/systemd/system/tunslip6.service ; \
	fi

clean:
	rm -f tunslip6
