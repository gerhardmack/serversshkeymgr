CC:=gcc
TARGET:=keymgr
SUBDIRS:=src
OBJDIR:=obj
BINDIR:=bin
INSTALLER:=installer
LOADER:=loader
CONFIG_H:=config.h
DISTDEL:=*~ *.bak *.old DEADJOE
INTINCLUDE:=include
EXTINCLUDE:=

# for production
#CFLAGS:=-O2 
# for debugging
CFLAGS=-g

# Required flags
LDFLAGS:=

SPLINTFLAGS:= +allglobs

# ------------------------------------------------------------------------
# --------------------- Do Not Edit below this line! ---------------------
# ------------------------------------------------------------------------

CFLAGS+=-fPIC

EMPTY:=
SPACE:=$(EMPTY) $(EMPTY)
NO_CPUS:=$(shell expr `egrep "^processor" /proc/cpuinfo 2>/dev/null | tail -n1 | sed -e 's/^[^:]*[    ]*: \(.*\)/\1/'` + 1 )
INCFLAGS:=$(foreach include,$(INTINCLUDE),-I $(PWD)/$(include) )
INCFLAGS+=$(foreach include,$(EXTINCLUDE),-I $(include) )
FARG='\( -name $(subst $(SPACE), -or -name ,$(strip $(foreach include,$(strip $(DISTDEL)),"$(include)"))) \)'

CFLAGS+=$(shell ./gccflags ${CC})

MAKEARGS:='CFLAGS=${CFLAGS}' 'CC=${CC}' 'LDFLAGS=${LDFLAGS}' \
	'INCFLAGS=${INCFLAGS}' 'OBJDIR=${OBJDIR}' 'BASE=${PWD}' \
	'BINDIR=${BINDIR}' 'TARGET=${TARGET}'

all: build

.PHONY: install
install: build
	@sh ${INSTALLER}

.PHONY: debian
debian: build
	@sh ${DEBIAN}

.PHONY: build
build: 
	@if [ ! -f ${INSTALLER} ] ; then \
		echo "Running Config" ; \
		bash Config ; \
	fi
	@if [ ! -f $(PWD)/include/$(CONFIG_H) ] ; then \
		echo "Running Config" ; \
		bash Config ; \
	fi
	@if [ ! -d $(BINDIR) ] ; then \
		echo "mkdir $(BINDIR)" ; \
		mkdir -p $(BINDIR) ; \
	fi
	@for i in $(SUBDIRS); do \
		( echo Entering $$i) ;\
		( cd $$i; ${MAKE} -s ${MAKEARGS} SUBDIR=$$i -j build; ) ; \
	done

.PHONY: clean
clean:
	@ echo Emptying ${PWD}/${OBJDIR}/
	@ find -P ${PWD}/${OBJDIR}/* -not -wholename "*CVS*" -delete 2>/dev/null ; true
	@ find -P ${PWD}/${BINDIR}/* -not -wholename "*CVS*" -delete -printf "Removing %p\n" 2>/dev/null ; true

.PHONY: distclean
distclean: clean
	@ echo distcleaning base directory
	@ ( rm -f Config.options $(INSTALLER) $(LOADER) include/${CONFIG_H} $(DISTDEL) )
	@ ( for i in $(SUBDIRS); do \
		echo "distcleaning $$i";\
		cd $$i ;\
		eval find . -type f ${FARG} -delete ;\
	done )
	@for i in $(INTINCLUDE); do \
        	echo "distcleaning $$i";\
        	( cd $$i; eval find . -type f ${FARG} -delete ; ) ; \
        done

.PHONY: splint
splint:
	@( splint $(SPLINTFLAGS) $(INCFLAGS) src/*.c)  
