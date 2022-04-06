# Core RLBox API

In this section we describe a large part of the RLBox API you are likely to
encounter when porting libraries. The API has some more advanced features and
types that are necessary but not as commonly used (see [Doxygen]_).  In most
cases the RLBox type system will give you an informative error if and how to
use these features.

Creating (and destroying) sandboxes
-----------------------------------

RLBox encapsulates sandboxes with `rlbox_sandbox <rlbox_sandbox>` class.
For now, RLBox supports two sandboxes: a Wasm-based sandboxed and the *null*
sandbox. The null sandbox doesn't actually enforce any isolation, but is very
useful for migrating an existing codebase to use the RLBox API.  In fact, in
most cases you want to port the existing code to use RLBox when interfacing
with a particular library and only then switch over to the Wasm-based sandbox. 

.. _rlbox_sandbox:
.. doxygenclass:: rlbox::rlbox_sandbox

.. doxygenclass:: rlbox::rlbox_noop_sandbox

.. _create_sandbox:
.. doxygenfunction:: create_sandbox

Creating sandboxes is mostly straightforward.  For the null sandbox, however,
you need to add a ``#define`` at the top of your entry file, before you include
the RLBox headers::

  #define RLBOX_USE_STATIC_CALLS() rlbox_noop_sandbox_lookup_symbol
  ...
  rlbox::rlbox_sandbox<rlbox_noop_sandbox> sandbox;
  sandbox.create_sandbox();

.. _destroy_sandbox:
.. doxygenfunction:: destroy_sandbox

It's important to destroy a sandbox after you are done with it. This ensures
that the memory footprint of sandboxing remains low. Once you destroy a sandbox
though, it is an error to use the sandbox object.

Calling sandboxed library functions
-----------------------------------

RLBox disallows code from calling sandboxed library functions directly.
Instead, application code must use the `invoke_sandbox_function() <invoke_sandbox_function>`
method.

.. _invoke_sandbox_function:
.. doxygendefine:: invoke_sandbox_function

Though this function is defined via macros, RLBox uses some template and macro
magic to make this look like a `sandbox <rlbox_sandbox>` method. So, in
general, you can call sandboxed library functions as::

  // call foo(4)
  auto result = sandbox.invoke_sandbox_function(foo, 4);

Exposing functions to sandboxed code
------------------------------------

Application code can expose `callback functions <callback>` to sandbox via
`register_callback() <register_callback>`.  These functions can be called
by the sandboxed code until they (1) are no longer in scope or (2) are
explicitly unregistered via the `unregister()` function.

.. _register_callback:

The type signatures of `register_callback() <register_callback>`
function is a bit daunting. In short, the function takes a `callback
function <callback>` and returns a function pointer that can be passed to the
sandbox (e.g., via `invoke_sandbox_function() <invoke_sandbox_function>`).

.. _callback:

A *callback function* is a function that has a special type:

* The first argument of the function must be a reference a `sandbox
  <rlbox_sandbox>` object.
* The remaining arguments must be `tainted <tainted>`.
* The return value must be `tainted <tainted>` or ``void``. This ensures
  that the application cannot accidentally leak data to the sandbox.

Forcing arguments to be `tainted <tainted>` forces the application to
handled values coming from the sandbox with care. Dually, the return type
ensures that the application cannot accidentally leak data to the sandbox.

Tainted values
--------------

Values that originate in the sandbox are *tainted*. We use a special tainted
type `tainted <tainted>` to encapsulate such values and prevent the
application from using tainted values unsafely.

.. _tainted:
.. doxygenclass:: rlbox::tainted

RLBox has several kinds of tainted values, beyond `tainted <tainted>`.
Thse, however, are slightly less pervasive in the surface API.

.. _tainted_volatile:
.. doxygenclass:: rlbox::tainted_volatile

.. _tainted_boolean_hint:
.. doxygenclass:: rlbox::tainted_boolean_hint

.. _verification:

Unwrapping tainted values
^^^^^^^^^^^^^^^^^^^^^^^^^

To use tainted values, the application can copy
the value to application memory, verify the value, and unwrap it. RLBox
provides several functions to do this.

.. doxygenfunction:: copy_and_verify

For a given tainted type, the verifier should have the following signature:

+------------------------+---------------+----------------------------------+
| Tainted type kind      |  Example type | Example verifier                 |
+========================+===============+==================================+
| Simple type            |  ``int``      | ``T_Ret(*)(int)``                |
+------------------------+---------------+----------------------------------+
| Pointer to simple type |  ``int*``     | ``T_Ret(*)(unique_ptr<int>)``    |
+------------------------+---------------+----------------------------------+
| Pointer to class type  |  ``Foo*``     | ``T_Ret(*)(unique_ptr<Foo>)``    |
+------------------------+---------------+----------------------------------+
| Pointer to array       |  ``int[4]``   | ``T_Ret(*)(std::array<int, 4>)`` |
+------------------------+---------------+----------------------------------+
| Class type             |  ``Foo``      | ``T_Ret(*)(tainted<Foo>)``       |
+------------------------+---------------+----------------------------------+

In general, the return type of the verifier ``T_Ret`` is not constrained and can
be anything the caller chooses.

.. doxygenfunction:: copy_and_verify_range
.. _copy_and_verify_string:
.. doxygenfunction:: copy_and_verify_string
.. doxygenfunction:: copy_and_verify_address

In some cases it's useful to unwrap tainted values without verification.
Sometimes this is safe to do and RLBox provides a method for doing so
called ``unverified_safe_because``

Since pointers are special (sandbox code may modify the data the pointer
points to), we have a similar function for pointers called
unverified_safe_pointer_because. This API requires specifying the number of
elements being pointed to for safety.

We however provide additional functions that are especially useful during
migration:

.. _UNSAFE_unverified:
.. doxygenclass:: rlbox::tainted_base_impl
   :members: UNSAFE_unverified, UNSAFE_sandboxed

These functions are also available for `callback <callback>`

.. danger::  Unchecked unwrapped tainted values can be abused by a compromised
   or malicious library to potentially compromise the application.

.. _tainted_ops:


Operating on tainted values
^^^^^^^^^^^^^^^^^^^^^^^^^^^
Unwrapping tainted values requires care -- getting a verifier wrong could lead
to a security vulnerability. It's also not cheap: we need to copy data to the
application memory to ensure that the sandboxed code cannot modify the data
we're tyring to verify. Lucikly, it's not always necessary to copy and verify:
sometimes we can compute on tainted values directly. To this end, RLBox defines
different kinds of operators on tainted values, which produce tainted values.
This allows you to perform some computations on tainted values, pass the values
back into the sandbox, and only later unwrap a tainted value when you need to.
operators like ``+`` and ``-`` on tainted values.

+-------------------------+------------------------------------------------+
|  Class of operator      |  Supported operators                           |
+=========================+================================================+
|  Arithmetic operators   |  ``=``, ``+``, ``-``, ``*``, ``/``, ``%``,     |
|                         |  ``++``, ``--``                                |
+-------------------------+------------------------------------------------+
|  Relational operators   |  ``==``, ``!=``, ``<``, ``<=``, ``>``, ``>=``  |
+-------------------------+------------------------------------------------+
|    Logical operators    |  ``!``, ``&&`` (limited), ``||`` (limited)     |
+-------------------------+------------------------------------------------+
|    Bitwise operators    |  ``~``, ``&``, ``|``, ``^``, ``<<``, ``>>``    |
+-------------------------+------------------------------------------------+
|    Compound operators   |  ``+=``, ``-=``, ``*=``, ``/=``, ``%=``,       |
|                         |  ``&=``, ``|=``, ``^=``, ``<<=``, ``>>=``      |
+-------------------------+------------------------------------------------+
|   Pointer operators     |  ``[]``, ``*``, ``&``, ``->``                  |
+-------------------------+------------------------------------------------+

When applying a binary operator like ``<<`` to a tainted value and an untainted
values the result is always tainted.

RLBox also defines several comparison operators on tainted values that sometime
unwrap the result:

* Operators ``==``, ``!=`` on tainted pointers is allowed if the rhs is
  ``nullptr_t`` and return unwrapped ``bool``.

* Operator ``!`` on tainted pointers retruns an unwrapped ``bool``.

* Operators ``==``, ``!=``, ``!`` on non-pointer tainted values return a
  ``tainted<bool>``

* Operators ``==``, ``!=``, ``!`` on `tainted_volatile <tainted_volatile>`
  values returns a `tainted_boolean_hint <tainted_boolean_hint>`

* Operators ``&&`` and ``||`` on booleans are only permitted when arguments are
  variables (not expressions). This is because C++ does not permit safe
  overloading of && and || operations with expression arguments as this affects
  the short circuiting behaviour of these operations.

Application-sandbox shared memory
---------------------------------

Since sandboxed code cannot access application memory, to share objects across
the boundary you need to explicitly allocate memory that both the application
and sandbox can access. To this end, `malloc_in_sandbox()
<malloc_in_sandbox>` allocates memory within the sandbox region and returns a
`tainted <tainted>` pointer that can be used by both the application and
sandbox (e.g., by passing the pointer as an argument to a function).

.. _malloc_in_sandbox:
.. doxygenfunction:: malloc_in_sandbox()
.. doxygenfunction:: malloc_in_sandbox(uint32_t)

Manually allocated memory is freed explicitly:

.. doxygenfunction:: free_in_sandbox(tainted<T*, T_Sbx> ptr)

Migrating code by temporarily removing tainting
-----------------------------------------------

RLBox is designed to simplify working with existing applications/code bases.
Rather than migrating an application to use RLBox's APIs in a single shot, RLBox
allows "incremental migration" to simplify this step. In particular, migrating
existing code to use RLBox APIs i.e. using `tainted` types and replacing
function calls to libraries with `invoke_sandbox_function()
<invoke_sandbox_function>` method, can be performed one line at a time. After
each such migration, you can continue to build, run/test the program with full
functionality to make sure the migration step is correct.

For example consider migrating some existing code that uses ``mylib``::

   // app.c:

   void print_error_message() {
      char* msgs[] = { "Success", "Fail" };
      int result = get_error_code();
      printf("Result: %s\n", msgs[result]);
      ...
   }


Rather than migrating the full function to use RLBox, you can migrate just the
call to ``get_error_code`` by leveraging the `UNSAFE_unverified
<UNSAFE_unverified>` APIs to removing the tainting::

   // app.c:

   void print_error_message() {
      char* msgs[] = { "Success", "Fail" };

      // Migrate this line to use the RLBox API
      tainted<int, rlbox_noop_sandbox> tainted_result = get_error_code();
      int result = tainted_result.UNSAFE_unverified();

      // Not migrated
      printf("Result: %s\n", msgs[result]);
      ...
   }

Observe that by renaming variables, we can easily leave the other lines of code unmodified.
After building and testing this step, we can now migrate the line that prints the message::

   // app.c:

   void print_error_message() {
      char* msgs[] = { "Success", "Fail" };

      // Migrate this line to use the RLBox API
      auto tainted_result = get_error_code();

      // Not migrated
      printf("Result: %s\n", msgs[tainted_result.UNSAFE_unverified()]);

      int result = tainted_result.UNSAFE_unverified();
      ...
   }

Similarly, we can proceed with replacing all uses of result in the ``...`` portion of code as well.

As the "UNSAFE" portion of the ``UNSAFE_unverified`` API indicates, migration is
**NOT** complete until we replace the uses of ``UNSAFE_unverified`` with
``copy_and_verify`` or ``unverified_safe_because`` as discussed in
`verification`. Here, the since ``tainted_result`` is being used to access
an array of size 2, we can verify it as shown below::

   printf("Result: %s\n", msgs[tainted_result.copy_and_verify([](int val){
      if (val < 0 or val >= 2) { abort(); }
      return val;
   }));

The exact opposite of the ``UNSAFE_unverified`` function i.e. converting
regularly "untainted" data to ``tainted`` is also available:

+------------------------+---------------+----------------------------------------+----------------------------------------------+
| Tainted type kind      |  Example type | Converted type                         | Conversion API                               |
+========================+===============+========================================+==============================================+
| Simple type            |  ``int``      | ``tainted<int, rlbox_noop_sandbox>``   | Automatic conversion. No code change needed. |
+------------------------+---------------+----------------------------------------+----------------------------------------------+
| Pointer to type        |  ``Foo*``     | ``tainted<Foo*, rlbox_noop_sandbox>``  | ``sandbox.UNSAFE_accept_pointer(ptr)``       |
+------------------------+---------------+----------------------------------------+----------------------------------------------+


.. _plugins:

