CFLAGS += -std=c99 -I /usr/include/zdb -I .
LDFLAGS += $(LIBSPREFIX)zdb

all: $(BUILDDIR)sqlauth$(DLSUFFICS)

sqlauth$(OBJSUFFICS): sqlauth.c
	$(CC) $(DCFLAGS) $(CFLAGS) sqlauth.c

pluginapi$(OBJSUFFICS): pluginapi.c
	$(CC) $(DCFLAGS) $(CFLAGS) pluginapi.c

$(BUILDDIR)sqlauth$(DLSUFFICS): pluginapi$(OBJSUFFICS) sqlauth$(OBJSUFFICS)
	$(LN) $(LNOUT)../../$(BUILDDIR)sqlauth$(DLSUFFICS) $(LDFLAGS) $(DLFLAGS) pluginapi$(OBJSUFFICS) sqlauth$(OBJSUFFICS)

clean:
	rm -rf *.o sqlauth$(DLSUFFICS)
