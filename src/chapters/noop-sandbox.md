# Retrofitting isolation in a simple application

For our tutorial, we're going to be sandboxing a tiny library ``mylib``. While
this library is very simple, it exercises key features of RLBox including:
calling functions, copying strings into the sandbox, registering and handling
callbacks from the library.

This first part of the tutorial is going to focus on modify our application to
add sandboxing, the [next part](./wasm-sandbox.md) will focus on recompiling our library with wasm to
enforce isolation.

In this example, we're going to use the _noop_ sandbox backend. The _noop_
sandbox does not actually enforce isolation, it is simply a tool that makes it
easier to port new libraries to RLBox. The _noop_ sandbox does nothing more than
turn our calls into the RLBox sandbox into normal function calls to the library
we already have linked in our application.

The reason for this `noop` backend is that it supports incrementally porting our
application. Instead of having to worry about trying to change all our library
interfaces at once (to account for ABI differences between a sandbox and our
normal library), and deal with the resulting head-aches. We can change gradually
change our function calls from normal library calls, to sandbox calls, and at
each step test that our application continues to work as expected.


### Our example library

``mylib`` declares four functions in [mylib.h](examples/noop-hello-example/mylib.h):

```c
{{#include examples/noop-hello-example/mylib.h}}
```

And implements those function in [mylib.c](./examples/noop-hello-example/mylib.c):

```c
{{#include examples/noop-hello-example/mylib.c}}
```


### Boilerplate

To get started, in our main application ([main.cpp](examples/noop-hello-example/main.cpp))
let's first import the RLBox library and implement some necessary boilerplate:

```cpp
{{#include examples/noop-hello-example/main.cpp:imports}}
  // ... will fill in shortly ...
{{#include examples/noop-hello-example/main.cpp:main-end}}
```

Why the boilerplate? RLBox has support for different kinds of sandboxing
back-ends. In practice we start with the _noop_ sandbox, which is not a real
sandbox, to get our types right and only at the end change from `noop` to a
real sandbox like Wasm. This, alas, means the RLBox types are
typically generic in the sandbox type (e.g., `rlbox::tainted<T,
sandbox_type>`); macros like `RLBOX_DEFINE_BASE_TYPES_FOR` define simpler types
for us (e.g., we can use `tainted_mylib<T>`). In this simple example we only
use the noop sandbox; we walk through how you modify this code to use Wasm [in
the next chapter](/chapters/wasm-sandbox.md).

### Creating sandboxes and calling sandboxed functions

Now that the boilerplate is out of the way, let's create a new sandbox and
call the `hello` function:

```cpp
{{#include examples/noop-hello-example/main.cpp:hello}}
```

We do not call `hello()` directly. Instead, we use the
`invoke_sandbox_function()` method. Once we turn on sandboxing, i.e., switch
from the noop sandbox to Wasm, we won't be able to call the function directly
either (e.g., because Wasm's
[ABI](https://en.wikipedia.org/wiki/Application_binary_interface) might be
different from the app).

### Calling sandboxed functions and verifying their return value

Let's now call the `add` function:

```cpp
{{#include examples/noop-hello-example/main.cpp:add}}
```

This call is a bit more interesting. First, we call `add` with arguments. Since
these arguments are primitive types RLBox doesn't impose any restrictions.
Second, RLBox ensures that the `unsigned` return value that `add` returns is
_tainted_ and thus cannot be used without verification. For example,
Here, we call the
`copy_and_verify()` method which copies the value into application memory and
runs our verifier function:

```cpp
[](unsigned ret){
      printf("Adding... 3+4 = %d\n", ret);
      return ret == 7;
}
```

This function (lambda) simply prints the tainted value and returns `true` if it
is `7`. A compromised library could return any value and if we use this value
to, say, index an array this could potentially introduce an out-of-bounds
memory access.

### Calling functions with (tainted) strings

Let's now call the `echo` function which takes a slightly more interesting
argument: a string. Here, we can't simply pass a string literal as an argument:
the sandbox cannot access application memory where this would be allocated.
Instead, we must allocate a buffer in sandbox memory and copy the string we
want to pass to `echo` into this region:

```cpp
{{#include examples/noop-hello-example/main.cpp:echo-pre}}
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
{{#include examples/noop-hello-example/main.cpp:echo}}
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
{{#include examples/noop-hello-example/main.cpp:callback}}
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
{{#include examples/noop-hello-example/main.cpp:call_cb}}
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
