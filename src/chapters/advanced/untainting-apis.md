# Untainting different types

As you sandbox more library APIs, you will soon have to start verifying
(removing the tainting) objects of different types. Below, we'll go through the
APIs used to remove tainting in different scenarios.

In all examples, there are a few things to keep in mind:

1. We will use a comment `<insert security checks here>` to refer that you need
to apply domain specific checking to ensure that you only permit values that do
not cause memory safety problems in the rest of your program. The
[tutorial](/chapters/tutorial/noop-sandbox/untaint.md) goes through an example
of adding security checks in more detail.

2. Note that all verifier can return values of any type or no value at all. This
gives some flexibility to how you want to manage your data. So for example, you
may be untainting an `int`, but can return an `unsigned int` or an `int*`, or
`void` etc.


## Tainted primitive/fundamental types

C/C++ defines the fundamental types as the types [built-in to the
language](https://en.cppreference.com/w/cpp/language/types.html), such as `int`,
`float` etc.

These types can be untainted with a verifier of the following form

```cpp
  tainted<int> a = ... ; //
  int a_verified = a.copy_and_verify([](int val){
    // <insert security checks here>
    return val;
  });
```

## Tainted byte buffers

Sometimes the sandbox returns a buffer that you want to untaint. For example,
you may use a sandboxed XML parse to parse jpeg images. In this example, after
the sandboxed library parses th image, it will produce a byte buffer with image
pixels, probably of the type `tainted<char*>` or some other tainted pointer to a
[fundamental type](https://en.cppreference.com/w/cpp/language/types.html).

These types can be untainted with a verifier of the following form

```cpp
  tainted<int*> a = ... ; //
  std::unique_ptr<int*> a_verified = a.copy_and_verify_range([&](std::unique_ptr<int*> val){
    // <insert security checks here>
    return val;
  }, size);
```

This API copies `size` bytes out of the sandbox and applies the necessary checks
like ensuring the entire buffer is coming from the sandbox memory.

In the example of a sandboxed image decoder, the buffer may hold completely
random data instead of pixels of the image. It is up to you to figure out what
your application context is and what security checks need to be in place. In the
example of a sandboxed image decoder, the usual expectation is that there is no
sensible way to check that decoding has occurred correctly, and rather the rest
of your program should be robust to showing an incorrect image on the screen. In
this case, there would be no security check in place.

## Tainted C-strings

Untainting C-strings of type `tainted<char*>` is covered in the
[tutorial](/chapters/tutorial/noop-sandbox/strings.md).

These types can be untainted with a verifier of the following form

```cpp
  tainted<char*> str = ...;
  std::unique_ptr<char[]> checked_string =
    str.copy_and_verify_string([](std::unique_ptr<char[]> val) {
        // <insert security checks here>
        return move(val);
    });
```
The API ensures that the tainted string lives within the sandbox and is null
terminated, and makes a copy of the string that you can use.

A useful check in `<insert security checks here>` is also to limit the size of
the string you want to allow.

## Tainted one-level pointer to a primitive/fundamental type

C/C++ defines the fundamental types as the types [built-in to the
language](https://en.cppreference.com/w/cpp/language/types.html), such as `int`,
`float` etc.

If you have a tainted pointer to a fundamental type such as `tainted<int*>`,
`tainted<float*>` etc., these types can be untainted with a verifier of the
following form

```cpp
  tainted<int*> a = ... ; //
  std::unique_ptr<int> a_verified = a.copy_and_verify([](std::unique_ptr<int> val){
    // <insert security checks here>
    return val;
  });
```

The idea here is that RLBox is effectively creating a deep clone of the object
after doing the required checks of ensuring the pointer is in the sandbox. We
would ideally allow this API for more types, but C++ makes it hard to know when
we can reasonably perform a deep clone of an object, and hence this API is
limited to tainted pointers to fundamental types.

This API is also limited to one-level pointers, i.e., things like
`tainted<int*>` and is not allowed for `tainted<int**>`.
<!-- 
## Tainted array types

## Tainted struct types

## Tainted one-level pointer to a struct types

## Tainted primitive/fundamental types that don't need verification

`unverified_safe_because`

## Tainted pointers that don't need verification

`unverified_safe_pointer_because`

## Untainting just the ``address'' bits of a pointer

`copy_and_verify_address` -->