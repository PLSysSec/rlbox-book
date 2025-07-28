
# What happens if we can't figure out a verifier?

In larger codebases, it may not be easy to find a suitable verifier for `ret`.
This maybe because `ret` is used in a lot of places and it is difficult to
figure out all the locations where it is used. Broadly, there are a few
different strategies to deal with this that we describe below. Ultimately, you
probably want to use a mix of these strategies to make this a tractable problem

## Strategy 1: Defer verification

The first strategy to simplify identification of verifiers is to defer it. In
fact, as a general rule you should do this wherever possible. This is because
the more you defer verification, the further into your program you move the
verifier. And the further your program, it is usually much clearer what a
tainted value is used for, and thus easier to write the verifier.

To make this easier, RLBox allows a number of operations on tainted values
directly (specifically, in scenarios where RLBox can ensure their safety).

For example, if you add a line in the application

```cpp
auto ret_plus_1 = ret + 1;
```

and attempt to compile your code, you will see that the compiler does not report
any error on this line. This is because RLBox permits this operation and
`ret_plus_1` is now a `tainted_mylib<unsigned int>`, i.e., RLBox has propagated
the tainting, a "tainted computation".

Indeed there are a number of operations that are supported as "tainted
computations", and produce a new tainted value. As a few examples:

- Arithmetic on a tainted value.
- Dereferencing a tainted pointer (a tainted pointer always points to memory in
  the sandbox. RLBox automatically checks this prior to the dereference to
  ensure safety).
- Comparing a tainted pointer to `nullptr`
- Using a `tainted<unsigned int>` to index a `tainted<int[]>`

There are also operations that are not allowed for safety reasons

- Using a `tainted<unsigned int>` to index an array in the host, i.e., `int[]`.
  This can lead to an out of bounds array access if allowed.
- Branching on a tainted value, i.e., `if` conditions with tainted values in the
  condition, `for` loops with tainted values in the condition.

You can learn more about this in the advanced topics
[chapter](./tainted-computations.md)

Trying to figure out what operations are allowed or not may seem tricky, but
there is a straightforward approach. Try the operation! If RLBox doesn't throw a
compilation error, you can be assured it is safe and you can defer this.

## Strategy 2: Verification for local use

## Strategy 3: Enforce the library contract

