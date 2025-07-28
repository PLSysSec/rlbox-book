# Tainted computations

RLBox does not permit some operations on tainted values for safety reasons

- Using a `tainted<unsigned int>` to index an array in the host, i.e., `int[]`.
  This can lead to an out of bounds array access if allowed.
- Branching on a tainted value, i.e., `if` conditions with tainted values in the
  condition, `for` loops with tainted values in the condition. The only
  exception here is comparing a tainted pointer to `nullptr`. So assuming `ret`
  is a `tainted<int>` , code of the form `if (ret == 4) {...}` would not be
  allowed. This is because any scenario where an application's control flow is
  determined by a tainted value is inevitably going to result in security
  issues. Thus, RLBox returns `tainted<bool>` during comparisons like `ret == 4`
  which do not allow branching or loops. You can, of course, try to verify the
  `tainted<bool>` using `copy_and_verify`, or better, verify `ret` prior to
  comparison to avoid this issue.
- Any form of comparison on the result of dereferencing a tainted pointer.
  Concretely, code of the form
  ```cpp
  tainted<int*> a = ...;
  tainted<bool> b = *(*a == 4)*;
  ```
  would not compile. This is because dereferencing a tainted pointer refers to a
  location in the sandbox's memory, and the sandbox could change that at any
  moment. To represent the volatility of this comparison, RLBox returns a new
  type `tainted_bool_hint` with the result of this comparison.
