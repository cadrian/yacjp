[![Build Status](https://travis-ci.org/cadrian/yacjp.svg?branch=master)](https://travis-ci.org/cadrian/yacjp)

# General description

Yet Another C JSON Parser.

## Why yet another library

The incentive is always the same: existing ones don't suit me. I found
some flaws in the better known _cJSON_ library:

 * no way to directly read a file; one must first put it fully in a
   string, then call the parser, thus maybe consuming a lot of memory
 * a one-size-fits-all structure, with fields for all types of JSON
   values (mixed fields include next/previous links, children,
   strings, and numbers)
 * arrays and objects are implemented using linked objects, may
   cause performance problems

To fix those flaws, YacJP provides the following features:

 * JSON reading from many data streams (provided input streams include
   strings, files, and file descriptors)
 * JSON writing to data streams, with options such as compact
   vs. pretty output
 * an object-oriented approach, with well separated data structures,
   and actual encapsulation
 * AFAICS a better number support

On the other hand cJSON is small; YacJP is bigger (approx. 4000 lines,
not counting tests; see the Statistics chapter).

## The YacJP philosophy

C does not mean ``not object''. Actually the whole YacJP library is
built using object-oriented techniques: one may find encapsulation,
polymorphism, and even some design patterns (composites, factories,
visitors).

YacJP is _compact_, but not at the price of extensibility and
legibility.

YacJP is _customizable_: the user may provide specific hooks such as
their own implementation of `malloc(3)` and `free(3)`, a \ref
json_on_error_fn "custom parse error handler", and so on.

YacJP is _extendable_: the user may provide their own implementation
of \ref json_stream "input/output streams", as long as they provide
the right interface they will be used correctly. Indeed, there is
polymorphism in C.

YacJP is _user-friendly_: to implement actions on the JSON values
tree, one just need to provide a \ref json_visitor "visitor" structure
which is a list of functions, one for each data type (objects, arrays,
and so on). Implementing such a structure is straightforward. There
are examples of such implementations in YacJP itself:

 * the \ref json_kill "kill" visitor allows to correctly (recursively)
   free a whole JSON value tree;
 * the \ref json_write_to "write_to" visitor allows to write a JSON
   value tree to an output stream, with a few options such as the
   level of prettiness.
