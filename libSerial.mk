# make fragment for SERIAL devices data library
# input argument LIBSERIAL
# outputs OBJ_DIR to create

# Other macros used by bigger build system 


SERIAL_PATH    = $(LIBSERIAL)/src
SERIAL_SRC     = $(SERIAL_PATH)
SERIAL_INC     = $(LIBSERIAL)/inc
SERIAL_OBJ     = $(OBJPATH)/libSerial
SERIAL_OBJECTS = $(SERIAL_OBJ)/serial.o
SERIAL_OBJECTS += $(SERIAL_OBJ)/serialUtils.o

# build these sources 
$(SERIAL_OBJ)/%.o: $(SERIAL_SRC)/%.c $(DEPENDS)
	$(CC) $(CFLAGS)   $< -o $@

# C_INCLUDES and C_OBJECTS used by simple makefiles, 
# other macros are for more complex makefiles 

C_INCLUDES += -I$(SERIAL_INC)
C_OBJECTS += $(SERIAL_OBJECTS)

# include platform.c here as we are building from source
# If it was being used as  a library, 
#   we compile platform.c with the application, not here

C_SOURCES += $(LIBSERIAL)/src/serial.c
C_SOURCES += $(LIBSERIAL)/src/serialUtils.c
C_SOURCES += $(LIBSERIAL)/src/platform.c

# platform.C belongs to the application to compile 
# C_SOURCES += $(LIBSERIAL)/src/platform.c

ifeq ($(PREBUILT_LIBS),1)

LIBS += -lserial
LIBDIR += -L $(LIBSERIAL)/lib

endif
