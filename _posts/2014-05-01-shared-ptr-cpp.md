---
layout : post
title : Shared Pointers
categories : [programming, C++]
---

I was working on some graph code today and discovered a rather unfortunate issue with `shared_ptr`.
The standard says

> § 20.7.2.2: The `shared_ptr` class template stores a pointer, usually obtained via new. `shared_ptr` implements semantics
>   of shared ownership; the last remaining owner of the pointer is responsible for destroying the object, or
>   otherwise releasing the resources associated with the stored pointer. A `shared_ptr` object is empty if it does
>   not own a pointer.

Unfortunately, I was using cppreference.com instead of the standard, so I missed the warning provided by the first sentence in the standard.

In my program, I was constructing several `shared_ptr`s through raw pointers to the same object. I expected that they would share ownership, and the object wouldn’t be destroyed until the last `shared_ptr` was destroyed. I assumed there was some global look-up table allowing `shared_ptr`s to find the control block for the object they point to. In reality, things are not so sophisticated. Each time a `shared_ptr` is constructed from a raw pointer, it is considered to be the sole owner.

> § 20.7.2.2.1.5: Postconditions: `use_count() == 1 && get() == p`

If you construct several `shared_ptr`s to the object, it will be destroyed each time one of these `shared_ptr`s is.
