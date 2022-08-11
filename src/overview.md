<p style="text-align:center"><img src="images/rlbox-logo.svg" alt="RLBox" /></p>

# Overview

RLBox is a toolkit for sandboxing third party C libraries, that are being used
by C++ code (support for other languages is in the works). RLBox was originally
developed for Firefox[^RLBoxPaper], which has been shipping with it since 2020.

The RLBox toolkit consists of:

1. A C++ framework (RLBox) that makes it easy to retrofit existing application
   code to safely interface with sandboxed libraries.

2. A Wasm backend (based on wasm2c) for isolating (sandboxing) C libraries.

In this section, we provide an overview of the RLbox framework, its reason for
being, and a high level sketch of how it works.  In the [next
section](./tutorial.md), we will provide a tutorial that provides an end-to-end
example of applying RLbox to a simple application.

### Why RLBox

Work on RLbox began several years ago while attempting to add fine grain
isolation third party libraries in the Firefox renderer. Initially we attempted
this process without any support from a framwork like RLBox, instead attempting
to manually deal with all the details sandboxing such as sanitizing untrusted
inputs, and reconciling ABI differences between the sandbox and host
application.

This went poorly, it was tedious, error prone, and did nothing to abstract the
details of the underlying sandbox from the developer. We had basically no hope
that this would result in code that was maintainable, or that normal Mozilla
developers who were unfamiliar with the gory details of our system would be able
to sandbox new library, let alone maintain existing ones.

So we scrapped this manual approach and build RLBox[^RLBoxPaper].

RLbox automates many of the low level details of sandboxing and allows you, as a
security engineer or application developer, to instead focus just on what you
need to do to sandbox your particular application.

To sandbox a library — and thus to move to a world where the library is
no longer trusted — we need to modify this application-library boundary.  For
example, we need to add security checks in Firefox to ensure that any value from
the sandboxed library is properly validated before it is used.  Otherwise, the
library (when compromised) may be able to abuse Firefox code to hijack its
control flow [^RLBoxPaper]. The RLBox API is explicitly designed to make
retrofitting of existing application code simpler and less
error-prone.[^RLBoxLogin]


### What does RLBox provide?

RLBox ensures that a sandboxed library is *memory isolated* from the rest of
the application — the library cannot directly access memory outside its
designated region — and that all *boundary crossings are explicit*. This
ensures that the library cannot, for example, corrupt Firefox's address space.
It also ensures that Firefox cannot inadvertently expose sensitive data to the
library. The figure below illustrates this idea.

<p style="text-align:center"><img src="images/arch.svg" alt="RLBox explicitly isolates the library data and control flow from the application" /></p>


Memory isolation is enforced by the underlying sandboxing mechanism (e.g.,
using Wasm[^RLBoxFirefox]) from the start, when you create the sandbox with
[`create_sandbox()`](chapters/api/sandbox.md). Explicit boundary
crossings are enforced by RLBox (either at compile- or and run-time). For
example, with RLBox you can't call library functions directly; instead, you
must use the [`invoke_sandbox_function()`](chapters/api/function.md) method. Similarly, the library cannot
call arbitrary Firefox functions; instead, it can only call functions that you
expose with the [`register_callback()`](chapters/api/callback.md)
method. (To simplify the sandboxing task, though, RLBox does expose a standard
library as described in the [Standard Library](chapters/api/stdlib.md).)

When calling a library function, RLBox copies simple values into the sandbox
memory before calling the function. For larger data types, such as structs and
arrays, you can't simply pass a pointer to the object. This would leak
[ASLR](https://en.wikipedia.org/wiki/Address_space_layout_randomization) and,
more importantly, would not work: sandboxed code cannot access application
memory.  So, you must explicitly allocate memory in the sandbox via
[`malloc_in_sandbox()`](chapters/api/memory.md) and copy application
data to this region of memory (e.g., via ``strlcpy``).

RLBox similarly copies simple return values and callback arguments. Larger data
structures, however, must (again) be passed by *sandbox-reference*, i.e., via a
reference/pointer to sandbox memory.

To ensure that application code doesn't unsafely use values that originate in
the sandbox -- and may thus be under the control of an attacker -- RLBox
considers all such values as untrusted and
[taints](https://shravanrn.com/oldrlboxdocs/#tainted-values) them. Tainted
values are essentially opaque values (though RLBox does provide some basic
operators on tainted values). To use a tainted value, you must unwrap it by
(typically) copying the value into application memory -- and thus out of the
reach of the attacker -- and *verifying* it. Indeed, RLBox forces application
code to perform the copy and verification in sync using verification functions
(see [this](https://shravanrn.com/oldrlboxdocs/#id15)).

### References

[^RLBoxPaper]: [Retrofitting Fine Grain Isolation in the Firefox Renderer](https://www.usenix.org/conference/usenixsecurity20/presentation/narayan) by S. Narayan, et al.

[^RLBoxLogin]: [The Road to Less Trusted Code: Lowering the Barrier to In-Process Sandboxing](https://www.usenix.org/publications/login/winter2020/garfinkel-tal) by T. Garfinkel et al.

[^RLBoxFirefox]: [WebAssembly and Back Again: Fine-Grained Sandboxing in Firefox 95](https://hacks.mozilla.org/2021/12/webassembly-and-back-again-fine-grained-sandboxing-in-firefox-95/) by B. Holley
