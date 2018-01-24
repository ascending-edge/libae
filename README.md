# Overview

This library, libae, is utility code that makes writing applications
in C on Linux a dream!

# Idioms

Almost all functions follow the same pattern: 

0.  They return a `boolean` that represents the success or failure of
    the function.
0.  The first parameters is an `ae_res_t*` named `e`.  Error,
    informational, and warning messages are stored in this and can
    easily be logged.
0.  The second paramter is named `self`.  This is analogous to the
    `this` pointer in C++.  It is a pointer to the object instance you
    wish to work with.

This pattern is so prevalent in this library that I do not document
these parameters when they are used in the typical manner (it would be
highly redundant busy work).

# Option Parsing
The option parser in libae makes it easy to have a robust set of
command line arguments.  Help output is automatically generated from
the options table.  Handling common argument types is also provided
while not restricting the user from more complex handling.

## Types

- Group - This is used for grouping common options in the help output.

- Help - This causes the help output to be printed to stdout and the
  program to exit with an exit code of 0.

- Version - This causes the program to output the version of the
  application to stdout and the program to exit with an exit code of
  0.

- Flag - The command line argument is present or not present and sets
  a boolean accordingly.  This is a simplified version of the Bit
  type.

- Int - The command line argument has a integer value.

- Double - The command line argument has a floating point value.

- String - The command line argument has a string value.

- Bit - The command line argument sets a bit in a bit flag.  This
  avoids the need for numerous boolean variables.

- Counter - The command line argument operates a counter.  An example
  would be providing multiple --verbose arguments with each one
  increasing the amount of output a program outputs.


### 

# Example
