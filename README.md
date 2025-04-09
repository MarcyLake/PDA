# PDA
To compile this program use the following command line prompt /n
g++ -std=c++17 PDA_main.cpp PDA.cpp Tokenizer.cpp /n
./a.out/n

The program 3 loads 3 machines, listed below
 - a^nb^n: this represents a language of some number of a's followed by the same number of b's
 - wwrev: for the alphabet {a, b, c} accepts any sentence that is a palindrome. While doing this writeup, I realized that the machine only accepts even-length sentences.
 - regex. Accepts valid regular expressions on 0's and 1's, using & to mean logical and, | to represent logical or, # for an empty string, e for an epsilon transition, U to mean union, A to mean intersection, ~ for negation, and * to mean 0, 1, or many. Parenthesis are also supported.

The program also allows you to input your own machine or read a file. Two files are provided, (sample1.txt and sample2.txt). These files can be read by the program, or used as a template for using the "make your own machine" function.
