.PHONY: raygui raylib

pixelme: main.c libs/libraygui.a libs/libraylib.a
	cc main.c -o ./build/pixelme -I./libs/raygui/src/ libs/libraygui.a libs/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

# default file to open for testing
ARGS ?= "./libs/raylib/logo/raylib_24x24.png"

run: pixelme
	./build/pixelme $(ARGS)

libs/libraygui.a: libs/libraylib.a
	cd libs/ && \
	mv raygui/src/raygui.h raygui/src/raygui.c && \
	cc -c raygui/src/raygui.c -o raygui.o -DRAYGUI_IMPLEMENTATION -I./raylib/src && \
	ar rcs libraygui.a raygui.o && \
	rm raygui.o && \
	mv raygui/src/raygui.c raygui/src/raygui.h

libs/libraylib.a:
	$(MAKE) -C libs/raylib/src PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC
	cp libs/raylib/src/libraylib.a libs/libraylib.a

clean: 
	rm -f build/*
	rm -f libs/libraygui.a libs/libraylib.a