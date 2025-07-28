.PHONY: book serve

book:
	mdbook build
	grep -v '^\s*// ANCHOR' src/examples/hello-example/main-withanchors.cpp > src/examples/hello-example/main.cpp
	grep -v '^\s*// ANCHOR' src/examples/noop-hello-example/main-withanchors.cpp > src/examples/noop-hello-example/main.cpp
	grep -v '^\s*// ANCHOR' src/examples/wasm-hello-example/main-withanchors.cpp > src/examples/wasm-hello-example/main.cpp
	./cpIfDifferent.sh src/examples/hello-example/mylib.c src/examples/noop-hello-example/mylib.c
	./cpIfDifferent.sh src/examples/hello-example/mylib.c src/examples/wasm-hello-example/mylib.c
	./cpIfDifferent.sh src/examples/hello-example/mylib.h src/examples/noop-hello-example/mylib.h
	./cpIfDifferent.sh src/examples/hello-example/mylib.h src/examples/wasm-hello-example/mylib.h

test:
	cmake -S src/examples/hello-example -B src/examples/hello-example/build -DCMAKE_BUILD_TYPE=Debug
	cmake --build src/examples/hello-example/build --config Debug --parallel
	src/examples/hello-example/build/main
	cmake -S src/examples/noop-hello-example -B src/examples/noop-hello-example/build -DCMAKE_BUILD_TYPE=Debug
	cmake --build src/examples/noop-hello-example/build --config Debug --parallel
	src/examples/noop-hello-example/build/main
	cmake -S src/examples/wasm-hello-example -B src/examples/wasm-hello-example/build -DCMAKE_BUILD_TYPE=Debug
	cmake --build src/examples/wasm-hello-example/build --config Debug --parallel
	src/examples/wasm-hello-example/build/main

serve: book
	mdbook serve
