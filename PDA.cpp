#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<stack>
#include<queue>
#include"PDA.h"

using namespace std;

PDA::PDA(string file_name) {
    valid_machine = true;
    vector<string> lines = read_from_file(file_name);
    if (lines.empty()) {
        cout << "UNABLE TO READ FILE, MACHINE NOT MADE" << endl;
        valid_machine = false;
    }
    else {
        bool valid = parse(lines, true);
        if (!valid) {
            cout << "ERROR: unable to parse input file" << endl;
            cout << "This machine is inoperable DO NOT USE IT" << endl;
            valid_machine = false;
        }
        init_tokenizer();
        last_run_valid = false;
    }
}

PDA::PDA(vector<string> lines, string e, string start, string accepts) {
    valid_machine = false;
    bool valid = parse(lines, false);
    if (!valid || accepts.empty()) {
        cout << "ERROR: Error creating machine. Hopefully the errors above this are useful" << endl;
        cout << "This machine is inoperable DO NOT USE IT" << endl;
        valid_machine = false;
    }
    else {
        epsilon = e;
        start_state = start;
        string holder;
        //go through accepts, seperate at commas, set accept states
        for (int i = 0; i < accepts.size(); i++) {
            if (accepts[i] == ',') {
                bool accepted = set_accept_state(holder);
                if (!accepted) {
                    cout << "ERROR: Error creating machine. Invalid accept state " << holder << endl;
                    cout << "This machine is inoperable DO NOT USE IT" << endl;
                    valid_machine = false;
                }
                holder = "";
            } else if (i == accepts.size() - 1) {
                holder += accepts[i];
                bool accepted = set_accept_state(holder);
                if (!accepted) {
                    cout << "ERROR: Error creating machine. Invalid accept state " << holder << endl;
                    cout << "This machine is inoperable DO NOT USE IT" << endl;
                    valid_machine = false;
                }
            }
            else {
                if(accepts[i] != ' ') holder += accepts[i];
            }
        }
    }
    init_tokenizer();
    last_run_valid = false;  
}

PDA::~PDA() {
    for (int i = 0; i < num_states; i++) {
        state* s = states[i];
        delete s;
    }
    num_states = 0;
}

bool PDA::valid() {
    return valid_machine;
}

bool PDA::check_last_run() {
    return last_run_valid;
}

vector<string> PDA::read_from_file(string file_name) {
    ifstream file(file_name);
    vector<string> lines;
    if (file.is_open()) {
        string line;
        while (file.good()) {
            getline(file, line);
            lines.push_back(line);
        }
        file.close();
    }
    else {
        cout << "Failed to open " << file_name << endl;
        file.close();
        vector<string> garbage;
        return garbage;
    }
    return lines;
}

//slightly different behavior from file vs from user input
//i folded it into the for loop so i didn't have to rewrite the comma parsing code
bool PDA::parse(vector<string> lines, bool is_from_file) {
    //loop for reading edges starts at a different place depending on if we read a file line or not
    //this tracks that
    int start_index = 0;
    //the first line of files (not user inputs) sets epsilon and accept states
    //i handle that seperately at the start of the file
    if (is_from_file) {
        bool found_first_line = false;
        int index = 0;
        //first check for comments at the start of the file
        while (!found_first_line) {
            string test_line = lines.at(index);
            if (test_line[0] == '/' && test_line.size() >= 2 && test_line[1] == '/') {
                start_index++;
            }
            else found_first_line = true;
            index++;
        }
        //next read the first non-comment line of the file
        vector<string> file_first_line;
        string first_line = lines.at(start_index);
        start_index++;
        string holder;
        for (int i = 0; i < first_line.size(); i++) {
            if (first_line[i] == ',') {
                    if (holder.empty()) {
                        cout << "ERROR: holder was empty while reading first line of file." << endl;
                        return false;
                    }
                    else {
                        file_first_line.push_back(holder);
                        holder = "";
                    }
                }
                //because the line doesn't end with a comma
                else if (i == first_line.size() - 1) {
                    holder += first_line[i];
                    file_first_line.push_back(holder);
                }
                else if (first_line[i] != ' ') {
                    holder += first_line[i];
                }
        }
        if (!file_first_line.empty()) {
            //the first thing we find sets epsilon
            epsilon = file_first_line.at(0);
            cout << "set epsilon" << endl;
            //the second thing is a start state
            add_state(file_first_line.at(1));
            start_state = file_first_line.at(1);
            //the rest are accept states
            for (int i = 2; i < file_first_line.size(); i++) {
                add_state(file_first_line.at(i));
                bool valid_state = set_accept_state(file_first_line.at(i));
                if (!valid_state) return false;
            }
        }
        else {
            cout << "ERROR: Failed to read first line of file" << endl;
            return false;
        }
    }
    
    //iterate through each line
    for (int i = start_index; i < lines.size(); i++) {
        bool is_comment = false;
        //declare things i need
        string s = lines.at(i);
        string params[5];
        int param_index = 0;
        string holder;
        //make sure the line isn't empty
        if (!s.empty()) {
            //iterate through the line
            for (int j = 0; j < s.size(); j++) {
                if (s[j] == '/' && j < s.size() - 1 && s[j + 1] == '/') {
                    is_comment = true;
                    j = s.size() - 1;
                }
                //if we hit a comma 
                else if (s[j] == ',') {
                    if (holder.empty()) {
                        cout << "ERROR: at param " << param_index << " holder was empty." << endl;
                        return false;
                    }
                    else {
                        params[param_index] = holder;
                        param_index++;
                        holder = "";
                    }
                }
                //because the line doesn't end with a comma
                else if (j == s.size() - 1) {
                    holder += s[j];
                    params[param_index] = holder;
                }
                else if (s[j] != ' ') {
                    holder += s[j];
                }
            }
        }
        if (!is_comment) {
            //if we read the wrong number of elements
            if (param_index != 4) { //4 because base 0 is such a funny girl
                cout << "ERROR: Line \""<< s << "\" contained wrong number of elements" << endl;
                return false;
            }

            //now that we have the paramters in an array, we need to go through them and make an edge function
            string from = params[0];
            string to = params[1];
            string read = params[2];
            string pop = params[3];
            string push = params[4];
            //check to see if the states are in the machine or not
            //if the to state isn't in the machine, add it
            //if the from state isn't in the machine, add it
            if (!search_state(from)) {
                  add_state(from);
            }
            if (!search_state(to)) {
                    add_state(to);
            }
            edge_function e(from, to, read, pop, push);
            add_edge_function(e);
            }
    }
    return true;
}

string PDA::remove_spaces(string s) {
    int spaces_removed = 0;
    for (int i = s.size() - 1; i >= 0; i--) {
        if (s[i] == ' ') {
            for (int j = i; j < s.size() - 1; j++) {
                s[j] = s[j + 1];
            }
            spaces_removed++;
        }
    }
    string result = s.substr(0, s.size() - spaces_removed);
    return result;
}

string PDA::get_epsilon() {
    return epsilon;
}

void PDA::add_state(string name) {
    state* s = new state(name);
    states.push_back(s);
    num_states++;
}

void PDA::add_edge_function(edge_function e) {
    edges.push_back(e);
}

bool PDA::search_state(string s) {
    for (int i = 0; i < states.size(); i++) {
        if (states.at(i)->name == s) return true;
    }
    return false;
}

state* PDA::get_state(string name) {
    for (int i = 0; i < states.size(); i++) {
        if (states.at(i)->name == name) return states.at(i);
    }
    cout << "FAILED TO FIND STATE " << name << endl;
    cout << "RETURNING FIRST STATE" << endl;
    string something_has_indeed_gone_wrong;
    cin >> something_has_indeed_gone_wrong;
    return states.at(0);
}

bool PDA::set_accept_state(string name) {
    state* s = get_state(name);
    if (s == NULL) return false;
    s->is_accept = true;
    return true;
}

void PDA::init_tokenizer() {
    t = Tokenizer(edges);
}

void PDA::print_machine() {
    cout << "===PRINTING MACHINE===" << endl;
    cout << "Machine states: {";
    for (int i = 0; i < num_states; i++) {
        cout << states.at(i)->name;
        if (states[i]->name == start_state) {
            cout << " (start state)";
        }
        if (states[i]->is_accept) {
            cout << " (accept state)";
        }
        if (i < num_states - 1) {
            cout << ", ";
        }
    }
    cout << "}" << endl;
    cout << "Epsilon: " << epsilon << endl;
    cout << "delta is defined by the following edges: " << endl;
    for (int i = 0; i < edges.size(); i++) {
        edges.at(i).print();
    }
    cout << endl;
}

vector<edge_function> PDA::run(string s) {
    //if the string is empty and we're in an accept state the sentence automatically accepts
    string state = get_state(start_state)->name;
    //REASONS NOT TO CODE IN C++ = if (s.empty() && get_state(state-)>is_accept) {
    if (s.empty() && get_state(state)->is_accept) {
        cout << "Accepted empty sentence in start state. NO steps were needed" << endl;
        vector<edge_function> victory_garbage;
        return victory_garbage;
    }
    //initialize variables that get reused every run
    last_run_valid = false;
    longest_path = {};
    vector<string> sv;
    if(!s.empty()) {
        //remove spaces from string
        s = remove_spaces(s);
        //tokenize the string
        sv = t.tokenize(s);
        if (sv.empty()) {
            vector<edge_function> garbage;
            return garbage;
        }
    }

    stack<string> stack_;    
    //find all valid edges from start state
    string first_char;
    if (s.empty()) first_char = epsilon;
    else first_char = sv.at(0);
    for (int i = 0; i < edges.size(); i++) {
        edge_function e = edges.at(i);
        //if we're coming from the right state
        if (e.from == state) {
            //if we're reading the right character or nothing
            if (e.read == epsilon || first_char == e.read) {
                //we know the stack is empty here, so if we're not popping anything
                if (e.pop == epsilon) {
                    //update that we took this step
                    vector<edge_function> steps;
                    steps.push_back(e);
                    //edge is valid, add ms with other machine info and this edge to the q
                    machine_snapshot ms(state, sv, stack_, e, steps);
                    q.push(ms);
                }
            }
        }
    }
    return run_help();
}

vector<edge_function> PDA::run_help() {
    /*
    pop the first element from the queue
    create a new ms representing the changes indicated by the popped element
    add ms's for all valid edges in the new state to the q
    call function again
    */
    //check failure (q is empty, no valid edges remain)
    if (q.empty()) {
        return longest_path;
    }
    machine_snapshot current_ms = q.front();
    q.pop();
   
    //isolate the edge we're testing
    edge_function test_edge = current_ms.e;
    //update state, string, and stack based on edge
    string new_state = test_edge.to;
    vector<string> new_s = current_ms.s;
    if (!new_s.empty() && test_edge.read != epsilon) {
        new_s.erase(new_s.begin());
    }
    stack<string> new_stack = current_ms.stack_;
    if (!current_ms.stack_.empty() && test_edge.pop != epsilon && test_edge.pop == current_ms.stack_.top()) new_stack.pop();
    if (test_edge.push != epsilon) new_stack.push(test_edge.push);
    
    //check acceptance
    // we do this here with the new stack since if these conditions are met after updating everything it means we've moved to an accept state
    if (new_s.empty() && new_stack.empty() && get_state(new_state)->is_accept) {
        last_run_valid = true;
        return current_ms.steps;
    }
    
    //now that we know what state we're in, what the string is, and what the stack is, find all valid edges with this updated information
    //these three variables will stay the same for all ms we make
    string first_char;
    if (new_s.empty()) first_char = epsilon;
    else first_char += new_s[0];
    for (int i = 0; i < edges.size(); i++) {
        edge_function e = edges.at(i);
        //if we're coming from the right state
        if (e.from == new_state) {
            //if the string is empty and we're reading nothing, 
            //or if the string is not empty and we're reading either epsilon or the right character
            //i chose to make one hideous condition to avoid copying the success condition code too many times
            //#trusttheprocess
            if ((new_s.empty() && e.read == epsilon) || (!new_s.empty() && (e.read == epsilon || e.read == first_char))) {
                //if the stack is empty and we're popping nothing
                //or if the stack is not empty and we're popping epsilon or the correct thing
                //same logic as above
                if ((new_stack.empty() && e.pop == epsilon) || (!new_stack.empty() && (e.pop == epsilon || e.pop == new_stack.top()))){
                        //define a vector (new_steps) representing the steps we've taken so far plus the new step
                        vector<edge_function> new_steps = current_ms.steps;
                        new_steps.push_back(e);
                        //update longest_path
                        if (longest_path.empty()) {
                            longest_path.push_back(e);
                        }
                        else if (new_steps.size() > longest_path.size()) {
                            longest_path = new_steps;
                        }
                        //edge is valid, add ms with other machine info and this ms to the q
                        machine_snapshot ms(new_state, new_s, new_stack, e, new_steps);
                        q.push(ms);
                }
            }
        }
    }
    return run_help();
}