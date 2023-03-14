# scripty - Frogger 2 Script Compiler
This is a recreation of the official Frogger 2 script compiler named 'etc'.  
Because 'etc' has not been found yet, we need some method for compiling and decompiling scripts to build the game.  
Even if the original 'etc' compiler is found, this compiler will likely be used anyways.
This is because it is likely more flexible, less buggy, uses modern syntax, and contains a decompiler.  

## How to use?
**Compile a script:** `scripty.exe compile <input_file> <output_file>`  
**Decompile a script:** `scripty.exe compile <input_file> <output_file> [macro_defs...]`  
**List supported functions:** `scripty.exe info`  

## How can I write scripts?
The easiest way to get started is by playing around with some of the [scripts from the retail build](../../../../assets/scripts).  
Some of the syntax may seem a little bit strange, especially with `AND(condition, condition)` instead of the more common `condition && condition`, but it's a fairly simple language.  
The best resource for answering questions about how scripting works is the game's [script.c](../../../../src/common/script.c) file.  

### Data Types
The data types in the scripting language are very simple, and there are no variables (excluding flags/counters).  

The core data types are as follows:  
**STRING:** A length-based string where the length is stored as a single byte. Individual strings may have different limits/buffer sizes.  
**BYTE:** An unsigned 8 bit integer.  
**SHORT:** An unsigned 16 bit integer.  
**INTEGER:** An unsigned 32 bit integer.  
**DECIMAL:** Decimal numbers are stored as 16.16 fixed point numbers, and the integer portion is unsigned.  

### Triggers
Triggers have two main modes, `TRIGGER_ONCE` and `TRIGGER_START`.  
`TRIGGER_ONCE` indicates that once the trigger fires, it will be unregistered, and cannot fire again, unless it is registered again.  
`TRIGGER_START` indicates that once a trigger fires, it is unable to fire again until its condition stops getting met. Once this occurs, it can be fired again.  

## Design Choices
 - This has been made in pure C instead of something like C# in order to avoid having external dependencies (eg: CLR or JVM) to build Frogger 2.  
 
### Differences from original compiler
The original compiler used a slightly different syntax from scripty, and thus the source files are not intercompatible.
However, conversion between the two should be easy to do by hand, since the changes are minimal.

The first main difference is the preprocessor syntax, here is an overview of the differences.  
```
etc:
%include "{file_name}" - Includes the contents of another file.
set {macro_name} {macro_token} - It is unknown if macro token had to be a number, or could be more than a single token.
In order to evaluate a macro the %{macro_name} would be used, so for example: "%death_tile".
I do not believe numbers were supported in identifiers, such as macro names.
The ";" character would mark the rest of the line as commented out.
It is unknown if numbers can be written in hexadecimal form.

scripty:
#include "{file_name}" - Includes the contents of another file.
#define {macro_name} {macro_tokens} - Defines a macro.
In order to evaluate a macro, the name of the macro can be typed out.
The "//" character sequence marks the rest of the line as commented out.
Numbers can be written in hexadecimal notation (0xFFFF)
```

The other main difference is that certain function names in the original compiler are different.
Some functions we don't know the original names for, others were poorly named, so we chose to rename them.  
