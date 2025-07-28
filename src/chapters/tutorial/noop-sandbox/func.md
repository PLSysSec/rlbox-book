# Sandboxing function calls

We now move on to sandboxing the function calls made by the application to
`mylib`. We can see that the application calls the `hello` function in the
library.

```cpp
{{#include ../../../examples/hello-example/main-withanchors.cpp:hello}}
```

To sandbox this call, this is as simple as changing the syntax to:

```cpp
{{#include ../../../examples/noop-hello-example/main-withanchors.cpp:hello}}
```

We have changed our code to not call `hello()` directly. Instead, we use RLBox's
`invoke_sandbox_function()` method. This allows RLBox to mediate the function
calls into the sandbox.


### Calling sandboxed functions and verifying their return value

Let's now sandbox the call to the `add` function. We can see the our application
calls the `add` function as shown below.

```cpp
{{#include ../../../examples/hello-example/main-withanchors.cpp:add-context}}
```

To change, the function call, we will use `invoke_sandbox_function` as before.

```cpp
{{#include ../../../examples/noop-hello-example/main-withanchors.cpp:add}}
```

There are now a couple of interesting things happening.
- First, `add` has arguments. These arguments are primitive types, so RLBox
doesn't impose any restrictions and you can pass them to the sandboxed function
as is (complex arguments have some restrictions as we will see later).
- Second, if you check the type of `ret`, you'll see that RLBox ensures that the
return value from `add` is _tainted_ and thus cannot be used without
verification. Concretely the type of `ret` is now `tainted_mylib<unsigned int>`.
Thus if you try to compile this program, you will get a compilation error
stating that a value of type ``tainted_mylib<unsigned int>` cannot be used as an
array index.

TO convert `ret` back to an `unsigned int`, we will have to verify it by calling
the `copy_and_verify()` API. This API copies the value into application memory
and runs a verifier function we will have to specify. The verifier should ensure
`ret` does not contain a value that is unexpected. For now, let's add just the
call to `copy_and_verify()` so we can unwrap `ret` from a
`tainted_mylib<unsigned int>` to an `unsigned int` without worrying about the
verifier.

```cpp
{{#include ../../../examples/noop-hello-example/main-withanchors.cpp:add}}
{{#include ../../../examples/noop-hello-example/main-withanchors.cpp:add-copy-and-verify}}
    // .. to be specified ..
{{#include ../../../examples/noop-hello-example/main-withanchors.cpp:add-copy-and-verify-close}}
```

In the next chapter, we will discuss what we can put in the verifier to ensure
the safety of `ret`.
