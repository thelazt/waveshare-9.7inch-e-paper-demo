# Path to the command-line utility program
MCUDIR = ../mcufont/encoder
MCUFONT = mcufont

QUALITY = 10

FONTS =
FONT_FILE = ../fonts.h
FONT_DIR = fonts/

# Characters to include in the fonts
CHARS = 0-255 0x2010-0x2015

define ADD_FONT
	$(1)$(2).dat: $(1).ttf
endef

all: $(FONT_FILE)

$(eval $(call ADD_FONT,DejaVuSans,12))

clean:
	rm -f $(FONTS:=.c) $(FONTS:=.dat)

$(FONT_FILE): $(FONTS:=.c) $(FONTS:=.dat)
	/bin/echo -e $(foreach font,$(FONTS),'\n#include "'$(FONT_DIR)/$(font)'.c"') > $@

$(MCUDIR)/$(MCUFONT):
	make -C $(MCUDIR) MCUFONT

%.c: %.dat $(MCUDIR)/$(MCUFONT)
	$(MCUDIR)/$(MCUFONT) rlefont_export $<

%.dat: %.bdf $(MCUDIR)/$(MCUFONT)
	$(MCUDIR)/$(MCUFONT) import_bdf $<
	$(MCUDIR)/$(MCUFONT) filter $@ $(CHARS)
	$(MCUDIR)/$(MCUFONT) rlefont_optimize $@ 50


