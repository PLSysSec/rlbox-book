# Handling tainted strings

Let's now sandbox the call to the `echo` function which takes a slightly more
interesting argument: a string. Here, we can't simply pass a string literal as
an argument: the sandbox cannot access application memory where this would be
allocated. RLBox thus prevents this code from compiling.

To fix the compilation error, we must allocate a buffer in sandbox memory and
copy the string we want to pass to `echo` into this region. We can do this with
the following code:

```cpp
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:echo-pre}}
```

Here `taintedStr` is a tainted string: it lives in the sandbox memory and could
be written to by the (compromised) library code concurrently. We have allocated
this my calling the `malloc_in_sandbox` API.

After this, we have to copy the string to the sandbox. Normally, we could copy
strings with `strncpy`, however, this is of type `tainted<char*>`. To make this
simpler, RLBox provides an `rlbox::strncpy` which allows passing tainted strings
as destinations. The only difference in the signature of `rlbox::strncpy`
compared to `strncpy` is that the first parameter must be the sandbox.
Internally, RLBox ensures that the string copies remain within the sandbox
boundary.

> **Note**: if you do need to convert a tainted pointer to a raw pointer, you
> can do so by following the approach listed in the advanced topics
> [chapter](../advanced/raw-sandbox-pointers.md)

Now, we can just call the function and free the allocated string:

```cpp
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:echo}}
```
