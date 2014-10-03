---
layout : post
title : Quantum Operators
categories : [programming, ocaml]
---

We typically reperesent states in quantum mechanics as wavefunctions, i.e. functions that map from a vector space V to the complex numbers C. This seems like a great place to use higher order functions.

The functions we are interested in are square-integrable, differentiable functions. We can express these constraints with module types in OCaml.

```ocaml
(** The underlying type needs to have operations between it and Complex.t **)
module type CompatibleWithComplex = sig
	(** This is the type of the domain space **)
	type t
	(** We need to be able to multiply elements of the space by complex numbers **)
	val mul : t -> Complex.t -> Complex.t
	(** We might need addition as well **)
	val add : t -> Complex.t -> Complex.t
end

(** We need a integrator **)
module type Integrable = sig
	(** The underlying space **)
	type t
	(** Functions to Complex.t **)
	type f = t -> Complex.t
	(** Integrate the function over some hardcoded interval **)
	val integrate : f -> Complex.t
end

module type Differentiable = sig
	type t
	type f = t -> Complex.t
	val differentiate : f -> f
end
```

As an example of a type that satisfies these constraints, I'll define a module that represents functions from the integers in [-5,5] to the complex plane.

```ocaml
module IntegralFunction = struct
	type t = int (* The domain of the function *)
	type f = t -> Complex.t

	let integrate f =
		List.fold_left Complex.add Complex.zero
			(List.map f [-5;-4;-3;-2;-1;0;1;2;3;4;5])
	 let differentiate (func:f):f = fun x ->
		Complex.div (Complex.sub (func (x+1)) (func (x-1)))
			{Complex.re=2.;im=0.}
	let mul x y = Complex.mul {Complex.re = (float_of_int x); im = 0.} y
	let add i c = Complex.add {Complex.re = (float_of_int i); im = 0.} c
end
```

We use the constraints as types in the argument to the functor

```ocaml
module Make_braket	(A : CompatibleWithComplex)
			(D : Differentiable with type t = A.t)
			(I : Integrable with type t = A.t) = struct
```

Now comes the interesting bit. We have a type that represents a wavefunction (a 'ket'),
but what type will its dual (bra) be? When we take the inner product of a bra and a ket,
we get a complex number as the result. If we think of taking the inner product as applying
the bra to a ket, it is obvious the bra should be

```ocaml
type bra = ket -> Complex.t
```

and an operator returns a ket when applied to a ket, so

```ocaml
type operator = ket -> ket
```

We can now begin defining useful functions.
The inner product of two functions is the integral of their product :

```ocaml
let inner_product f1 f2 = I.integrate
	(fun x -> Complex.mul (Complex.cong (f1 x)) (f2 x))
```

So the dual of a ket k is a function that takes another ket k' and returns <k|k'> :

```ocaml
let make_bra (k:ket) = (fun k' -> inner_product k k')
```
