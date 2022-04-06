# Standard library

RLBox provides several helper functions to application for handling tainted data
(memory influenced by or located in sandoxed regions) similar to the C/C++
standard library. We list a few of these below; they operate similar to the
standard library equivalents, but they accept tainted data.

.. doxygenfunction:: memset

|

.. doxygenfunction:: memcpy

|

.. doxygenfunction:: sandbox_reinterpret_cast

|

.. doxygenfunction:: sandbox_const_cast

|

.. doxygenfunction:: sandbox_static_cast

RLBox also provides helper functions for allocating memory in the sandbox (resp
application) and transferring it from the application (resp sandbox) with:

.. doxygenfunction:: copy_memory_or_grant_access

|

.. doxygenfunction:: copy_memory_or_deny_access


These functions copy when using a sandbox like Wasm. For the nop-sandbox, they
don't copy and only move pointers, though.


Handling more complex ABIs
==========================

Passing structs to/from a sandbox 
---------------------------------

RLBox currently doesn't automatically let you use fields of structs.  To use
fields of a tainted struct you need to essentially tell RLBox about the struct
type and layout. You typically do this in a separate header file and include
this file in your application code as follows::

   // app code:
   
   #include "lib_struct_file.h"
   rlbox_load_structs_from_library(mylib); 

The first line includes the header file (we will populate shortly).  The second
line loads the struct definitions using the library name ``mylib``.

Let's suppose our library has two structs::

   // mylib.h:

   struct Inner {
      int val;
   };
   
   struct Foo {
      unsigned char[5] status_array;
      Inner internal;
      unsigned int width;
   };

In ``_lib_struct_file.h`` you will then provide a definition for ``Foo`` alone
would be as follows::

   ...
   #define sandbox_fields_reflection_mylib_class_Foo(f, g, ...)         \
     f(unsigned char[5], status_array, FIELD_NORMAL, ##__VA_ARGS__) g() \
     f(Inner, internal, FIELD_NORMAL, ##__VA_ARGS__) g()                \
     f(unsigned int, width, FIELD_NORMAL, ##__VA_ARGS__) g()
     
   #define sandbox_fields_reflection_mylib_allClasses(f, ...)  \
     f(Foo, mylib, ##__VA_ARGS__)    
   ...

Since ``Foo`` has an ``Inner`` struct, though, we'll need to also provide a
definition for this struct if we want to access the ``val`` value::

   ...
   #define sandbox_fields_reflection_mylib_class_Inner(f, g, ...)    \
     f(int, val, FIELD_NORMAL, ##__VA_ARGS__) g()        

   #define sandbox_fields_reflection_mylib_class_Foo(f, g, ...)         \
     f(unsigned char[5], status_array, FIELD_NORMAL, ##__VA_ARGS__) g() \
     f(Inner, internal, FIELD_NORMAL, ##__VA_ARGS__) g()                \
     f(unsigned int, width, FIELD_NORMAL, ##__VA_ARGS__) g()

   #define sandbox_fields_reflection_mylib_allClasses(f, ...)  \
     f(Inner, mylib, ##__VA_ARGS__)                            \
     f(Foo, mylib, ##__VA_ARGS__)                           
   ...

Each struct file is intended to hold all struct definitions associated with 
a library.

.. danger::  The compiler currently doesn't catch type mismatches, missing
   members, or incorrectly ordered members in the struct definition.

Take a look at the `ogg struct layout in Firefox
<https://searchfox.org/mozilla-central/source/media/libogg/geckoextra/include/OggStructsForRLBox.h>`_
for a complete example.

In the future we will likely generate these kinds of files automatically.

Invoking varargs functions
---------------------------------------

RLBox does not (yet) support calling functions with variable arguments
currently. So, if the library you are sandboxing has APIs with variable
arguments you need to monomorphize the usages. Specifically, you need to create
wrapper functions for each usage you wish to expose.  Consider the following
example::

   // Original library function:
   int example_call(int x, int y, ...);
   
   ...

   // Original invocations in application code:
   rv = example_call(x, y, RESET_FLAG);
   rv = example_call(x, y, INVERT_FLAG, 'c');


One way to handle this in RLBox is to expose two functions as follows::

   int example_call_reset(int x, int y, uint8_t flag) {
      return example_call(x, y, flag);
   }

   int example_call_invert(int x, int y, uint8_t flag, char c) {
      return example_call(x, y, flag, c);
   }

Then, you can call them as usual::

   rv = sandbox.invoke_sandbox_function(example_call_reset, x, y, RESET_FLAG);
   rv = sandbox.invoke_sandbox_function(example_call_invert, x, y, INVERT_FLAG, 'c');

Invoking C++ functions in a sandbox
------------------------------------------

RLBox does not currently let you invoke C++ library functions directly. Instead
you need to expose a C ABI. Functions are largely straightforward. Methods
require a bit of work since the receiver object is implicit: The simplest way
to do this for class methods is to expose C functions and just pass a pointer
to the receiver object as the first argument.

Passing application pointers into the sandbox with `app_pointer`
------------------------------------------------------------------------------------------

It's sometimes useful to pass application pointers into the sandbox. For
example, you may need to pass a pointer to the receiver (``this``) so sandbox
library can pass this pointer back in a callback. We can't trust the sandbox with
actual applications pointers. RLBox instead provides a level of indirection.

If you want to pass a pointer into the sandbox you can use the
`get_app_pointer() <get_app_pointer>` API, which returns an `app_pointer`. For
example, in the expat library we need to pass a pointer to the receiver::

  mAppPtr = mSandbox->get_app_pointer(static_cast<void*>(this));
  // convert the app_pointer to tainted
  tainted_expat<void*> t_driver = mAppPtr.to_tainted();
  // call function as usual:
  mSandbox->invoke_sandbox_function(..., t_driver);

where ``mAppPtr`` is a member of the class::

   app_pointer_expat<void*> mAppPtr;

Internally, RLBox keeps a map between app pointers and the corresponding
tainted pointers exposed to the sandbox. This lets you lookup the pointers in
callbacks, for example::

  void callback(rlbox_sandbox_expat& aSandbox, ... tainted_expat<void*> t_driver) {
    nsExpatDriver* self = aSandbox.lookup_app_ptr(rlbox::sandbox_static_cast<nsExpatDriver*>(t_driver));

Like callbacks, you need to keep app pointers alive and unregister them when
you're done::

  mAppPtr.unregister();


