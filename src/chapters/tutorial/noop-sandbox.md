# Retrofitting isolation in a simple application

This first part of the tutorial is going to focus on modify our application to
add sandboxing, the [next part](./wasm-sandbox.md) will focus on recompiling our
library with wasm to enforce isolation.

In this example, we're going to use the `noop` sandbox backend. The `noop`
sandbox does not actually enforce isolation, it is simply a tool that makes it
easier to port new libraries to RLBox. The `noop` sandbox does nothing more than
turn our calls into the RLBox sandbox into normal function calls to the library
we already have linked in our application.

The reason for this `noop` backend is that it supports incrementally porting our
application. Instead of having to worry about trying to change all our library
interfaces at once (to account for ABI differences between a sandbox and our
normal library), and deal with the resulting head-aches. We can change gradually
change our function calls from normal library calls, to sandbox calls, and at
each step test that our application continues to work as expected.


