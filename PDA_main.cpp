#include <iostream>
#include <string>
#include <vector>
#include "PDA.h"

void fine_print();
bool work_with_machine(PDA* p);
void simulate_run(vector<edge_function> edges, string s, string epsilon);

using namespace std;

int main() {
    system("clear");
    cout << "Welcome to the PDA machine!\n" << endl;
    bool master_loop = true;
    while (master_loop) {
        cout << "What would you like to do?" << endl;
        cout << "1. Load a mchine" << endl;
        cout << "2. Enter your own machine" << endl;
        cout << "3. Read machine from file" << endl;
        cout << "4. Quit" << endl;
        string choice;
        getline(cin, choice);
        if (choice == "1") {
            cout << "Which machine would you like to load?" << endl;
            cout << "1. a^nb^n" << endl;
            cout << "2. ww^r" << endl;
            cout << "3. Regex" << endl;
            cout << "4. Main menu" << endl;
            cout << "5. Exit program" << endl;
            string files[3] = {"anbn.txt", "wwrev.txt", "regex.txt"};
            getline(cin, choice);
            char c;
            if (choice.size() > 1) cout << "Please enter a number in the correct range" << endl;
            else {
                c = choice[0];
                if ((int)c < 49 || (int)c > 53) {
                    cout << "Please enter a number in the correct range" << endl;
                }
            }
            int machine_choice = (int)c - 49; //normally 48, menu starts at 1 so we need to subtract one more
            if (machine_choice == 5) return 0;
            else if (machine_choice == 4) int pass = 0; //do nothing
            else if (machine_choice >= 0 && machine_choice <= 2) { 
                PDA p = PDA(files[machine_choice]);
                if (p.valid()) {
                    p.print_machine();
                    bool keep_working = work_with_machine(&p);
                    if(!keep_working) return 0;
                }
            }
        }
        else if (choice == "2") {
            cout << "Let's make you a machine! :tada: Press 1 to see the fine print and 2 to start" << endl;
            cout << "1. Fine print" << endl;
            cout << "2. Start" << endl;
            getline(cin, choice);
            if (choice == "1") fine_print();
                vector<string> lines;
                cout << "Enter edge functions in the following format: " << endl;
                cout << "from, to, read, write, pop" << endl;
                cout << "where 'from' is the source state, 'to' is the destination state, and read write and pop are the symbols to read write and pop" << endl;
                cout << "to quit, just type 'quit'" << endl;
                bool reading = true;
                while (reading) {
                    string s;
                    getline(cin, s);
                    if (s == "quit") reading = false;
                    else {
                        lines.push_back(s);
                    }
                }
                string epsilon;
                cout << "Now define epsilon (read/pop/push nothing)" << endl;
                getline(cin, epsilon);
                string start;
                cout << "Enter the name of the start state" << endl;
                getline(cin, start);
                cout << "Now enter the name of every accept state seperated by a comma (eg: q1, q2)" << endl;
                string accepts;
                getline(cin, accepts);
                PDA p = PDA(lines, epsilon, start, accepts);
                if (p.valid()) {
                    p.print_machine();
                    bool keep_working = work_with_machine(&p);
                    if(!keep_working) return 0;
                }

        }
        else if (choice == "3") {
            cout << "===NOTES ON FILE FORMAT===" << endl;
            cout << "I got comment support working :3" << endl;
            cout << "The first line of the file should be formatted as: [epsilon, start state, accepet states]" << endl;
            cout << "E.G. '#, start, qAccept1, qAccept2' will set # as epsilon, start as the start state, and qAccept1 and qAccept 2 as accept states" << endl;
            cout << "The rest of the lines define edges functions as: [from, to, read, write, pop, push]. This defines what state the edge goes from and goes to, what is read from the string, and what is popped from and pushed to the stack." << endl;
            cout << "I've provided 'sample1.txt' and 'sample2.txt' as examples." << endl;
            string file_name;
            cout << "Enter file name: " << endl;
            getline(cin, file_name);
            PDA p = PDA(file_name);
            if (p.valid()) {
                p.print_machine();
                bool keep_working = work_with_machine(&p);
                if(!keep_working) return 0;
            }
        }
        else return 0;
    }
    return 0;
}

void fine_print() {
    cout << "A couple things to know: " << endl;
    cout << "1. All you have to enter is the edge functions, then define epsilon and enter the names of accept states" << endl;
    cout << "   Everything else is handled by the program based on this information" << endl;
    cout << "2. This program doesn't check if your machine makes sense. User errors are possible" << endl;
    cout << "   For example, if you use '#' for epsilon in the edges and then define it as 'e', that's valid. It will not work" << endl;
    cout << "   Likewise, if you mean for qAccept to be a start state and then enter qA the program will break" << endl;
    cout << "3. The program is capable of processing any inputs in the alphabet (including inputs longer than one character, like Let) with some condition" << endl;
    cout << "   You cannot use space as an input. I don't know why you would but the first step my algorithm is removing spaces from the string" << endl;
    cout << "   Likewise, no input can have a space in it. 'Foo bar' will not work" << endl;
    cout << "   If an input sentence is ambiguous in the language, my program just picks the first valid interpretation and runs it" << endl;
    cout << "   Disambiguating was out of the scope of this project. Sorry not sorry" << endl;
    cout << "4. There is no cycle detection in this program" << endl;
    cout << "   It utilizes a breadth-first search so it should never get stuck if there's a valid path" << endl;
    cout << "   However, if you make a machine that loops forever and never accepts you'll cause a stack overflow #rude" << endl;
    cout << "This has been the fine print. Hope you had fun! :3" << endl;
}

bool work_with_machine(PDA* p) {
    bool looping = true;
    while (looping) {
        cout << "What would you like to do now?" << endl;
        cout << "1. Test Sentence" << endl;
        cout << "2. Load a different machine" << endl;
        cout << "3. Exit Program" << endl;
        string choice;
        getline(cin, choice);
        if (choice == "1") {
            cout << "Test which sentence?" << endl;
            string s;
            getline(cin, s);
            vector<edge_function> steps = p->run(s);
            if (!p->check_last_run()) {
                cout << "No accepting path found! Showing the longest path" << endl;
                for (int i = 0; i < steps.size(); i++) {
                    cout << "Step " << i + 1 << ": ";
                    steps.at(i).print();
                }
            }
            else {
                cout << "Accepting path found" << endl;
                cout << "What would you like to do?" << endl;
                cout << "1. See accepting path (fast, less detail)" << endl;
                cout << "2. Simulate accepting run (slow, more detail)" << endl;
                cout << "3. Test another sentence" << endl;
                getline(cin, choice);
                if (choice == "1") {
                    for (int i = 0; i < steps.size(); i++) {
                        cout << "Step " << i + 1 << ": ";
                        steps.at(i).print();
                    }
                }
                else if (choice == "2") {
                    simulate_run(steps, s, p->get_epsilon());
                }
                else if (choice != "3") cout << "Invalid choice" << endl;
            }
        }
        else if (choice == "2") return true;
        else return false;
    }
    return false;
}

void pause() {
    cout << "Press any key to continue " << endl;
    string any_key;
    getline(cin, any_key);
}

void print_v_stack(vector<string> s) {
    cout << "top-> {";
   for (int i = s.size() - 1; i >= 0; i--) {
        cout << s.at(i);
       if (i != 0) cout << ", ";
   }
    if (s.empty()) cout << "empty";
    cout << "}" << endl;
}

void simulate_run(vector<edge_function> edges, string s, string epsilon) {
    string working_s = s;
    vector<string> stack_;
    for (int i = 0; i < edges.size(); i++) {
        edge_function e = edges.at(i);
        cout << "===STEP " << i + 1 << "===" << endl;
        cout << "Current string: ";
        if (working_s.empty()) cout << "EMPTY_STRING" << endl;
        else cout << working_s << endl;
        cout << "Current stack: "; 
        print_v_stack(stack_);
        cout << "Currently in state " << e.from << " going to state " << e.to << endl;
        cout << "Current edge function:" << endl;
        cout << "   Read: " << e.read << endl;
        cout << "   Pop: " << e.pop << endl;
        cout << "   Push: " << e.push << endl;
        pause();
        //update s and stack
        if (e.read != epsilon) {
            string temp = working_s.substr(e.read.size(), working_s.size() - 1);
            working_s = temp;
        }
        if (e.pop != epsilon) {
            stack_.pop_back();
        }
        if (e.push != epsilon) {
            stack_.push_back(e.push);
        }
        if (i == edges.size() - 1) {
            cout << "Currently in state: " << e.to << " which is an accept state" << endl;
            if (working_s.empty()) cout << "String is empty" << endl;
            else cout << "STRING IS NOT EMPTY SOMETHING HAS GONE WRONG https://xkcd.com/2200/" << endl;
            if (stack_.empty()) cout << "Stack is empty" << endl;
            else cout << " STACK IS NOT EMPTY SOMETHING HAS GONE WRONG https://xkcd.com/2200/" << endl;
            cout << "Since all 3 conditions are met, the sentence \"" << s << "\" is accepted by the machine" << endl; 
        }
    }
}