# Getting raw pointers into the sandbox memory

Typically, RLBox does not let you create raw pointers into sandbox memory, i.e.,
pointers of the form `char*`. Rather the pointers will be wrapped as
`tainted<char*>`. However, there maybe certain scenarios where you really need a
raw pointer into sandbox memory.

You can do this with the `unverified_safe_pointer_because` API. This converts a
`tainted` pointer to a raw pointer with only minimal verification of checking
that the pointer is within the sandbox boundary.

The details of how to use this API are provided [here](/chapters/advanced/untainting-apis.md).
