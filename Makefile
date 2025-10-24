MAKEFLAGS += --no-print-directory
CORES := $(shell nproc)

debug:
	@if [ ! -d build ]; then \
		mkdir -p build; \
	fi; \
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && \
	make -j$(CORES)

release:
	@if [ ! -d build ]; then \
		mkdir -p build; \
	fi; \
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && \
	make -j$(CORES)

clean:
	@if [ -d build ]; then \
		echo "Removing build folder"; \
		rm -rf build; \
	fi
