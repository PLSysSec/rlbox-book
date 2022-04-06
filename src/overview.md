<p style="text-align:center"><img src="images/rlbox-logo.svg" alt="RLBox" /></p>

# Overview

RLBox is a toolkit for sandboxing third-party libraries. The toolkit consists
of (1) a Wasm-based sandbox and (2) an API for retrofitting existing
application code to interface with a sandboxed library. In this overview, we
focus on the API, which abstracts over the underlying sandboxing mechanism.
This lets you port your application without worrying about the Wasm sandboxing
details. The Wasm-based sandbox is documented in a [separate chaper](chapters/sandbox/wasm.md).

### Why do we need a sandboxing API?

Sandboxing libraries without the RLBox API is tedious and error-prone.
This is especially the case when retrofitting an existing codebase like Firefox
where libraries are trusted and thus the application-library boundary is
blurry.  To sandbox a library — and thus to move to a world where the library
is no longer trusted — we need to modify this application-library boundary. 
For example, we need to add security checks in Firefox to ensure that any value
from the sandboxed library is properly validated before it is used.  Otherwise,
the library (when compromised) may be able to abuse Firefox code to hijack its
control flow [^RLBoxPaper]. The RLBox API
is explicitly designed to make retrofitting of existing application code
simpler and less error-prone.[^RLBoxLogin]


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
considers all such values as untrusted and [taints](chapters/api/tainted.md)
them. Tainted values are essentially opaque values (though RLBox does provide
some basic operators on tainted values). To use a tainted value, you must
unwrap it by (typically) copying the value into application memory -- and thus
out of the reach of the attacker -- and *verifying* it. Indeed, RLBox forces
application code to perform the copy and verification in sync using
verification functions (see [this chapter](chapers/api/tainted.md)).

### References

[^RLBoxPaper]: [Retrofitting Fine Grain Isolation in the Firefox Renderer](https://www.usenix.org/conference/usenixsecurity20/presentation/narayan) by S. Narayan, et al.

[^RLBoxLogin]: [The Road to Less Trusted Code: Lowering the Barrier to In-Process Sandboxing](https://www.usenix.org/publications/login/winter2020/garfinkel-tal) by T. Garfinkel et al.

[^RLBoxFirefox]: [WebAssembly and Back Again: Fine-Grained Sandboxing in Firefox 95](https://hacks.mozilla.org/2021/12/webassembly-and-back-again-fine-grained-sandboxing-in-firefox-95/) by B. Holley
