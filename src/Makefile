SRC:=$(shell find . -name "*.c" | sed 's/^..//') 
FLD:=$(shell find . -type d | grep -v CVS | sed 's/^..//' | sed 's/\.//g')
TRUE=TRUE

EFLAGS=${CFLAGS}
TTARGET=${TARGET}

OBJD=${BASE}/${OBJDIR}

OBJS := $(patsubst %,${OBJD}/%,${SRC:%.c=%.o})
DEPS := $(patsubst %,${OBJD}/%,${SRC:%.c=%.d})

all: build

${OBJD}/%.d: %.c
	@ echo Generating $@
	@ mkdir -p $(dir $@)
	@ ($(CC) -MM $(CPPFLAGS) $(INCFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$ )

${OBJD}/%.o : %.c
	@ echo "Building $@"
	@ $(CC) $(CFLAGS) $(INCFLAGS) -o ${OBJD}/$*.o -c ${BASE}/${SUBDIR}/$<

-include $(SRC:%.c=$(OBJD)/%.d)

build: $(DEPS) $(OBJS) 
	@ echo "Building" $(TARGET)
	@ $(CC) $(CFLAGS) $(INCFLAGS) $(OBJS) $(LDFLAGS) -o ${BASE}/${BINDIR}/$(TARGET)
	@ echo "Completed: $(TARGET)"
