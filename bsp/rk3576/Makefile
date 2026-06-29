# Makefile for KICKPI-K7 board support

# Include paths
INCDIR = $(INCDIR_PREFIX)$(CONFIG_ARCH_CHIP)

# Sources
CSRCS = kickpi-k7_bringup.c

# Dependencies
DEPDIR = .dep

all: $(DEPDIR) libboard.a

$(DEPDIR):
	@mkdir -p $@

libboard.a: $(CSRCS:.c=.o)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.a
	rm -rf $(DEPDIR)

distclean: clean
	rm -f Makefile

-include $(DEPDIR)/*.d
