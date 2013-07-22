#include <stdlib.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

//this made me half to type a little less. I really should have done it with the iterators, though.
typedef std::map<std::string, std::string> names;
typedef std::pair<std::string, std::string> a_name;
typedef std::map<std::string, std::string>::iterator name_iter;

typedef std::map<std::string, int> vals;
typedef std::pair<std::string, int> a_val;
typedef std::map<std::string, int>::iterator val_iter;

class Block { 
    Block *_enclosing;

public:
    Block(Block *enclosing) : _enclosing(enclosing){};
    ~Block(){};
    names var_names;
    vals var_vals;
    Block *enclosing();
};

//I only predefined these because they were the only ones I used in functions other than main.
void set(std::string val_name, std::string val);
std::string get(std::string val_name);
void unset(std::string val_name);
