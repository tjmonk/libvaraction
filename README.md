# libvaraction
VarServer Actions Library for building actions parse trees

## Overview

The VarAction library is a support library for script execution
involving varserver variables.  Many standard operations are
supported between VarServer variables.  These include:

- assignment operators
- bitwise operators
- boolean operators
- comparison operators
- math operators
- string operators
- typecasting operators

This library also supports some timer operations

The variable actions are contained in nodes, and functional
chains of operations are expected to be compiled into a binary tree
of operations.

For example the assignment operator would be a parent node.  Its left child
represents the destination variable, and its right node represents the
value (or result of some other operation) being assigned.

The assignment parent node, may then be a child of a higher level
operation.

## Prerequisites

The varaction library is a support library for the varserver.
It requires definitions from:

- varserver : variable server ( https://github.com/tjmonk/varserver )

## Limitations:

The varaction library currently only supports the following varserver
data types:

- string
- float
- uint32
- uint16

## Build

```
$ ./build.sh
```

