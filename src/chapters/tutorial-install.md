## Setting up our tutorial environment

To start, create an `example` directory where our example app and tools will
live, and enter that directory.

```bash
mkdir example ; cd example
```

Next, clone the repo for this book, which contains our example code, and copy it
to its own directory, and build the example code.

```bash
git clone git@github.com:PLSysSec/rlbox-book.git
mkdir myapp
cp -r rlbox-book/src/chapters/examples/noop-hello-example/* myapp
cd myapp
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release
cmake --build ./build --config Release --parallel
```

Then run it to make sure everything is working.
```bash
./build/main
```

You should see the following output:
```bash
Hello from mylib
Adding... 3+4 = 7
Adding... 3+4 = 7
OK? = 1
echo: hi hi!
hello_cb: hi again!
```

Finally, return to the `example` directory, and clone and build our wasm toolchain,
which includes fork of `wasm2c`, the wasi-sdk (everything you need to compile
your C to wasm).

```bash
cd ..
git clone https://github.com/PLSysSec/rlbox_wasm2c_sandbox
cd rlbox_wasm2c_sandbox
cmake -S . -B ./build
cmake --build ./build --target all
cd ..
```

Now we have everything we need to get started.


