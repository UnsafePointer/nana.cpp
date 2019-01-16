ifeq ($(strip $(LIBTRANSISTOR_HOME)),)
$(error "Please set LIBTRANSISTOR_HOME in your environment. export LIBTRANSISTOR_HOME=<path to libtransistor>")
endif

LLVM_POSTFIX := -5.0
PROGRAM := nanacpp
OBJ := src/CPUController.o src/Emulator.o src/InterruptController.o src/JoypadController.o src/Logger.o src/main.o src/MemoryController.o src/PPUController.o src/Register.o src/TimerController.o src/Utils.o
INCLUDE := include

include $(LIBTRANSISTOR_HOME)/libtransistor.mk

all: $(PROGRAM).nso $(PROGRAM).nro $(PROGRAM)

src/%.o: src/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -D__SDL2__=1 -D__LIBTRANSISTOR__=1 -Wno-pointer-arith -c -o $@ $<

%.o: %.S
	$(AS) $(AS_FLAGS) $< -filetype=obj -o $@

$(PROGRAM).nro.so: ${OBJ} $(LIBTRANSITOR_NRO_LIB) $(LIBTRANSISTOR_COMMON_LIBS)
	$(LD) $(LD_FLAGS) -lSDL2 -o $@ ${OBJ} $(LIBTRANSISTOR_NRO_LDFLAGS)

$(PROGRAM).nso.so: ${OBJ} $(LIBTRANSITOR_NRO_LIB) $(LIBTRANSISTOR_COMMON_LIBS)
	$(LD) $(LD_FLAGS) -lSDL2 -lm -o $@ ${OBJ} $(LIBTRANSISTOR_NRO_LDFLAGS)

clean:
	rm -rf src/*.o *.nso *.nro *.so
