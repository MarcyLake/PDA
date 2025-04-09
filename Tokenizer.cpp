#include<vector>
#include<string>
#include<iostream>
#include"PDA.h"

using namespace std;

bool Tokenizer::is_reserved(string s) {
    for (int i = 0; i < reserved_words.size(); i++) {
        if (s == reserved_words.at(i)) {
            return true;
        }
    }
    return false;
}

Tokenizer::Tokenizer() {
}

Tokenizer::Tokenizer(vector<edge_function> edges) {
    for (int i = 0; i < edges.size(); i++) {
        string read = edges.at(i).read;
        if (!reserved_words.empty()) {
            bool already_present = is_reserved(read);
            if (!already_present) reserved_words.push_back(read);
        }
        else {
            reserved_words.push_back(read);
        }
    }
}

/*
The main thing this class does
takes a string (to be read by a machine)
return a vector of strings that can be 'read' one at a time
the algorithm is a little complext but not hard (i made it more complicated to do error checking)
    
*/
vector<string> Tokenizer::tokenize(string s) {
    vector<string> result;
    if (s.empty()) {
        cout << "ERROR CANNOT TOKENIZE EMPTY STRING" << endl;
        return result;
    }
    string temp;
    //a failsafe for if something is included in the string that is not in the machine
    bool failed = true;
    for (int i = 0; i < s.size(); i++) {
        failed = true;
        temp += s[i];
        //we have found a reserved word (part of the machine alphabet and/or stack alphabet)
        if (is_reserved(temp)) {
            //before we push it back, we need to deal with the possibility that this reserved word is the substring of another reserved word
            vector<string> subs = is_substring(temp);
            //we know we're looking at a reserved word, so if we're at the end of the loop we succeed
            //this condition lets the recursion work
            /*
            a note on that for my own sake. the example i was testing with first was foobarfoo
            the reason this condition is needed is that the string foo in that alphabet should accept
            however, the next conidion (subs.size == 1) prohibited foo from ever accepting since 
            even when foo is the only thing in the string, it is still a substring of foobar
            this condition gets around that without harming the intended functionality of the program
            */
            if (i == s.size() - 1) {
                //we can do this bc we know temp is reserved
                result.push_back(temp);
                temp = "";
                //failed should be false when we exit the string because at the last character we should have made a reserved word
                failed = false;
            }
            else if (subs.size() == 1) {
                result.push_back(temp);
                temp = "";
                //failed should be false when we exit the string because at the last character we should have made a reserved word
                failed = false;
            }
            else {
                //there's more notes on this algorithm at the bottom of this file
                //iterate through all of subs (note: subs contains temp bc is_substring() always starts by adding temp to the result)
                for (int i = 0; i < subs.size(); i++) {
                    //first isolate the rest of the string 
                    string test_word = subs.at(i);
                    string remainder;
                    //error protection before substring (if we're trying foobar but string is foo it throws out of range)
                    if (s.size() > test_word.size()) {
                        //make a substring starting one more than the size of the string going until the end of the string
                        remainder = s.substr(test_word.size(), s.size() - 1);
                    }
                    vector<string> check;
                    //we can't tokenize an empty string. check to make sure remainder is non-etmpy
                    if (!remainder.empty()) {
                         //recusrsively try to tokenize rest of string
                        check = tokenize(remainder);
                    }
                    //if check is non-empty we were able to tokenize the remainder of the string
                    if (!check.empty()) {
                        //start with the test word as the first token
                        result.push_back(test_word);
                        for(int j = 0; j < check.size(); j++) {
                            //add every other element from check
                            result.push_back(check.at(j));
                        }
                        return result;
                    }
                }
            }
        }
    }
    if (failed) {
        vector<string> garbage;
        return garbage;
    }
    //this is not just so the compiler doesn't yell at me
    //any language where no word is a substring of another returns here
    return result;
}

/*
Exists for an edge case where, for example, int and ints are both reserved words
or more realistically something like A and A'
If a and a' are both in the alphabet, the string a'a will fail to tokenize, which is wrong
this function is just meant to determine if the substring of a reserved keyword
return the keyword if true, returns an empty string if false
*/
vector<string> Tokenizer::is_substring(string s) {
    vector<string> result;
    result.push_back(s); //include s here to make things easier within tokenize()
    int size = s.size();
    for (int i = 0; i < reserved_words.size(); i++) {
        //if the word we're looking at is shorter than s, s cannot be a substring of it
        if (reserved_words.at(i).size() > size) {
            string sub = reserved_words.at(i).substr(0, size);
            if (sub == s) result.push_back(reserved_words.at(i));
        }
    }
    return result;
}

 /*
i moved this down here to make the function itself easier to read
the final condition in tokenize is to handle the case where a reserved word is a substring of another reserved word
for example, the string a'a in an alphabet a and a' are both valid  
here's a basic example sentence in an alphabet with foo and foobar

f o o b a r f o o
0 1 2 3 4 5 6 7 8
currently, i = the index of s that first recognizes a reserved word so i = 2
temp = foo (reserved in this example)
subs = {foo, foobar} (note that subs is a vector bc if food is also a reserved word we can include it in the vector)
(note also that subs always includes temp. This is implemented in is_substring() and makes things much easier)
HIGH LEVEL ALGO
for every element of subs
    isolate the rest of s as a substring
    try to tokenize that string
    if we do, combine it with the previous tokenization and return
    otherwise move onto the next element of subs
    if we finish looping subs without finding one, the string is invalid
EG
s = foobarfoo
subs = {foo, foobar}
try to tokenize barfoo (substring of s after foo)
this fails, move to next element of subs
try to tokenize foo (substring of s after foobar)
this accepts
return {foobar, foo}

this is a greedy algorithm that just accepts the first valid string it finds
i'm sure there are btter ways to do this by which i mean i'm sure no actual tokenizer uses a greedy algorithm
i'm using one here because one of the results of my program accepting any machine is that i don't know the rules ahead of time
for example, consider the following lettuce code
let x = 5 in 
let xlet = 6 in
xlet + x
the greedy algo returns {let, x, =, 5, let, x, let, = 6, in, x, let, +, x}
which is wrong
if I knew I was doing lettuce I could work on solving this problem by coding a specific case for the 'let' keywork
but because i don't know the rules, i can't do that because then if someone uses let differently it breaks
*/