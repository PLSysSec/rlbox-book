# Handling tainted string

### Calling functions with (tainted) strings

Let's now call the `echo` function which takes a slightly more interesting
argument: a string. Here, we can't simply pass a string literal as an argument:
the sandbox cannot access application memory where this would be allocated.
Instead, we must allocate a buffer in sandbox memory and copy the string we
want to pass to `echo` into this region:

```cpp
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:echo-pre}}
```

Here `taintedStr` is a tainted string: it lives in the sandbox memory and could
be written to by the (compromised) library code concurrently. In general, it's
unsafe for us to use tainted data without verification since it could be
attacker controlled. In this particular case, though, we just want to copy data
(`helloStr` specifically) to `taintedStr`. We do this by using the
`unverified_safe_pointer_because` to essentially cast `taintedStr` to a `char*`
without any verification. This is safe because we are just copying
`helloStr` to sandbox memory: at worst, the sandboxed library can overwrite the
memory region pointed to by `taintedStr` and crash when it tries to print
it.[^note-1]


> **Note:** Internally, `unverified_safe_pointer_because` is not actual just a
> cast. It also ensures (1) that the pointer is within the sandbox and that
> (2) accessing `helloSize` bytes off the pointer would stay within the sandbox
> boundary.

It's worth mentioning that the string `"writing to region"` does not have any
special meaning in the code. Rather the RLBox API asks you to provide a
free-form string that acts as documentation. Essentially you are providing a
string that says _it is safe to remove the tainting from this type because..._
. Such documentation may be useful to other developers who read your code. In
the above example, a write to the sandbox region cannot cause a memory safety
error in the application so it's safe to remove the taint.

Now, we can just call the function and free the allocated string:

```cpp
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:echo}}
```

> **Sneak peak of upcoming feature:** In an upcoming version of RLBox
> transferring a buffer into the sandbox will much simpler with a new
> [TransferBuffer](https://searchfox.org/mozilla-central/source/xpcom/base/RLBoxUtils.h)
> abstraction. To get a sneak preview of this, take a look at the usage in
> [Firefox](https://searchfox.org/mozilla-central/source/gfx/ots/RLBoxWOFF2Host.cpp#175).

### Registering and handling callbacks

Finally, let's call the `call_cb` function. To do this, let's first define a
callback for the function to call. We declared our callback in the boilerplate, but never defined the function. So let's do that at the end of the file:

```cpp
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:callback}}
```

This callback is called with a tainted string. To actually use the tainted
string we need to verify it. To do this, we use the string verification function
[`copy_and_verify_string()`](chapters/api/tainted.md) with a simple verifier:

```cpp
    str.copy_and_verify_string([](unique_ptr<char[]> val) {
        release_assert(val != nullptr && strlen(val.get()) < 1024, "val is null or greater than 1024\n");
        return move(val);
    });
```

This verifier moves the string if it is not null and if its length is less than 1KB.
In the callback we simply print this string.

Let's now continue back in `main`. To `call_cb` with the callback we first
need to register the callback - otherwise RLBox will disallow the
library-application call - and pass the callback to the `call_cb` function:

```cpp
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:call_cb}}
```

### Build and run

If you haven't installed RLBox, see the [Install](/chapters/rlbox-install.md)
chapter.

Clone this books' repository:

```bash
git clone https://github.com/PLSysSec/rlbox-book
cd rlbox-book/src/chapters/examples/noop-hello-example
```

Build:

```bash
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release
cmake --build ./build --config Release --parallel
```

Run:

```bash:
$ ./build/main
Hello from mylib
Adding... 3+4 = 7
OK? = 1
echo: hi hi!
hello_cb: hi again!
```


[^note-1]: For single threaded applications the attacker can't overwrite the pointer because we're not calling into the sandbox before calling `strncpy.
