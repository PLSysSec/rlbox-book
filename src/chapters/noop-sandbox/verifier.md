# Verifying tainted values

Continuing our example, we need to figure out what values of `ret` are safe,
and write a verifier that checks that `ret` has one of these safe values.

So the question is: What do we put as the verifier for `ret` to remain safe?

Perhaps unsurprisingly, the answer here is "it depends". However, the intuition
is: the safety check you should put in the verifier should ensure that `ret` has
a value *that does not cause a memory safety issue in the rest of the program*.

Let's continue with our example of verifying `ret`, which is tainted return from
`add`.

Let's look at how `ret` is used to figure this out.

```cpp
{{#include ../../examples/hello-example/main-withanchors.cpp:add-context}}
```

In this example, this is simple. We can see that `ret` is only used in one
place:

```cpp
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:add-tainted-use}}
```

Thus, we simply need to ensure that `ret` isn't a value that is bigger than the
size of `array`. We can ensure this by writing the following:

```cpp
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:add}}
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:add-copy-and-verify}}
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:add-verifier}}
{{#include ../../examples/noop-hello-example/main-withanchors.cpp:add-copy-and-verify-close}}
```

Here, `release_assert` is just a small macro in the file that calls `abort()` if
the check fails.

## What happens if we get the verifier wrong?

Unfortunately, there is nothing RLBox can do to make sure that your verifier is
correct. Ultimately, verifiers is a part of your trusted code base (TCB), and
you have to get this right. However, the one upside of RLBox is that all
verifiers are clearly marked, making them easier to check during a security
audit. It is also possible that static analysis tools can be configured to
sanity check the verifiers.

## What happens if we can't figure out a verifier?

We discuss this in more detail in this advanced topics
[chapter](/chapters/verifier-strategies.md).
