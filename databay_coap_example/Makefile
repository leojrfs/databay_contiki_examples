# check for databay_user_conf.h file
ifneq ($(shell test -f databay_user_conf.h; echo $$?),0)
$(error "'databay_user_conf.h' does not exist, please create it by copying 'databay_user_conf_template.h'")
endif

CONTIKI_PROJECT = databay_coap_example
CONTIKI_SOURCEFILES += \
	databay_coap_client.c \
	cmp.c

all: $(CONTIKI_PROJECT)

CONTIKI = ../contiki-ng
include $(CONTIKI)/Makefile.dir-variables
ifeq ($(ENABLE_SHELL),1)
MODULES += os/services/shell
endif
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/coap
include $(CONTIKI)/Makefile.include