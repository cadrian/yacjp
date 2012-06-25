# Introduction

## Why yet another library

The incentive is always the same: existing ones don't suit me. I found
some flaws in the better known cJSON library:

 * no way to directly read a file; one must first put it fully in a
   string, then call the parser, thus maybe consuming a lot of memory
 * a one-size-fits-all structure, with fields for all types of JSON
   values (mixed fields include next/previous links, children,
   strings, and numbers)
 * no way to output JSON

To fix those flaws, YacJP provides the following features:

 * JSON reading from many data streams (provided input streams include
   strings, files, and file descriptors)
 * JSON writing to data streams, with options such as compact
   vs. pretty output
 * an object-oriented approach, with well separated data structures,
   and actual encapsulation
 * AFAICS a better number support


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


\defgroup json_stream JSON streams

The library proposes several implementations of input and output
streams.


\defgroup json_value JSON values

The library implements all the JSON value types: objects, arrays,
strings, numbers, and the three constants `true`, `false`, and `null`.

\defgroup json_parse Parsing from a JSON stream

The library provides a function that drives the JSON parsing, while
using callbacks for both memory management and error management.

The parsing function uses the \ref json_stream "JSON input streams" as
byte providers.


\defgroup json_write Writing to a JSON stream

Writing to a \ref json_stream "JSON output stream" is only a matter of
using a writer object.


\defgroup json_utils JSON tools

The library also provides a fez tools, oriented to the management of
the JSON values.

Currently there is only one such tool: the JSON values killer that
correctly (recursively) frees all the memory allocated by JSON values.
