# Introduction

## Why yet another library

The incentive is always the same: existing ones don't suit me. I found
some flaws in the better known _cJSON_ library:

 * no way to directly read a file; one must first put it fully in a
   string, then call the parser, thus maybe consuming a lot of memory
 * a one-size-fits-all structure, with fields for all types of JSON
   values (mixed fields include next/previous links, children,
   strings, and numbers)
 * no way to _output_ JSON

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

C does not mean "not object". Actually the whole YacJP library is
built using object-oriented techniques: one may find encapsulation,
polymorphism, and even some design patterns (composites, factories,
visitors).

YacJP is _compact_, but not at the price of extensibility and
legibility.

YacJP is _customizable_: the user may provide specific hooks such as
their own \ref json_memory_t "implementation" of `malloc(3)` and
`free(3)`, a \ref json_on_error_fn "custom parse error handler", and
so on.

YacJP is _extendable_: the user may provide their own implementation
of \ref json_stream "input/output streams", as long as they provide
the right interface they will be used correctly. Indeed, there is
polymorphism in C.

YacJP is _user-friendly_: to implement actions on the JSON values
tree, one just need to provide a \ref json_visitor "visitor" structure
which is a list of functions, one for each data type (objects, arrays,
and so on). Implementing such a structure is straightforward; there
are examples of such implementations in YacJP itself:

 * the \ref json_kill "kill" visitor allows to correctly (recursively)
   free a whole JSON value tree;
 * the \ref json_write_to "write_to" visitor allows to write a JSON
   value tree to an output stream, with a few options such as the
   level of prettiness.

\mainpage YacJP

YacJP is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, version 3 of the License.

YacJP is distributed in the hope that it will be useful, but __without
any warranty__; without even the implied warranty of
__merchantability__ or __fitness for a particular purpose__.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with YacJP.  If not, see http://www.gnu.org/licenses/

Copyleft © 2012 Cyril ADRIAN


\defgroup json_stream Byte streams

The library proposes several implementations of input and output
streams.

\{
\defgroup json_in_stream Input streams

Input streams act as byte cursors: their state is the current byte,
change it by reading yet another byte.
\{
\}

\defgroup json_out_stream Output streams

Output streams act as byte buckets: fill them and flush them.
\{
\}

\defgroup json_unicode_stream Output streams

Specific unicode input streams that provide utf8 data.
\{
\}

\}


\defgroup json_value JSON values

The library implements all the JSON value types: objects, arrays,
strings, numbers, and the three constants `true`, `false`, and `null`.

\{

\defgroup json_object Objects

The JSON objects are in fact associative arrays. Keys are unicode
strings (utf-8 encoded); values are \ref json_value "JSON values".

\defgroup json_array Arrays

The JSON arrays are lists of \ref json_value "JSON values" laid out
one after the other, indexed starting at 0.

\defgroup json_string Strings

The JSON strings are unicode strings.

\defgroup json_number Numbers

The JSON numbers are infinite-precision numbers with some accessors
that get the value as either int or double.

\defgroup json_const Constants

The JSON constants are shared pointers to unique data. There is one
such pointer for each known JSON constant: `false`, `true`, and
`null`.

\}

\defgroup json_parse Parsing from a JSON stream

The library provides a function that drives the JSON parsing, while
using callbacks for both memory management and error management.

The parsing function uses the \ref json_stream "JSON input streams" as
byte providers.

The JSON parser has two not normed extensions:

 * a trailing comma is allowed before the closing '}' or ']' of
   objects and arrays respectively (C-like behaviour)
 * comments are allowed in three forms: C forms (`//...` and
   `/*...*/`) and script forms (`#...`) — a bastard form (`/#...`) is
   also understood as a line comment

\defgroup json_write Writing to an output stream

Writing to a \ref json_stream "JSON output stream" is only a matter of
using a writer object.


\defgroup json_utils Tools

The library also provides a few tools, oriented to the management of
the JSON values.

The first tool is the JSON values killer that correctly (recursively)
frees all the memory allocated by JSON values.

The second tool is used virtually everywhere: the memory manager. The
library uses a pair of functions similar to `malloc(3)` and `free(3)`
to handle memory chunks allocation and deallocation. Those functions
are available through a structure `json_memory_t` passed to most YacJP
functions. The library provides a standard ("stdlib") \ref
stdlib_memory "memory manager" but the user is free to provide her
own.

The third tool is a lookup function.


\defgroup json_hash Hash tables

The library provides a general-purpose hash table. It is internally
used by \ref json_object "JSON objects" but it may be used anywhere
associative tables are needed.
