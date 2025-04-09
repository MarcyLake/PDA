#ifndef PDA_H
#define PDA_H

#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<stack>
#include<queue>


using namespace std;

struct state {
    bool is_accept;
    string name;
    state(string n) {
        is_accept = false;
        name = n;
    }
};

struct edge_function {
    string read;
    string push;
    string pop;
    string from;
    string to;
    edge_function() {
        //just here so machine_snapshot constructor works
    }
    edge_function(string f, string t, string r, string po, string pu) {
        read = r;
        push = pu;
        pop = po;
        from = f;
        to = t;
    }
    
    void print() {
        cout << "Move from state " << from << " to state " << to << ". Read " << read << " from string. Pop " << pop << 
            " and push " << push << " from stack." << endl;
    }
};

struct machine_snapshot {
    string state;
    vector<string> s;
    stack<string> stack_;
    edge_function e;
    vector<edge_function> steps;
    machine_snapshot(string state_, vector<string> s_, stack<string> stack__, edge_function e_, vector<edge_function> steps_) {
        state = state_;
        s = s_;
        stack_ = stack__;
        e = e_;
        steps = steps_;
    }
};

class Tokenizer {
    private:   
        vector<string> reserved_words;
        bool is_reserved(string s);
        vector<string> is_substring(string s);
    public:
        Tokenizer();
        Tokenizer(vector<edge_function> edges);
        vector<string> tokenize(string s);
};

class PDA {
    private:
        vector<state*> states;
        string start_state;
        vector<edge_function> edges;
        int num_states;
        string epsilon;
        vector<string> read_from_file(string file_name);
        bool parse(vector<string> lines, bool is_from_file);
        vector<edge_function> run_help();
        queue<machine_snapshot> q;
        Tokenizer t;
        string remove_spaces(string s);
        bool last_run_valid;
        bool valid_machine;
        /*
        longest_path is a last minute addition
        if the sentence fails this will return the longest path that was attempted
        this won't *perfectly* map onto the path that came closest to succeeding (e.g. infinite loop in graph)
        but i think for the majority of machines it will at least be a good look at what went wrong
        which will make debugging a machine much easier
        */
        vector<edge_function> longest_path;
    
        void add_state(string name);
        void add_edge_function(edge_function e);
        bool search_state(string s);
        state* get_state(string name);
        bool set_accept_state(string name);
        void init_tokenizer();
    public:   
        PDA(string file_name);
        PDA(vector<string> lines, string e, string start, string accepts);
        ~PDA();
        bool valid();
        bool check_last_run();
        string get_epsilon();
        void print_machine();
        vector<edge_function> run(string s); 
};

#endif