# Miscellaneous troubleshooting

## Assigning 0 or NULL to tainted pointers is not supported

Unfortunately, `NULL` in C++ is types as int and this makes it indistinguishable
from any other integer. So, RLBox does not allow zeroing out pointers with `0`
or `NULL`. You can, however, pass `NULL` using the C++ `nullptr` keyword.

## I cannot call `copy_and_verify` on `tainted<void*>`

RLBox does not allow `copy_and_verify` on `tainted<void*>` as it could lead to
some anti-patterns in verifiers. Cast it to a different tainted pointer with
`sandbox_reinterpret_cast` and then call the appropriate verification API. If
you only need the address as an opaque value, consider
`copy_and_verify_address`.

You can use `UNSAFE_unverified` to remove the wrapper without casting, but this
performs no verification or range check. It should only be a temporary measure
during incremental migration. See [Using `UNSAFE_unverified` during
migration](/chapters/advanced/unsafe-unverified.md) for details.
