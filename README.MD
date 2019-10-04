# Type Safe Smart Pointer - Proof of Concept  
  
This project is a small proof of concept of a more type safe smart pointer that solves the template covariance problem through the use of template meta programming. Many type-level programming constructs now available in the STL are reimplemented in `meta.hpp` via SFINAE. 

## What is the template covariance problem?

The short explanation is that for all template types `F`, `F<A>` is never a subtype of `F<B>`. But there are some template types `F`, where we would like this to be the case. For example, it would make sense if for all `A`, `B` such that `B` is a subtype of `A` , `std::shared_ptr<B>` is also a subtype of `std::shared_ptr<A>`, so that we can make use of subtype polymorphism like with regular old  pointers.

This is achieved by adding an implicit conversion function or copy constructor `my_shared_ptr<A>(my_shared_ptr<B>)` that is only enabled when `B <: A`.   

## Excursion: What is variance really? 

The term "covariance" refers to a specific property of a functor, which is a categorical homomorphism (not the C++ "functor" which is a misnomer). 

A category `C` is a mathematical object consisting of a class of objects and a set of morphisms between objects, such that every object `a` has an identity morphism `id : a ~> a` and for every two morphisms `f : a ~> b`, `g : b ~> c` there exists a composition `g . f : a ~> c`. The important thing to understand about categories is that they are all about composition; We know nothing about the objects, in fact they only exist to serve as the end points of morphisms.  

Programming is all about composition of abstractions, so it turns out that category theory is sometimes useful to model patterns arising in programming. We can think of a category consisting of types and the functions between them, a category of values and their ordering relation, or a category of types and their subtype relation. It is the latter that we are interested in today. 

A functor is a structure preserving mapping between two categories (possible the same one). It maps every object from one category to an object of the other and every morphism from one category to a morphism of the other. The mapping has to be structure preserving, meaning the functor has to preserve identity and composition of morphisms. There are two ways to make this happen:

- ### The covariant functor
	For all morphisms `f`, `g`:  `F(g . f) = F(g) . F(f)`.
	The direction of composition stays the same. Types in programming that have a covariant functor are usually some kind of "producer", that we can take values out of, for example the type family `List(a) : Type -> Type` or the family of functions that enumerate the values of a type `(Int -> a) : Type -> Type`. Such types admit a `map` function to change the produced value: `map : (a -> b) -> F(a) -> F(b)`. 
	
	How this relates to subtyping: A `Factory` producing `Audi`s can be used in any place expecting a `Factory` that produces any `Car`s. `Audi` is a subtype of `Car` and `Factory(Audi)` is a subtype of `Factory(Car)`. The subtype relation is the same, thus `Factory` must have a covariant functor. 

- ### The contravariant functor
	For all morphisms `f`, `g`:  `F(g . f) = F(f) . F(g)`.
	The direction of composition is reversed. Types in programming that have a contravariant functor are usually some kind of "consumer", that we can put values into, for example the family of functions that indexes values of a type: `(a -> Int) : Type -> Type`.  Such types admit a `contramap` function to change the consumed value: `contramap : (a -> b) -> F(b) -> F(a)`. 

	Analogously, a `Customer` buying any `Car` can be used in any place that expects a `Customer` willing to buy a `BMW`.  `Customer(Car)` is a subtype of `Customer(BWM)` even though `BMW` is a subtype of `Car`. The subtype relation is reversed, thus `Customer` must have a contravariant functor.


