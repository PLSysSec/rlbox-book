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

3. We will refer to the term fundamental types frequently. C/C++ defines the
fundamental types as the types [built-in to the
language](https://en.cppreference.com/w/cpp/language/types.html), such as `int`,
`float` etc.

## Untainting fundamental types

These types can be untainted with a verifier of the following form

```cpp
  tainted<int> a = ... ; //
  int a_verified = a.copy_and_verify([](int val){
    // <insert security checks here>
    return val;
  });
```

There maybe some scenarios where the application can handle any possible integer
from the sandbox, i.e., the `<insert security checks here>` can be left empty.

- An example of this could be when you are calling an sandboxed library function
that returns an integer 0 on success and a non-zero error code otherwise. From
the host application perspective, you may do something if the returned value is
0, and exit otherwise.


In this scenario, RLBox provides a shorthand API called
`unverified_safe_because` which can be used as follows.

```cpp
  tainted<int> a = ... ; //
  int a_verified = a.unverified_safe_because("Error code. App is robust to all values of error code");
```

The `unverified_safe_because` API takes a string argument that allows the
developer to document why they are doing this and why its safe. This string does
not have any special meaning in the code. Rather the RLBox API asks you to
provide a free-form string that acts as documentation. Essentially you are
providing a string that says _it is safe to remove the tainting from this type
because..._ . Such documentation may be useful to other developers who read your
code. The above is equivalent to:

```cpp
  tainted<int> a = ... ; //
  int a_verified = a.copy_and_verify([](int val){
    // Error code. App is robust to all values of error code
    return val;
  });
```

## Untainting byte buffers

Sometimes the sandbox returns a buffer that you want to untaint. For example,
you may use a sandboxed XML parse to parse jpeg images. In this example, after
the sandboxed library parses th image, it will produce a byte buffer with image
pixels, probably of the type `tainted<char*>` or some other tainted pointer to a
fundamental type.

These types can be untainted with a verifier of the following form

```cpp
  tainted<int*> a = ... ; //
  std::unique_ptr<int*> a_verified = a.copy_and_verify_range([&](std::unique_ptr<int*> val){
    // <insert security checks here>
    return val;
  }, size);
```

This API **copies** `size` bytes out of the sandbox and applies the necessary
checks like ensuring the entire buffer is coming from the sandbox memory.

In the example of a sandboxed image decoder, the buffer may hold completely
random data instead of pixels of the image. It is up to you to figure out what
your application context is and what security checks need to be in place. In the
example of a sandboxed image decoder, the usual expectation is that there is no
sensible way to check that decoding has occurred correctly, and rather the rest
of your program should be robust to showing an incorrect image on the screen. In
this case, there would be no security check in place.


## Untainting byte buffers without copying


There maybe some scenarios where the application can handle any possible byte
buffer from the sandbox, i.e., the `<insert security checks here>` can be left
empty.

- An example of this, would be if the image being displayed to the app in our
  sandboxed libjpeg example is say an application background and may not really
  matter.

In this case, we may want to use the byte before **without making a copy**, to
avoid overheads.

RLBox provides aan API for this called `unverified_safe_pointer_because` which
can be used as follows.

```cpp
tainted<char*> a = ...;
char* raw = a.unverified_safe_pointer_because(10, "Demo of a raw pointer");
```

`unverified_safe_pointer_because` takes two parameters. The first is the number
of bytes in this pointer that you will be accessing. RLBox needs this to ensure
that these many bytes of the pointer stay within the sandbox boundary. The
second is a string, that allows the developer to document why they are doing
this and why its safe. This string does not have any special meaning in the
code. Rather the RLBox API asks you to provide a free-form string that acts as
documentation. Essentially you are providing a string that says _it is safe to
remove the tainting from this type because..._ . Such documentation may be
useful to other developers who read your code.

## Untainting C-strings

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

If you want to convert the above `checked_string` from a
`std::unique_ptr<char[]>` back to `char*`, you can do this like with ever other
`unique_ptr`.

```cpp
char* copied_string = checked_string.release();
// You should call delete copied_string; when you want to release the memory.
```

You can also call `copy_and_verify_string` to output `std::string`.

```cpp
  tainted<char*> str = ...;
  std::string checked_string =
    str.copy_and_verify_string([](std::string val) {
        // <insert security checks here>
        return val;
    });
```

## Untainting one-level pointers to fundamental types

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

## Untainting just the "address bits" of a pointer

Your application may sometimes need just the raw bits of a tainted pointer
without needing to look at the data being pointed to. An example of this would
be if you want to maintain a hashmap of pointers in the class, but the pointers
are produced by the sandbox.

```cpp
tainted<int*> foo = sandbox.invoke_sandbox_function(...);

std::map<int*, int> my_map;
my_map[foo] = 3; // RLBox gives a compiler error
```

For this scenario, RLBox provides an API called `copy_and_verify_address` which
takes a verifier that accepts a `uintptr_t`. This API can be used as follows.

```cpp
tainted<int*> foo = sandbox.invoke_sandbox_function(...);

std::map<int*, int> my_map;
uintptr_t foo_verified = foo.copy_and_verify_address([&](uintptr_t addr) {
    // <insert security checks here>
    return addr;
});
my_map[foo_verified] = 3;
```

## Untainting C-arrays of fundamental types

Untainting C-arrays of fundamental types like `tainted<int[3]>` is possible
using `copy_and_verify`. Note, however, that the API expects the verifier to
accept an argument of `std::array`, so that the data is correctly copied.

These types can be untainted with a verifier of the following form

```cpp
  tainted<int[3]> a = ... ; //
  std::array<int, 3> a_verified = a.copy_and_verify([](std::array<int, 3> val){
    // <insert security checks here>
    return val;
  });
```

<!--

## Tainted struct types

## Tainted one-level pointer to a struct types

-->