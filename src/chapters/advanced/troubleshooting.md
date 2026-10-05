# Miscellaneous troubleshooting

## Assigning 0 or NULL to tainted pointers is not supported

Unfortunately, `NULL` in C++ is types as int and this makes it indistinguishable
from any other integer. So, RLBox does not allow zeroing out pointers with `0`
or `NULL`. You can, however, pass `NULL` using the C++ `nullptr` keyword.

## I cannot call `copy_and_verify` on `tainted<void*>`

RLBox does not allow `copy_and_verify` on `tainted<void*>` as it could lead to
some anti-patterns in verifiers. Cast it to a different tainted pointer with
[`sandbox_reinterpret_cast`](/chapters/advanced/casting-tainted-values.md) and
then call the appropriate verification API. If you only need the address as an
opaque value, consider
`copy_and_verify_address`.

You can use `UNSAFE_unverified` to remove the wrapper without casting, but this
performs no verification or range check. It should only be a temporary measure
during incremental migration. See [Using `UNSAFE_unverified` during
migration](/chapters/advanced/unsafe-unverified.md) for details.

## `tainted` has the wrong number of template arguments

The generic `rlbox::tainted` type requires both a value type and a sandbox type,
so `tainted<int>` produces a "wrong number of template arguments" error. Use the
library-specific type created by `RLBOX_DEFINE_BASE_TYPES_FOR` instead:

```cpp
tainted_mylib<int> value = ...;
```

This is equivalent to `rlbox::tainted<int, rlbox_noop_sandbox>`, but will
automatically use the sandbox type configured for `mylib`.

## Mismatched arguments types for function `rlbox::detail::polyfill::is_invocable_v` ...

RLBox checks that the function arguments passed in to the
`sandbox.invoke_sandbox_function` match the argument types of the function
being invoked. For example a function `void foo(unsigned int a)` expects just
one `unsigned int` argument etc. Passing in a parameter with the wrong type
would return an error just like C/C++.

Using the RLBox [Casting
operations](/chapters/advanced/casting-tainted-values.md) that cast tainted
values could be helpful to address these errors.
```
