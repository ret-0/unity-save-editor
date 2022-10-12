BIN = "editor"

.PHONY: init
init:
	rm -rf imgui/
	git clone 'https://github.com/ocornut/imgui'
	cp imgui/backends/imgui_impl_sdl.cpp          imgui/
	cp imgui/backends/imgui_impl_opengl3.cpp      imgui/
	cp imgui/backends/imgui_impl_sdl.h            imgui/
	cp imgui/backends/imgui_impl_opengl3.h        imgui/
	cp imgui/backends/imgui_impl_opengl3_loader.h imgui/
	cp imgui/misc/cpp/imgui_stdlib.cpp imgui/
	cp imgui/misc/cpp/imgui_stdlib.h   imgui/

	cd imgui/ && git clone https://github.com/aiekick/ImGuiFileDialog.git && cd ImGuiFileDialog && git checkout Lib_Only

.PHONY: clean
clean:
	rm -rf build/*
	rm -rf out/*
	rm -rf decoder/obj/*
	rm -rf decoder/bin/*

.DEFAULT_GOAL := default
.PHONY: default
default:
	@clear
	@cd decoder && make -s
	@cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && echo && make -s -j $(shell nproc) && mv $(BIN) ..
	@strip -s $(BIN)
	@cp $(BIN) out/

.PHONY: debug
debug:
	@clear
	@cd decoder && make -s
	@cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && echo && make -s -j $(shell nproc) && mv $(BIN) ..
