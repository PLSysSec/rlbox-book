# Casting tainted values

Ordinary C++ casts will not work on RLBox's tainted types. RLBox instead
provides equivalent casts that change the wrapped type while keeping the result
tainted and associated with the same sandbox type.

## Static casts

Use `sandbox_static_cast` when the equivalent C++ `static_cast` is valid. For
example, a scoped enum can be converted to its underlying integer type before
being passed to a sandbox function that expects an integer.

```cpp
enum class number : unsigned int { seven = 7 };
tainted_mylib<number> value = ...;

tainted_mylib<unsigned int> converted =
  rlbox::sandbox_static_cast<unsigned int>(value);
```

## Reinterpret casts

Use `sandbox_reinterpret_cast` to reinterpret one tainted pointer type as
another. Both the source and destination types must be pointers.

```cpp
tainted_mylib<void*> value = ...;

tainted_mylib<int*> int_pointer =
  rlbox::sandbox_reinterpret_cast<int*>(value);
```

## Const casts

Use `sandbox_const_cast` to change the `const` qualification of a tainted value
when the equivalent C++ `const_cast` is valid.

```cpp
tainted_mylib<const char*> value = ...;

tainted_mylib<char*> mutable_value =
  rlbox::sandbox_const_cast<char*>(value);
```

As with a C++ `const_cast`, removing `const` does not make an object safe to
modify if the underlying object is actually const.
