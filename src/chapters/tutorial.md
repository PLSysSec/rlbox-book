# The RLBox Tutorial

In this tutorial we will walk you through the steps of adding sandboxing to a
very simple application and library. However, all the basic step generalize to
more complex examples.

We will start by describing the [simple application](./example.md) that
uses a library, and then describe how to sandbox this in two parts.

- In the [first part](./noop-sandbox.md), we will look at how to use
RLBox to retrofit sandboxing in an existing application, taking all the steps to
ensure that control flow and data flow across the application library boundary
are secure.

- In the [second part](./wasm-sandbox.md), we will look at how to
re-build our library with wasm and link this into our application, so our
library is isolated.

Once we complete these two steps, our library is now securely isolated from our
application.

## Downloading and running the examples

To get the source code for the examples in the tutorial, download the repo as
shown below:

```bash
git clone https://github.com/PLSysSec/rlbox-book
```

The chapters going forward will give commands on how to build and run these
examples.

The examples in this tutorial will be self-contained and will pull those repos
as needed. However, for reference, RLBox currently spans two repositories. One
that contains just the RLBox C++ framework and the other, which contains the
RLBox plugin for Wasm files compiled with the `wasm2c` compiler (which converts
your C library to an isolated and sandboxed version). The two repos are
available here:

- The core RLBox library is available at [https://github.com/PLSysSec/rlbox](https://github.com/PLSysSec/rlbox)
- The Wasm2c RLBox plugin is available at [https://github.com/PLSysSec/rlbox_wasm2c_sandbox](https://github.com/PLSysSec/rlbox_wasm2c_sandbox)


