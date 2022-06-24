# Coding challenge: Variants on C++ tokenizer

## Purpose
This repo contains the basic setup for a simple C++ coding challenge. The basic
problem description can be found below, in this `README`. The code in
master-branch is the starting-points. Some interesting solutions, with different
takes, can be found in branches.

## Assumptions
The user is assumed to have his/her own setup for programming, including
compiler, editor and more. Only the code, basic documentation and a Makefile
is in this repo. oh, and the minimalistic unit-test code of course.

## The basic problem
We will parse a text, with readable C++ code and as fast as possible.
The input is a random text (Lorem Ipsum), the output is an array of 32-bit hash
values of the words from the text.

The **_hash-algorithm_** will be provided and must be used as is. When timing
the execution, only the time processing the text and generating the list is
counted. Time spent reading the file or writing the output is ignored.

> Maybe also compare with a vanilla C variant?

## Definitions

|Name|Definition|
|:--:|:---|
|`Input`|Either the entire text or a series of fragments of the text. |
|`Output`|A list of hash-values representing the text-input|
|`Tokenizer`|This is the solution, the code that takes `Input` and produce`Output`|
|`Block`|A text, or part there-of, as a pointer to the start and a length.|

## Types of solutions
When using this challenge, one way to make it a little more interesting is to
provide more than one variant. Either every contributor pick one variant or
every one provide two distinct variants.

### The one-pass variant
This `Tokenizer`-variant assumes the entire text is already in-memory, which
means the solution can scan the text in one go. This is probably the simplest
one to implement.

### The block-wize variant
In the `Block`-case, the text is delivered to the `Tokenizer` in blocks. Since
this may pause parsing a token halfway through, it requires a little more of
the logic.

## Comparing solutions
To compare the different solutions we look at three basic aspects.

|Aspect|Description|Value|
|:--:|:---|:--:|
|Readability|Divided into **Crisp**, **Readable** and **Obscure** |10/5/0 points|
|Extendability|Bonus if the code can support both one-pass and block-wize |0/10 points|
|Speed|The number of tokens per µs on a reference CPU|0-100 points|

The reference-CPU is selected when testing but should be at least gen10 Intel I7
at 2.5 GHz. Typical speeds from 40-80 tokens/µs will give the intended evaluation.

The speed-test runs a pre-run to kick-start CPU (->preformance-mode) and runs for
at-least 5 seconds.
