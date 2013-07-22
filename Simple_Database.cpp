/*
 * **************************************************
 * Problem 2 of Thumbtack's challenges for their job.
 * Simple Database
 * Can see full description here: 
 * http://www.thumbtack.com/challenges
 * **************************************************
 */

#include "simple_database.h"
using namespace std;

Block *block;
name_iter block_it;
val_iter val_it;

//for testing things and yeah.
void error(string the_error){
    cout << the_error;
    exit(0);
}

//convenience function to test if var is in given block.
//returns true if var_name is in a_block.
bool var_in_block(string var_name, Block* a_block){
    block_it = a_block->var_names.find(var_name);
    return block_it != a_block->var_names.end();
}

//convenience function to test if val is in given block.
//returns true if val is in a_block.
bool val_in_block(string val, Block* a_block){
    val_it = a_block->var_vals.find(val);
    return val_it != a_block->var_vals.end();
}

//return to past block that we were in before the user did a BEGIN.
Block *Block::enclosing(){
    return _enclosing;
}

//start a new block without copying all the old data over. That's for other functions to worry about.
void begin(){
    block = new Block(block);
}

//go back a Block. WOOPS DIDN'T NEED THAT DATA
void rollback(){
    Block * temp = block;
    if (block->enclosing() == (Block *) NULL)
	//I'm not sure if it should exit on an INVALID ROLLBACK but I had it do that 
	//because it didn't seem like a good idea to continue at this point...
	error("INVALID ROLLBACK\n");
    else {
	block = block-> enclosing();
	temp->Block::~Block();
    }
}

//a convenience function to find a block by name
Block* find_names(string val_name){
    Block *temp = block;
    block_it = block->var_names.find(val_name);
    for(
	    temp = block; 
	    temp->enclosing() != (Block *) NULL && block_it == temp->var_names.end();
	    temp = temp->enclosing(), block_it = temp->var_names.find(val_name)
       ){}
    return temp;
}

//a convenience function to find a block by value.
Block* find_vals(string val){
    val_it = block->var_vals.find(val);
    Block* temp = block;
    for(
	    temp = block;
	    //end if the next block is NULL, OR if you found it in var_vals.
	    //aka continue if the next block isn't NULL, AND if you haven't found val
	    temp->enclosing() != (Block *) NULL && val_it == temp->var_vals.end();
	    temp = temp->enclosing(), val_it = temp->var_vals.find(val)
       ){}
    return temp;
}

//set a value val_name to value val. Because there are blocks, this is tricky when you're resetting a variable that already exists.
//I wonder if I needn't have worried about pre-existing variables and assume that each new set would be unique without unsetting
//the old variable.
void set(string val_name, string val){
    Block *temp = find_names(val_name);  
    bool var_in_past_block = var_in_block(val_name, temp);
    if(var_in_block(val_name, block)){
	//already found IN THIS BLOCK so let's unset it first.
	unset(val_name);
    } else if(var_in_past_block && temp != block){
	//the value was in a PAST block so we need to know we are changing the number of values for NUMEQUALTO since we're
	//changing an existing val_name.
	block_it = temp->var_names.find(val_name);
	val_it = temp->var_vals.find(block_it->second);
	block->var_vals.insert(a_val(val_it->first,val_it->second-1));
    }
    block->var_names.insert(a_name(val_name, val));

    temp = find_vals(val);
    if(!val_in_block(val,temp)){
	//not found so need to add to list.
	block->var_vals.insert(a_val(val, 1));
    }
    else{
	//already exists, so insert in to where it belongs.
	int num_vals = val_it->second;
	if(!var_in_past_block){
	    //add one for this new addition.
	    num_vals ++;
	}	
	if(!val_in_block(val,block)){
	    //not in latest block, need to make a new insertion.
	    block->var_vals.insert(a_val(val,num_vals));
	} else {
	    val_it->second = num_vals;
	}
    }
}

//get the value of a variable, val_name. If it does not exist, return "NULL"
string get(string val_name){
    Block * temp = find_names(val_name);

    if(!var_in_block(val_name, temp)){
	return "NULL";
    }else{	
	return block_it->second;
    }    
}

//unset a variable with name val_name.
void unset(string val_name){
    Block * temp = find_names(val_name);

    if(!var_in_block(val_name, temp)){
	//doesn't exist, so don't unset because why would I do that? 
    }else{	
	string val = block_it->second;
	block_it = block->var_names.find(val_name);
	temp = find_vals(val);
	val_it = temp->var_vals.find(val);
	int temp_val = val_it->second - 1;
	/*
	 * If we are changing the current block, we want to actually erase the value.
	 * Otherwise, we don't want to, b/c rollback. We just want it to LOOK like we changed it.
	 */
	if(var_in_block(val_name, block)){
	    temp->var_names.erase(val_name);
	    val_it->second = temp_val;
	} 
	else
	{
	    block->var_names[val_name]="NULL";
	    block->var_vals.insert(a_val(val,temp_val));
	}

    }    
}

//find the number of current variables with the value val
int numequalto(string val){
    Block *temp = find_vals(val);
    if(!val_in_block(val,temp))
	return 0;
    return val_it->second;
}

void commit_vars(Block *add){
    if(add->enclosing() != block && add->enclosing() != (Block *) NULL){
	commit_vars(add->enclosing());
    }
    //don't need to do anything with var_vals because set should take care of that.
    block_it = add->var_names.begin();
    unsigned int it =0; 
    for(
	    block_it = add->var_names.begin();
	    block_it != add->var_names.end() && it<add->var_names.size();
	    ++block_it, it++
       )
    {
	set(block_it->first, block_it->second);
    }

}

//rollback all the blocks, except the first one. Simulating writing to file, except I apparently don't have to do that.
void commit(){
    Block * out = block;
    while(block->enclosing() != (Block *) NULL){
	block = block-> enclosing();
    }
    commit_vars(out);
    while(out->enclosing() != block){
	Block *temp = out;
	out=out->enclosing();
	temp->Block::~Block();
    }

}

//ignore whitespace, although my terminal background is actually purple so really it's ignoring purplespace.
char ignore_space(char c){
    while(isspace(c)){
	c = cin.get();
    }
    return c;
}

//find a word. I define a "word" to be any string of characters that do not have spaces
string nextword(char c){
    string next_word = "";
    while(!isspace(c)){
	next_word+=c;
	c=cin.get();
    }
    cin.putback(c);
    return next_word;
}

int main(){
    block = new Block((Block *) NULL);
    char c = cin.get();
    while(!cin.eof()){
	c=ignore_space(c);
	string next_word=nextword(c);
	if(next_word == "SET"){
	    c = cin.get();
	    c=ignore_space(c);
	    string arg_name = nextword(c);
	    c = cin.get();
	    c=ignore_space(c);
	    if(!isdigit(c)){
		error("This is not a valid character entry. You need to enter a digit there. \n");
	    }
	    string arg_val = "";
	    while(!isspace(c) && isdigit(c)){
		arg_val += c;
		c=cin.get();
	    }
	    set(arg_name, arg_val);
	}
	else if(next_word == "GET"){
	    string arg_name = "";
	    c = cin.get();
	    c=ignore_space(c);
	    while(!isspace(c)){
		arg_name+= c;
		c=cin.get();
	    }
	    string temp = get(arg_name);	    
	    cout << temp << endl;
	}
	else if (next_word == "END"){
	    exit(0);
	}
	else if (next_word == "NUMEQUALTO"){

	    c=cin.get();
	    c=ignore_space(c);
	    string arg_val = "";
	    while(!isspace(c) && isdigit(c)){
		arg_val += c;
		c=cin.get();
	    }
	    cout << numequalto(arg_val) << endl;
	}
	else if (next_word == "UNSET"){
	    c = cin.get();
	    c = ignore_space(c);
	    next_word = nextword(c);
	    c = cin.get();
	    unset(next_word);
	}
	else if (next_word == "BEGIN"){
	    c = cin.get();
	    begin();
	}
	else if (next_word == "ROLLBACK"){
	    c=cin.get();
	    rollback();
	}
	else if (next_word == "COMMIT"){
	    c=cin.get();
	    commit();
	}
    }
}
