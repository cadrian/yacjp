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
