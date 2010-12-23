SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

ifeq ($(WIN32), yes)
	ifeq ($(STATIC), yes)
		LIB     = libduct.a
		INSTALL = 
	else
		LIB     = libduct.dll
		INSTALL = 
	endif
else
	ifeq ($(STATIC), yes)
		LIB     = libduct.a
		INSTALL = 
	else
		LIB     = libduct.so
		INSTALL = && $(LN) $(LNFLAGS) $(LIB).$(VERSION) $(DESTLIBDIR)/$(LIB)
	endif
endif

LIBNAME = $(LIBPATH)/$(LIB).$(VERSION)

all: $(LIB) clean

compile: $(LIB)

libduct.a: $(OBJ)
	@(mkdir -p $(LIBPATH))
	$(AR) $(ARFLAGS) $(LIBNAME) $(OBJ)
	$(LN) $(LNFLAGS) $(LIB).$(VERSION) $(LIBPATH)/$(LIB)

libduct.so: $(OBJ)
	@(mkdir -p $(LIBPATH))
	$(CPP) $(LDFLAGS) -Wl,-soname,$(LIB) -o $(LIBNAME) $(OBJ)
	$(LN) $(LNFLAGS) $(LIB).$(VERSION) $(LIBPATH)/$(LIB)

libduct.dll: $(OBJ)
	@(mkdir -p $(LIBPATH))
	$(CPP) $(LDFLAGS) -Wl,-soname,$(LIB) -o $(LIBNAME) $(OBJ)
	$(LN) $(LNFLAGS) $(LIB).$(VERSION) $(LIBPATH)/$(LIB)

$(OBJ): %.o: %.cpp
	$(CPP) -o $@ -c $< $(CFLAGS)

clean:
	rm -f $(OBJ)

install:
	@(mkdir -p $(DESTLIBDIR))
	@($(CP) $(LIBNAME) $(DESTLIBDIR) $(INSTALL))

