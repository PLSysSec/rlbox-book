# The RLBox Tutorial

In this tutorial we will walk you through the steps of adding sandboxing to a
very simple application and library. However, all the basic step generalize
to more complex examples.

We have broken the tutorial into two parts, in the [first
part](/chapters/noop-sandbox.md), we will look at how to use RLBox to retrofit
sandboxing in an existing application, taking all the steps to that control flow
and data flow across the application library boundary are secure.

In the [second part](/chapters/wasm-sandbox.md), we will look at how to
re-build our library with wasm and link this into our applciation, so our
library is isolated.

Once we complete these two steps, our library is now securely isolated
from our application

If you would like to download and run the examples yourself, follow the
instructions [here](/chapters/tutorial-install.md).

<!-- XXX add link to tutorial install instructions and add support for second example -->

