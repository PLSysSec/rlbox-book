# Getting raw pointers into the sandbox memory

Typically, RLBox does not let you create raw pointers into sandbox memory, i.e.,
pointers of the form `char*`. Rather the pointers will be wrapped as
`tainted<char*>`. However, there maybe certain scenarios where you really need a
raw pointer into sandbox memory.

You can do this with the `unverified_safe_pointer_because` API. This converts a
`tainted` pointer to a raw pointer with only minimal verification.

`unverified_safe_pointer_because` takes two parameters. The first is the number
of bytes in this pointer that you will be accessing. RLBox needs this to ensure
that these many bytes of the pointer stay within the sandbox boundary. The
second is a string, that allows the developer to document why they are doing
this and why its safe. This string does not have any special meaning in the
code. Rather the RLBox API asks you to provide a free-form string that acts as
documentation. Essentially you are providing a string that says _it is safe to
remove the tainting from this type because..._ . Such documentation may be
useful to other developers who read your code.

```cpp
tainted<char*> a = sandbox.malloc_in_sandbox(12);
char* raw = a.unverified_safe_pointer_because(10, "Demo of a raw pointer");
```
