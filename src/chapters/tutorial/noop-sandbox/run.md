# Building and running

To build this example on your machine, run the following commands

```bash
cd cd rlbox-book/src/chapters/examples/noop-hello-example
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug
cmake --build ./build --config Debug --parallel
```

Then run it to make sure everything is working.

```bash
./build/main
```

You should see the following output:

```bash
Hello from mylib
Got array value 7
echo: hi hi!
hello_cb: hi again!
```
