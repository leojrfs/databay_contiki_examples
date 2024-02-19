all: serve

ifeq ($(VERBOSE),1)
VERBOSE_PARAM = -v
else
VERBOSE_PARAM =
endif

.PHONY: serve
serve:
	@if ! [ -d .venv ]; then \
		python3 -m venv .venv; \
		. .venv/bin/activate; \
		pip3 install -r requirements.txt; \
	fi; \
	. .venv/bin/activate; \
	./databay_udp_proxy.py $(VERBOSE_PARAM)

.PHONY: clean
clean:
	rm -rf .venv

.PHONY: distclean
distclean: clean