
ifndef $(DEBUGBUILD)
	export DEBUGBUILD = yes
endif
ifndef $(STATIC)
	export STATIC = no
endif

# Define as 'yes' to build using Linux-compiled mingw32
ifndef $(BUILDW32FROMUNIX)
	export BUILDW32FROMUNIX = no
endif

ifeq ($(DEBUGBUILD), yes)
    DEBUGFLAGS = -g -DDEBUG
else
    DEBUGFLAGS = -O2 -DNDEBUG
endif

BUILDFLAGS = 
LINKFLAGS  = 

ifeq ($(STATIC), no)
	LINKFLAGS  = -shared
	ifeq ($(BUILDW32FROMUNIX), no)
		BUILDFLAGS = -fPIC
	endif
endif

ifeq ($(BUILDW32FROMUNIX), yes)
	export WIN32   = yes
	export CC      = i586-mingw32msvc-gcc
	export CPP     = i586-mingw32msvc-g++
	export AR      = i586-mingw32msvc-ar
	export CP      = cp
	export LN      = ln
	ifeq ($(DEBUGBUILD), yes)
		export LIBPATH = ../../lib/win32/debug
	else
		export LIBPATH = ../../lib/win32/release
	endif
else
	export CC      = gcc
	export CPP     = g++
	export AR      = ar
	export CP      = cp
	export LN      = ln
	ifeq ($(DEBUGBUILD), yes)
		export LIBPATH = ../../lib/linux/debug
	else
		export LIBPATH = ../../lib/linux/release
	endif
endif

export CFLAGS     = -W -Wall -pedantic -I../../include -licui18n -licudata -licuio -licuuc $(DEBUGFLAGS) $(BUILDFLAGS)
export LDFLAGS    = $(LINKFLAGS)
export VERSION    = 0.1
export LNFLAGS    = -s -f
export ARFLAGS    = rcs
export DESTDIR    = /usr/local
export DESTLIBDIR = $(DESTDIR)/lib
export DESTINCDIR = $(DESTDIR)/include/duct
export SRCDIR     = src/duct
export INCDIR     = include/duct

#clean
all: compile

compile:
	@(echo "##compiling")
	@(cd $(SRCDIR) && make compile)

clean:
	@(echo "##cleaning")
	@(cd $(SRCDIR) && make clean)

install:
	@(echo "##installing")
	@(cd $(SRCDIR) && make install)
	@(cd $(INCDIR) && make install)

docs:
	@(echo "##generating documentation")
	@(cd docs/ && doxygen Doxyfile)

.PHONY: clean, docs

