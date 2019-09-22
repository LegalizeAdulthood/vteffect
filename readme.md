This repository contains code for generating various
animation effects as VT100 escape sequences.  The code
currently consists of four utilities:
* `scat`, a slow cat program that simulate output at a specific baud rate
* `vtdump`, a utility to dump escape sequences as text commands
* `vtasm`, a utility to assemble text commands into escape sequences
* `vteffect`, a program for generating VT100 animation effects

# scat

Scat is a slow cat program designed to allow you to play
back VT100 animations at a fixed baud rate when you have
a very fast local terminal, e.g. a linux shell on a desktop
machine.

Usage:

`scat` *baud* [*file*]

The baud rate parameter is required and can be any number.
The program computes a delay between characters in order to
simulate the requested baud rate.

The optional file argument consists of the single file that
will be displayed.  If omitted, standard input is printed.

# vtdump

Vtdump is a program that reads a series of escape sequences
and emits a series of commands, one per line, that describe
the escape sequences.

Usage:

`vtdump` [*file*]

The *file* argument is optional and standard input is used
if it is omitted.  The input file is decomposed into commands.
A command is either a control sequence, VT100 escape sequence
or literal text.

A control sequence command is displayed by the mnemonic name
of the control character, e.g. bs for backspace.  Control sequence
commands have no parameters.

An escape sequence command is displayed by the mnemonic of
the escape sequence followed by any optional parameters.  Not
all escape sequences have parameters and some may have default
parameters.  The parameters are separated from the command by
a space and are shown as a quote delimited string.  (The parameter
may contain unescaped quote characters.)

Literal text is displayed a line beginning and ending with a
double quote.  The text between the quotes is what is contained
in the input.  Note that the quotes are simply delimiting
characters and the literal text may contain double quote characters
that are not escaped in any way.

# vtasm

Vtasm is a program that reads a series of text commands, one per line,
and emits the corresponding escape sequences.  The input format is
that output by `vtdump`.  `vtasm` recovers the original escape sequence
input from the output produced by `vtdump`

Usage:

`vtasm` [*file*]

The *file* argument is optional and standard input is used
if it is omitted.  The escape sequence corresponding to the input
command sequence is written to standard output.

# vteffect

Vteffect generates animated effects as VT100 escape sequences.

Usage:

`vteffect` [*options*] *effect*

*options* may be one one or more of the following options:
* `-l` *top*,*bottom* -- specifies the top and bottom lines
* `-dw` *top*,*bottom* -- specifies that lines top through bottom are double wide
* `-nl` *num* -- specifies the number of lines
* `-ss` *file* -- specifies the text file containing the start screen
* `-es` *file* -- specifies the text file containing the end screen
* `-sattr` *attr* -- specifies the start screen character attribute
* `-eattr` *attr* -- specifies the end screen character attribute
* `-wavedir` *dir* -- specifies the wave direction

*effect* can be one of the following effect names:
* `lptest`, a line printer scrolling text test pattern
* `erase_in_line`, erases the screen leaving a sin wave pattern remaining
* `erase_in_display`, erases the screen diagonally from the top left and bottom right
* `wipe_circle_in`, circular wipe inwards towards the center of the screen
* `wipe_circle_out`, circular wipe outwards from the center of the screen
* `wipe_random`, random character wipe
* `canoe`, sprite based canoe animation
* `attribute_wave`, wipe across the screen switching attributes
* `maze`, draw a screen sized maze
* `figlet`, render text via figlet
* `show`, minimally render a screen

You may see additional effects mentioned in the source code as a work
in progress.

Within each effect, an attempt is made to emit the minimal number of
escape sequence characters in order to achieve the effect as quickly
as possible.  Generally this takes place within the cursor positioning
code as the current cursor position is updated.

Not all effects support all command-line options.  Check the code for
details on which options are supported for which effects.
