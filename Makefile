.PHONY: book serve

book:
	mdbook build
	grep -v '^\s*// ANCHOR' src/chapters/examples/hello-example/main-withanchors.cpp > src/chapters/examples/hello-example/main.cpp
	grep -v '^\s*// ANCHOR' src/chapters/examples/noop-hello-example/main-withanchors.cpp > src/chapters/examples/noop-hello-example/main.cpp
	grep -v '^\s*// ANCHOR' src/chapters/examples/wasm-hello-example/main-withanchors.cpp > src/chapters/examples/wasm-hello-example/main.cpp
	./cpIfDifferent.sh src/chapters/examples/hello-example/mylib.c src/chapters/examples/noop-hello-example/mylib.c
	./cpIfDifferent.sh src/chapters/examples/hello-example/mylib.c src/chapters/examples/wasm-hello-example/mylib.c
	./cpIfDifferent.sh src/chapters/examples/hello-example/mylib.h src/chapters/examples/noop-hello-example/mylib.h
	./cpIfDifferent.sh src/chapters/examples/hello-example/mylib.h src/chapters/examples/wasm-hello-example/mylib.h

test:
	cmake -S src/chapters/examples/hello-example -B src/chapters/examples/hello-example/build -DCMAKE_BUILD_TYPE=Debug
	cmake --build src/chapters/examples/hello-example/build --config Debug --parallel
	cmake -S src/chapters/examples/noop-hello-example -B src/chapters/examples/noop-hello-example/build -DCMAKE_BUILD_TYPE=Debug
	cmake --build src/chapters/examples/noop-hello-example/build --config Debug --parallel
	cmake -S src/chapters/examples/wasm-hello-example -B src/chapters/examples/wasm-hello-example/build -DCMAKE_BUILD_TYPE=Debug
	cmake --build src/chapters/examples/wasm-hello-example/build --config Debug --parallel

serve: book
	mdbook serve
