#include <fstream>
#include <vector>
#include <iostream>

using namespace std;


/*####################################################*\
#......................................................#
#...UTILITY............................................#
#......this is the largest part of the code............#
#......................................................#
#......................................................#
\*####################################################*/


/*#...Writing.............................#...#..#.###*/

/**
 * this will push a \\t before every line of the string 
 * @param str a string
 * @return a string with a \\t placed after every \\n
*/
string pushBack(string str) {
    str = "\t" + str;
    //index
    std::string::size_type n = 0;
    while ( ( n = str.find( "\n", n ) ) != std::string::npos ) {
        str.replace( n, 1, "\n\t");
        n += 2;
    }
    return str;
}

/**
 * converts your primitives and objects into
 * name: [
 * \tNAME:VALUE
 * \t{
 * \t\t...
 * \t}
 * ]
 * @param name name of the object
 * @param data array of primitives and objects (manually converted into strings using toPrimitive() and toObject()/toArray() respectfully)
 * @param c length of the array
*/
string toArray(string name, string* data, int c) {
    string prefix = name + ": [\n";
    for(int i = 0; i < c; ++i)
        prefix += pushBack(data[i]) + "\n";
    return prefix + "]";
}

/**
 * converts your primitives and objects into
 * {
 * \tNAME:VALUE
 * \t{
 * \t\t...
 * \t}
 * }
 * @param data array of primitives and objects (manually converted into strings using toPrimitive() and toObject()/toArray() respectfully)
 * @param c length of the array
*/
string toObject(string* data, int c) {
    string prefix = "{\n";
    for(int i = 0; i < c; ++i)
        prefix += pushBack(data[i]) + "\n";
    return prefix + "}";
}

/**
 * converts your primitives (and objects) into
 * {NAME:VALUE;NAME:VALUE}
 * 
 * !!! USE FOR PRIMITIVES ONLY AS TO NOT BULK UP THE CODE !!!
 * @param data array of primitives and objects (manually converted into strings using toPrimitive() and toObject()/toArray() respectfully)
 * @param c length of the array
*/
string toObjectInline(string* data, int c) {
    string prefix = "{" + data[0];
    for(int i = 1; i < c; ++i)
        prefix += "; " + data[i];
    return prefix + "}";
}
/**
 * converts your values into
 * NAME:VALUE
 * (value is automatically escaped, name is not)
 * @param name name:VALUE
 * @param value NAME:value
*/
string toPrimitive(string name, string value) {
    for(int i = 0; i < value.length(); ++i)
        switch(value[i]) {
            //Special characters need to be escaped
            case '\\':
            case ';': 
            case '\n':
            case '[': 
            case '{':
            case ']': 
            case '}':
            case ':':
            case ' ':
            case '\t':
                value = value.insert(i,1,'\\');
                ++i;
                break;
        }
    return name + ": " + value;
}


/*#...Reading.............................#...#..#.###*/

struct Token {
    string name;
    vector<Token> data;
    int c;
    string value;

    bool obj;

    Token() {}

    Token(string name,string value) {
        this->name = name;
        this->value = value;
    }

    /**
     * returns the Token with the same NAME as str
     * use Token.data[index] to index the Token or use 
     * Token.getValue(to_string(index)) if you know
     * that the root token was generated from an array
     * instead of an object
     * 
     * !!! ONLY USE ON Token::DataToken !!!
     * @param str the name to be found
     * @return the Token with the same name as str.
     * if not found, token.value will be NOT FOUND
    */
    Token* getValue(string str) {
        if(obj) {
            for(int i = 0; i < data.size(); ++i) {
                if(data[i].name == str)
                    return (data.data()+i);
            }
            return new Token(str,"NOT FOUND");
        }
        cerr << "Token::getValue(): Token passed is not an ObjectToken, terminating...";
        terminate();
    }
};

struct DataToken : Token {
    bool obj = false;
};

struct ObjectToken : Token {
    bool obj = true;
};

/**
 * Turns your file into a Token tree from which you can get the data from by using Token::getValue();
 * @param str the begining of the root token (should be a value, not {})
*/
ObjectToken tokenize(ifstream& str) {
    ObjectToken root;
    root.name = "root";
    char s;
    string name = to_string(0);
    string value = "";
    bool escaped = false;
    while(str.read(&s,1)) {
        //if last character was a \, escape this character
        if(escaped) {
            value.push_back(s);
            escaped = false;
            continue;
        }

        switch(s) {
            //if current character is a \, escape the next character
            case '\\':
                escaped = true;
                break;
            //end of current assignment, push to the stack and go to the next one
            case ';': case '\n':
                if(!value.empty()) {
                    DataToken dt = *new DataToken();
                    dt.name = name;
                    dt.value = value;
                    root.data.push_back(dt);
                    name = to_string(root.data.size());
                    value = "";
                }
                break;
            //[] and {} define Objects (or Arrays) both are handled the same way, skips the { and [
            case '[': case '{': 
            {
                ObjectToken ot = tokenize(str);
                ot.name = name;
                root.data.push_back(ot);
                name = to_string(root.data.size());
                value = "";
                break;
            }
            /* ] and } mean that a subsection was closed off, this works because if you go into a new
            tokenize() function at its corresponding [/{, it will keep the same stringstream, skipping
            the code inbetween the []/{} respectfully */
            case ']': case '}':
                if(!value.empty()) {
                    DataToken dt = *new DataToken();
                    dt.name = name;
                    dt.value = value;
                    root.data.push_back(dt);
                    name = to_string(root.data.size());
                    value = "";
                }
                return root;
                break;
            //since its NAME:VALUE we know that after a : we just got past the NAME and are just now writing to VALUE
            case ':':
                name = value;
                value.clear();
                break;
            //whitespace is skipped unless escaped
            case ' ': case '\t':
                break;
            //write the currently selected char to value
            default:
                value.push_back(s);
                break;
        }
    }
    //if value was not pushed, push it now
    if(!value.empty()) {
        DataToken dt = *new DataToken();
        dt.name = name;
        dt.value = value;
        root.data.push_back(dt);
        name = to_string(root.data.size());
        value = "";
    }
    return root;
}
