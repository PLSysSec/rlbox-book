# Branching on tainted values

Performing comparison operations on tainted values will produce three different
types depending on the context:

1. The most intuitive case is when you compare a tainted pointer with
   `nullptr_t`. This is permitted by RLBox with no change, and the comparison
   will produce a bool as expected.

   ```cpp
   tainted<int*> p = ...;
   bool b = (p == nullptr);
   ```

   This also means you can write branches for this case as you normally would.

   ```cpp
   tainted<int*> p = ...;
   if (p == nullptr) { ... }
   ```

2. The next case is when you are comparing against a tainted value such as a
   `tainted<int>`. Assuming `ret` is a `tainted<int>`, RLBox returns
  `tainted<bool>` during comparisons like `ret == 4` and this type does not
  allow branching or loops. This is because any scenario where an application's
  control flow is determined by a tainted value is inevitably going to result in
  security issues. You have two options here:

    - **Option 1**: You can try to verify the `tainted<bool>` using
    `copy_and_verify`, but "verifying" a bool doesn't really make sense. A bool
    has two values true or false, and the only way to "verify" it is if you knew
    which value it was supposed to have. So "verifying" the bool only makes
    sense if you are writing code where the value of the bool doesn't really
    matter. For example:

        ```cpp
        tainted<int> ret = ...;
        tainted<bool> b = (ret == 4);

        if (b.copy_and_verify("Worst case, my app exits early.")) {
            exit(1);
        }
        ```
    - **Option 2**: You can try to verify `ret` prior to comparison to avoid
    this issue. Depending on the application there may be more context to verify
    `ret` than the boolean. For example:

        ```cpp
        tainted<int> t_ret = sandbox.invoke_sandbox_function(get_value_between_1_and_10);
        int ret = t_ret.copy_and_verify([&](int val){
            assert(ret >= 0 && ret <= 10);
            return ret;
        });
        if (ret == 4) { ... }
        ```

3. The last case is when you perform a comparison on the result of dereferencing
  a tainted pointer, this would produce a `tainted_bool_hint`. For example:

    ```cpp
    tainted<int*> a = ...;
    tainted_bool_hint b = *(*a == 4)*;
    // The following would not compile
    // tainted<bool> b = *(*a == 4)*;
    ```

    The reason for this is that this pattern introduces a subtle difference from
    case 2. Here, the dereference of a tainted pointer means you are reading data
    from a location inside the sandbox's memory. This is memory that the sandboxed
    code can read and write and thus the data can change at any moment. To
    represent the volatility of this comparison, RLBox returns a new type
    `tainted_bool_hint` with the result of this comparison to indicate that your
    app should really view this as nothing more than a hint of the real compared
    value. Verification of this looks a lot like Option 1 in case 2 above.

    ```cpp
    tainted<int*> a = ...;
    tainted_bool_hint b = *(*a == 4)*;

    if (b.copy_and_verify("Worst case, my app exits early.")) {
        exit(1);
    }
    ```