# Handling callbacks

Finally, let's sandbox the call to the `call_cb` function. To do this, we need
to modify the callback to have a signature that RLBox permits. Currently the
callback looks like this:

```cpp
{{#include ../../examples/hello-example/main-withanchors.cpp:callback}}
```

To modify this to a signature RLBox will allow, we need to
- Set the first parameter to be a reference to the sandbox
- Make all parameters and returns a tainted value. (A `void` return does not
  need to be tainted)

With this change, the callback will now look like

```cpp
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:callback-signature}}
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

Note that `cb` here is an RAII type. Meaning the callback is automatically
unregistered if `cb` goes out of scope. If you want the callback to be
registered for longer, make sure to keep `cb` alive.
