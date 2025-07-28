# Creating a noop sandbox

To get started, in our main application
([main.cpp](/examples/hello-example/main.cpp)) let's first import the RLBox
library and add some necessary boilerplate in the top of the file:

```cpp
{{#include ../../../examples/noop-hello-example/main-withanchors.cpp:imports}}
```

**Why the boilerplate?**

RLBox has support for different kinds of sandboxing back-ends/plugins, thus we
need to specify which backend we will use and their configurations. While there
is a lot of effort to avoid boilerplate, there is certain amount that either
cannot be removed, or would be too costly to remove, which are the bits you are
seeing here.

**What does this boilerplate do?**

Let's briefly go through the boilerplate to understand their specific purpose.

`#define RLBOX_SINGLE_THREADED_INVOCATIONS` tells RLBox that only one thread in
our host application will invoke functions in the sandboxed library at a given
time (There can be multiple application threads that all invoke functions, but
the host application must ensure that only one thread executes functions in the
sandboxed library. This can be done with a per-sandbox lock.). This macro allows
RLBox to elide several internal mutex calls that greatly speeds up its
performance. If you want to support multiple threads calling into the same
sandbox, you can avoid this macro.

`#define RLBOX_USE_STATIC_CALLS() rlbox_noop_sandbox_lookup_symbol` tells RLBox
that the `noop` sandbox makes static function calls into the library. For
technical reasons, not all sandboxes support direction function calls. So, some
sandbox backends may rely on indirect function calls via pointers into the
sandboxed library. Unfortunately, this is something RLBox must know up front, so
we have specify this. For all practical purposes, just think of this line as
something you should specify as is. Each sandbox backend/plugin will have a
version of this line in its documentation that you can copy as is.

We have `#include`d two headers, `rlbox.hpp` which is the base rlbox library,
and `rlbox_noop_sandbox` for the `noop` sandbox backend.

`RLBOX_DEFINE_BASE_TYPES_FOR` defines `tainted` types that are specific for each
library. In our example, this macro now gives us `tainted_mylib<T>`, which will
automatically map to `rlbox::tainted<T, rlbox_noop_sandbox>`. If we change the
sandbox plugin/backend in the future, the mapping will change automatically.

## Creating and destroying sandboxes

Now that the boilerplate is out of the way, let's create a new sandbox instance
in the top of the main function that we will use in this application.

```cpp
{{#include ../../../examples/noop-hello-example/main-withanchors.cpp:create}}
```

and destroy the sandbox at the end of main

```cpp
{{#include ../../../examples/noop-hello-example/main-withanchors.cpp:destroy}}
```

> **Note**: We can create multiple sandbox instances if we wanted. You can think
> of each sandbox instance as an isolated instance of the library. Each instance
> cannot interfere with another instance.
>
> To see where this could be useful, consider securing a webserver that parses
> XML data from each incoming connection. You could sandbox the XML parsing
> library and spin up a single sandbox. This would ensure the server doesn't get
> compromised due to an XML parsing bug, however it won't prevent one malicious
> connection from interfering with the parsed XML contents of a different
> connection. However, you could spin up a new sandboxed XML-parser library
> instance for each incoming connection. This architecture would guarantee that
> a bug while processing an XML parameter in one of the connections will not
> spill over to processing of other connections.
