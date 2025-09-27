# Tainted computations

RLBox does not permit some operations on tainted values for safety reasons

- Using a `tainted<unsigned int>` to index an array in the host, i.e., `int[]`.
  This can lead to an out of bounds array access if allowed.
- Branching on tainted values. How to handle this is covered in more detail in
  the chapter on [tainted branching](/chapters/advanced/tainted-bool.md).
- Comparison on the result of dereferencing a tainted pointer. This produces a
  `tainted_bool_hint` as  discussed in the chapter on [tainted
  branching](/chapters/advanced/tainted-bool.md).
