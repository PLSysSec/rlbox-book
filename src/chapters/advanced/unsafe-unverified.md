# Using `UNSAFE_unverified` during migration

RLBox normally requires application code to verify values received from a
sandbox before removing their `tainted` wrapper. However, when migrating a large
application to RLBox, it can be useful to update one library call at a time and
temporarily leave the surrounding application code unchanged. RLBox provides
`UNSAFE_unverified` for this limited use case.

> **Warning:** `UNSAFE_unverified` disables RLBox's protection for the value it
> returns. The value is still controlled by the sandbox even though its C++ type
> is no longer `tainted`. A migration is not complete while application code
> relies on `UNSAFE_unverified`.

## What does `UNSAFE_unverified` do?

`UNSAFE_unverified` takes no arguments and returns the application-side value
held by the wrapper without applying a verifier or any checks.

```cpp
tainted_mylib<int> tainted_result = ...;
int result = tainted_result.UNSAFE_unverified();
```

Removing the tainting in this way does not make the value trustworthy. For example, an
unchecked integer can still cause an out-of-bounds access when used as an array
index, an incorrect allocation when used as a size, or unsafe control flow when
used in a branch.

## How does this differ from the other untainting APIs?

The right API depends on what the application needs to do with the value.

| API | Behavior | Intended use |
| --- | --- | --- |
| `copy_and_verify` | Passes the value to an application-provided verifier. For supported pointer types, RLBox first copies the pointee into application memory. | The usual way to remove tainting. |
| `unverified_safe_because` | Does not run a verifier, but requires a reason explaining why the application is safe for every possible value. It does not support pointer types. | Non-pointer values whose contents cannot cause a safety problem. |
| `unverified_safe_pointer_because` | Requires an element count and a reason, and checks that the entire pointed-to range is contained in sandbox memory. It does not copy or validate the contents. | One-level data pointers when the application can safely operate directly on sandbox memory. |
| `UNSAFE_unverified` | Does not run a verifier and requires no reason or element count. For pointers, it does not check that a requested range is contained in sandbox memory. | Temporary incremental migration only. |

More details about the safe APIs for different value types are available in
[Untainting different types](/chapters/advanced/untainting-apis.md).

## Using the API during incremental migration

Consider an application that uses a library return value as an index.

```cpp
void print_error_message()
{
    const char* messages[] = { "Success", "Fail" };
    int result = get_error_code();
    printf("Result: %s\n", messages[result]);
}
```

After moving the library call into a sandbox, its return value is tainted. A
temporary migration step could unwrap the result so that the rest of the
function continues to compile.

```cpp
auto tainted_result = sandbox.invoke_sandbox_function(get_error_code);

// Temporary migration step. The sandbox still controls result.
int result = tainted_result.UNSAFE_unverified();
printf("Result: %s\n", messages[result]);
```

This code is not safe yet. A compromised library could return an index outside
the bounds of `messages`. The completed migration verifies the value before it
is used.

```cpp
int result = tainted_result.copy_and_verify([](int value) {
    if (value < 0 || value >= 2) {
        abort();
    }
    return value;
});
printf("Result: %s\n", messages[result]);
```

This pattern lets you migrate and test a large function in smaller steps, but
each temporary use of `UNSAFE_unverified` should be tracked and replaced before
the sandboxing work is considered complete.


## Finishing the migration

Before considering a migration complete:

1. Search the migrated code for every use of `UNSAFE_unverified`.
2. Determine how each returned value is used by the application.
3. Replace each use with `copy_and_verify`, a type-specific verification API,
   or a reason-bearing `unverified_safe_*` API.
4. Review the resulting verifier or safety argument assuming that the sandbox
   may return any value allowed by the underlying C type.

Leaving `UNSAFE_unverified` in place bypasses the trust-boundary checks that
RLBox's tainted types are intended to enforce.
