# TODO (mittonk): Remove these?
#DEBUG := 1
LIBS :=
CFLAGS :=
# \
# -I/usr/local/share/retroarch-apple-deps/sdl/include \
#  -I/usr/local/share/retroarch-apple-deps/sdl/macOS/include
  
CPPFLAGS = $(CFLAGS) -std=c++11 -Wno-mismatched-tags -Wno-reorder-ctor -Wno-constexpr-not-const -Wno-unsupported-friend -Wno-return-type -Wno-unused-variable -Wno-reorder

STATIC_LINKING := 0
AR             := ar

ifneq ($(V),1)
   Q := @
endif

ifneq ($(SANITIZER),)
   CFLAGS   := -fsanitize=$(SANITIZER) $(CFLAGS)
   CXXFLAGS := -fsanitize=$(SANITIZER) $(CXXFLAGS)
   LDFLAGS  := -fsanitize=$(SANITIZER) $(LDFLAGS)
endif

# TODO (kmitton): Playing with C++11 seems more fun than following retroarch coding standards.
CXXFLAGS += -std=c++11

ifeq ($(platform),)
platform = unix
ifeq ($(shell uname -a),)
   platform = win
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   platform = osx
else ifneq ($(findstring win,$(shell uname -a)),)
   platform = win
endif
endif

# system platform
system_platform = unix
ifeq ($(shell uname -a),)
	EXE_EXT = .exe
	system_platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
	system_platform = osx
	arch = intel
ifeq ($(shell uname -p),powerpc)
	arch = ppc
else ifeq ($(shell uname -p),arm)
	arch = arm64
endif
endif
ifneq ($(findstring MINGW,$(shell uname -a)),)
	system_platform = win
endif

CORE_DIR    += .
TARGET_NAME := dice
LIBM		    = -lm

ifeq ($(ARCHFLAGS),)
# TODO (kmitton): arch vs archs, compare to skeletor
ifeq ($(arch),ppc)
   ARCHFLAGS = -arch ppc -arch ppc64
else ifeq ($(arch),arm64)
   ARCHFLAGS = -arch arm64
else
   ARCHFLAGS = -arch i386 -arch x86_64
endif
endif

ifeq ($(platform), osx)
ifndef ($(NOUNIVERSAL))
   CXXFLAGS += $(ARCHFLAGS)
   LFLAGS += $(ARCHFLAGS)
endif
endif

ifeq ($(STATIC_LINKING), 1)
EXT := a
endif

ifeq ($(platform), unix)
	EXT ?= so
   TARGET := $(TARGET_NAME)_libretro.$(EXT)
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=$(CORE_DIR)/link.T -Wl,--no-undefined
else ifeq ($(platform), linux-portable)
   TARGET := $(TARGET_NAME)_libretro.$(EXT)
   fpic := -fPIC -nostdlib
   SHARED := -shared -Wl,--version-script=$(CORE_DIR)/link.T
	LIBM :=
else ifneq (,$(findstring osx,$(platform)))
   TARGET := $(TARGET_NAME)_libretro.dylib
   fpic := -fPIC
   SHARED := -dynamiclib
else ifneq (,$(findstring ios,$(platform)))
   TARGET := $(TARGET_NAME)_libretro_ios.dylib
	fpic := -fPIC
	SHARED := -dynamiclib

ifeq ($(IOSSDK),)
   IOSSDK := $(shell xcodebuild -version -sdk iphoneos Path)
endif

	DEFINES := -DIOS
	CC = cc -arch armv7 -isysroot $(IOSSDK)
ifeq ($(platform),ios9)
CC     += -miphoneos-version-min=8.0
CXXFLAGS += -miphoneos-version-min=8.0
else
CC     += -miphoneos-version-min=5.0
CXXFLAGS += -miphoneos-version-min=5.0
endif
else ifneq (,$(findstring qnx,$(platform)))
	TARGET := $(TARGET_NAME)_libretro_qnx.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=$(CORE_DIR)/link.T -Wl,--no-undefined
else ifeq ($(platform), emscripten)
   TARGET := $(TARGET_NAME)_libretro_emscripten.bc
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=$(CORE_DIR)/link.T -Wl,--no-undefined
else ifeq ($(platform), vita)
   TARGET := $(TARGET_NAME)_vita.a
   CC = arm-vita-eabi-gcc
   AR = arm-vita-eabi-ar
   CXXFLAGS += -Wl,-q -Wall -O3
	STATIC_LINKING = 1
else
   CC = gcc
   TARGET := $(TARGET_NAME)_libretro.dll
   SHARED := -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=$(CORE_DIR)/link.T -Wl,--no-undefined
endif

LDFLAGS += $(LIBM)

ifeq ($(DEBUG), 1)
   CFLAGS += -O0 -g -DDEBUG
   CXXFLAGS += -O0 -g -DDEBUG
else
   CFLAGS += -O3
   CXXFLAGS += -O3
endif

include Makefile.common

OBJECTS := $(SOURCES_C:.c=.o) $(SOURCES_CXX:.cxx=.o) $(SOURCES_CPP:.cpp=.o)

CFLAGS   += -Wall -D__LIBRETRO__ $(fpic)
CXXFLAGS += -Wall -D__LIBRETRO__ $(fpic)

all: $(TARGET)

$(TARGET): $(OBJECTS)
ifeq ($(STATIC_LINKING), 1)
	$(AR) rcs $@ $(OBJECTS)
else
	@$(if $(Q), $(shell echo echo LD $@),)
	$(Q)$(CXX) $(fpic) $(SHARED) $(INCLUDES) -o $@ $(OBJECTS) $(LDFLAGS)
endif

%.o: %.c
	@$(if $(Q), $(shell echo echo CC $<),)
	$(Q)$(CC) $(CFLAGS) $(fpic) -c -o $@ $<

%.o: %.cxx
	@$(if $(Q), $(shell echo echo CXX $<),)
	$(Q)$(CXX) $(CXXFLAGS) $(fpic) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: clean

print-%:
	@echo '$*=$($*)'


# TODO (mittonk): Integrate up.
#CPP  = g++
#CC   = gcc
#RM   = rm -f
#MOC  = moc-qt4

#CHIP_OBJ := chips/7400.o chips/7402.o chips/7404.o chips/7407.o chips/7408.o chips/7410.o chips/7411.o chips/7420.o chips/7421.o chips/7425.o \
   			chips/7427.o chips/7430.o chips/7432.o chips/7437.o chips/7442.o chips/7448.o chips/7450.o chips/7454.o chips/7474.o chips/7475.o \
			chips/7476.o chips/7483.o chips/7485.o chips/7486.o chips/7490.o chips/7492.o chips/7493.o chips/7495.o chips/7496.o chips/74107.o \
		   	chips/74109.o chips/74S112.o chips/74116.o chips/74121.o chips/74150.o chips/74151.o chips/74153.o chips/74155.o chips/74164.o \
		   	chips/74165.o chips/74166.o chips/74174.o chips/74175.o chips/74191.o chips/74192.o chips/74193.o chips/74194.o chips/74279.o \
		   	chips/9301.o chips/9310.o chips/9311.o chips/9312.o chips/9314.o chips/9316.o chips/9321.o chips/9322.o \
		   	chips/9602.o chips/555astable.o chips/555mono.o chips/555pwm.o chips/74S287.o chips/8225.o chips/8277.o chips/2533.o \
			chips/82S16.o chips/82S115.o chips/82S123.o chips/82S131.o chips/TMS4800.o \
			chips/clock.o chips/capacitor.o chips/diode_matrix.o chips/latch.o chips/clk_gate.o chips/wired_logic.o \
			chips/mixer.o chips/566.o \
			chips/input.o chips/audio.o chips/video.o chips/dipswitch.o chips/rom.o chips/vcd_log.o chips/wav_log.o 

#GAME_OBJ := games/pong.o games/rebound.o games/gotcha.o games/spacerace.o games/stuntcycle.o games/pongdoubles.o \
   			games/tvbasketball.o games/breakout.o games/antiaircraft.o games/attack.o \
		   	games/sharkjaws.o games/quadrapong.o games/jetfighter.o games/crashnscore.o \
			games/crossfire.o games/pinpong.o games/cleansweep.o games/wipeout.o \
			games/highway.o games/steeplechase.o games/indy4.o

#MANYMOUSE_OBJ := manymouse/manymouse.o manymouse/windows_wminput.o manymouse/linux_evdev.o \
				 manymouse/macosx_hidmanager.o manymouse/macosx_hidutilities.o manymouse/x11_xinput2.o

# OBJ := main.o chip.o circuit.o settings.o game_config.o phoenix/phoenix.o $(CHIP_OBJ) $(GAME_OBJ) $(MANYMOUSE_OBJ)
#OBJ := main.o chip.o circuit.o settings.o game_config.o $(CHIP_OBJ) $(GAME_OBJ)

#BIN := dice
#
#ifeq ($(PLATFORM),)
#    uname := $(shell uname -a)
#    ifeq ($(uname),)
#        PLATFORM := windows
#    else ifneq ($(findstring Darwin,$(uname)),)
#        PLATFORM := osx
#    else ifneq ($(findstring Linux,$(uname)),)
#        PLATFORM := linux
#    endif
#endif

#ifeq ($(PLATFORM),windows)
#	OBJ += ui/dice.o
#	LIBS += -lSDL -static-libgcc -static-libstdc++ -lmingw32 -lopengl32 -lkernel32 -luser32 -lgdi32 -ladvapi32 -lcomctl32 -lcomdlg32 -lshell32 -lole32 -mwindows #-mconsole
#	CFLAGS += -DPHOENIX_WINDOWS
#	BIN := dice.exe
#else ifeq ($(PLATFORM),linux)
#	OBJ := phoenix/qt/platform.moc $(OBJ)
#	LIBS += `pkg-config --libs QtCore QtGui` -lSDL -lGL -ldl -lX11
#	CFLAGS += `pkg-config --cflags QtCore QtGui` -DPHOENIX_QT
	#LIBS += `pkg-config --libs gtk+-2.0` -lX11 -lGL
	#CFLAGS += -DPHOENIX_GTK `pkg-config --cflags gtk+-2.0`
#else ifeq ($(PLATFORM),osx)
#	LIBS += -lc++ -lobjc -framework OpenGL -framework SDL -framework Cocoa -framework Carbon -framework IOKit
#	CFLAGS += -x objective-c++ -stdlib=libc++ -DPHOENIX_COCOA -F/Library/Frameworks
#endif

# For profiling
#LIBS += -pg 
#CFLAGS += -pg 

# For gcov
#CFLAGS += -fprofile-arcs -ftest-coverage

#all: $(BIN)

#clean:
#${RM} $(OBJ) $(BIN)

#$(BIN): $(OBJ)
#	$(CPP) $(filter %.o,$(OBJ)) -o "$(BIN)" $(CPPFLAGS) $(LIBS)

#%.o: %.cpp
#	$(CPP) $(CPPFLAGS) -c $< -o $@

#%.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@

#%.o: %.rc
#	windres $< $@

#%.moc: %.moc.hpp
#	$(MOC) -i -o $@ $<

#dumpasm: $(BIN)
#	objdump -d -M intel -S $(BIN) > $(BIN).s

#profile: $(BIN)
#	$(BIN)
#	gprof $(BIN) > prof.txt

#coverage: $(BIN)
#	$(BIN)
#	gcov chip.cpp

#osx: $(BIN)
#	mkdir -p dice.app/Contents/MacOS
#	mkdir -p dice.app/Contents/Resources
#	cp $(BIN) dice.app/Contents/MacOS
#	cp ui/Info.plist dice.app/Contents/Info.plist
#	cp ui/icon.icns dice.app/Contents/Resources/dice.icns
