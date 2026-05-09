#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>



//Macros:
#define MAX_ID_LEN 30 //Maximum identifier size = 30
#define MAX_INT_LEN 10 //Maximum int size in C = 10
#define MAX_STR_LEN 4096



// ---------------------------------------------------------------------------------SYMBOL TABLE:
typedef enum {TYPE_INT,TYPE_TEXT} VarType;



typedef struct Variable {
    char name[MAX_ID_LEN +1];
    VarType type;
    char *str_val;
    unsigned int int_val;     

    struct Variable *next;
}Variable;



Variable *symbol_table = NULL;


//Looks up and returns a variable from the symbol table using its identifier:
Variable* find_var(const char *name) {
    Variable *curr = symbol_table;
    while (curr) {
        if ( strcmp(curr->name, name) == 0 ) return curr;
        curr = curr->next;
    }
    return NULL;
}


//Add an identifier to symbol table
Variable* add_var(const char *name , VarType type) {
    if(find_var(name)) {
        printf("Semantic Error: Variable '%s' already declared.\n", name);
        exit(1);
    }
    Variable *v = (Variable*)malloc(sizeof(Variable));
    strncpy(v->name , name , MAX_ID_LEN);
    v->type= type;
    v->int_val= 0;
    v->str_val= strdup(""); 
    v->next= symbol_table;
    symbol_table= v;
    return v;
}





//---------------------------------------------------------------------Lexical Analyser:

//store token types:
typedef enum {T_EOF,T_ID,T_INT,T_STR,T_KW,T_OP_ASSIGN,T_OP_PLUS,T_OP_MINUS,T_SEMI,T_LPAREN,T_RPAREN, T_COMMA} TokenType;

//Store keywords in a String array:
const char *keywords[] = {"new","int","text","size","subs","locate","insert","override","read","write","from","to","input","output","asText","asString", "prompt"};

FILE *fin = NULL;
TokenType cur_token;
char cur_text[MAX_STR_LEN];
unsigned int cur_int;


//Checks if this keyword is valid(contains in keywords array):
int isKeyword(const char *str) {
    for (int i = 0; i < 17; i++) { //We have 17 keywords
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void throw_error(const char *msg) {
    printf("Interpreter Error: %s\n" , msg);
    exit(1);
}


void next_token() {
    int current;
    while ( (current = fgetc(fin)) != EOF ) {
        //continue if we reach to whitespace characters
        if (isspace(current)) {
            continue;
        }

        //commentline control:
        if (current == '/') {
            int next;
            if ( ( next = fgetc(fin) ) == '*') { //ensuring that we have * after /
                int comment_closed = 0;
                while ((current = fgetc(fin)) != EOF) {
                    if (current == '*') {
                        int check_slash;
                        if ( (check_slash = fgetc(fin) ) == '/') {
                            comment_closed = 1;
                            break;
                        }
                        ungetc(check_slash, fin); 
                    }
                }

                if (!comment_closed) {
                    throw_error("Lexical Error: A big lexeme just left open");
                }
                continue;
            } else {
                ungetc(next, fin);
            }
        }

        // Identifier and Keyword control:
        if (isalpha(current)) {
            char buffer[32];
            int i = 0;
            buffer[0] = current;
            i++;
            
            while ((current = fgetc(fin)) != EOF && (isalnum(current) || current== '_')) {
                if (i <= MAX_ID_LEN) {
                    buffer[i++] = current;
                }
            }
            buffer[i]= '\0';
            ungetc(current,fin);

            if (i >MAX_ID_LEN) {
                throw_error("Lexical Error: Identifier size exceeded");
            } else if (isKeyword(buffer)) {
                strcpy(cur_text, buffer);
                cur_token = T_KW;
                return;
            } else {
                strcpy(cur_text, buffer);
                cur_token = T_ID;
                return;
            }
        }

        // Integer constants (Zero or Positive)
        if(isdigit(current)) {
            char buffer[32];
            int i = 0;
            buffer[0] = current;
            i++;
            
            while ((current = fgetc(fin)) != EOF && isdigit(current)) {
                if (i <= MAX_INT_LEN) buffer[i++] = current;
            }
            buffer[i] = '\0';
            ungetc(current, fin);
            
            if (i > MAX_INT_LEN) {
                throw_error("Lexical Error: Integer exceeded the integer limit");
            } else {
                cur_int = (unsigned int)strtoul(buffer, NULL, 10);
                cur_token = T_INT;
                return;
            }
        }

        // String constants ("...")
        if (current == '"') {
            int capacity = 32; // Beggining capacity
            char *buffer = (char *)malloc(capacity * sizeof(char));
            int i = 0;

            int string_closed = 0;
            
            while ((current = fgetc(fin))!= EOF) {
                if (current == '"') {
                    string_closed = 1;
                    break;
                }
                
                //Double the capacity if buffer is full
                if (i >= capacity - 1) { 
                    capacity *= 2;
                    buffer = (char *)realloc(buffer, capacity * sizeof(char));
                }
                
                buffer[i++] = current;
            }
            buffer[i] = '\0';
            
            if (!string_closed) {
                free(buffer);
                throw_error("Lexical Error: A big lexeme just left open");
            } else {
                strncpy(cur_text, buffer, MAX_STR_LEN - 1);
                cur_text[MAX_STR_LEN - 1] = '\0';
                cur_token = T_STR;
                free(buffer);
                return;
            }
        }

        // Operators and Punctuation
        if (current == ':') {
            int next = fgetc(fin);
            if (next == '=') {
                cur_token = T_OP_ASSIGN; return;
            } else {
                ungetc(next, fin);
                throw_error("Lexical Error: invalid character ':'");
            }
        } else if (current == '+') {
            cur_token = T_OP_PLUS; return;
        } else if (current == '-') {
            cur_token = T_OP_MINUS; return;
        } else if (current == ';') {
            cur_token = T_SEMI; return;
        } else if (current == '(') {
            cur_token = T_LPAREN; return;
        } else if (current == ')') {
            cur_token = T_RPAREN; return;
        } else if (current == ',') {
            cur_token = T_COMMA; return; 
        } else {
            throw_error("Lexical Error: invalid character");
        }
    }
    cur_token = T_EOF;
}

void expect(TokenType expected, const char *err_msg) {
    if (cur_token !=expected) throw_error(err_msg);
    next_token();
}

//-------------------------------------------------------------------EVALUATOR AND PARSER

//defining Value type:
typedef struct {
    VarType type;
    unsigned int i;
    char *s;
    } Value;

//returns value in Value form
Value get_value() {
    Value val = {TYPE_INT,0,NULL}; //initialize
    if(cur_token == T_INT) {
        val.type =TYPE_INT; 
        val.i = cur_int;
        next_token();
    }else if(cur_token == T_STR) {
        val.type = TYPE_TEXT; 
        val.s = strdup(cur_text);
        next_token();
    }else if(cur_token == T_ID) {
        Variable *v = find_var(cur_text);
        if(!v) throw_error("Undeclared variable used in expression.");
        val.type = v->type;
        if(v->type == TYPE_INT) val.i = v->int_val;
        else val.s = strdup(v->str_val);
        next_token();
    }else if( (cur_token == T_KW) && (strcmp(cur_text , "size") == 0 || strcmp(cur_text, "asString") == 0 || strcmp(cur_text, "asText") == 0) ) {
        char func[30];
        strcpy(func, cur_text);
        next_token(); 

        
        expect(T_LPAREN, "missing (");
        
        Value arg = get_value();
        
        expect(T_RPAREN, "missing )");
        
        if(strcmp(func, "size") == 0) {
            if (arg.type != TYPE_TEXT) throw_error("Type Error: 'size' expects a text argument.");
            val.type = TYPE_INT;
            val.i = strlen(arg.s); 
        } else if(strcmp(func , "asString") == 0) { 
            if (arg.type != TYPE_INT) throw_error("Type Error: 'asString' expects an int argument.");
            val.type =  TYPE_TEXT; 
            char buf[32]; 
            sprintf(buf, "%u", arg.i); 
            val.s = strdup(buf); 
        } else if(strcmp(func, "asText")==0) {
            if (arg.type != TYPE_TEXT) throw_error("Type Error: 'asText' expects a text argument.");
            val.type = TYPE_INT; 
            val.i = (unsigned int) strtoul(arg.s , NULL , 10);
        }
    } else{
        throw_error("Expected value (identifier, integer or string).");
    }
    return val;
}

//Assigns an evaluated Value to an existing variable in the symbol table:
void assign_var(const char *name , Value val) {
    Variable *v = find_var(name);

    if (!v) throw_error("Cannot assign to undeclared variable.");
    if (v->type != val.type) throw_error("Type mismatch in assignment.");
    if (v->type ==TYPE_INT) {
        v->int_val = val.i;
    } else{
        if(v->str_val) free(v->str_val);
        v->str_val = val.s;
    }
}

void parse_statement() {
    //for keytwords:
    if(cur_token == T_KW && strcmp(cur_text , "new") == 0) {
        next_token();
        VarType vtype;
        if (strcmp(cur_text , "int") == 0) vtype = TYPE_INT;
        else if (strcmp(cur_text , "text") == 0) vtype = TYPE_TEXT;
        else throw_error("Expected 'int' or 'text' after 'new'.");
        next_token();
        
        if (cur_token != T_ID) throw_error("Expected identifier in declaration.");
        add_var(cur_text, vtype);
        next_token();
        expect(T_SEMI, "Expected ';' after declaration.");
    } 
    else if (cur_token == T_KW && strcmp(cur_text , "output") == 0) {
        next_token();
        Value val = get_value();
        if (val.type == TYPE_INT) printf("%u\n", val.i);
        else printf("%s\n", val.s);
        expect(T_SEMI, "Expected ';' after output statement.");
    }
    else if (cur_token == T_KW && strcmp(cur_text , "input") == 0) {
        next_token();
        if (cur_token != T_ID) throw_error("Expected variable after 'input'.");
        char var_name[MAX_ID_LEN]; strcpy(var_name, cur_text);
        next_token();
        

        if (cur_token != T_KW || strcmp(cur_text, "prompt") != 0) {
            throw_error("Expected 'prompt' keyword.");
        }
        next_token(); 
        
        Value prompt_val = get_value();
        
        printf ("%s", prompt_val.s);
        char input_buf[100];
        fgets (input_buf , sizeof(input_buf), stdin);
        input_buf[strcspn(input_buf, "\n")] = 0; 
        
        Value res = {TYPE_TEXT, 0, strdup(input_buf)};
        assign_var(var_name, res);
        expect(T_SEMI, "Expected ';'.");
    }


    //for identifiers:
    else if (cur_token == T_ID) {
        char target_var[MAX_ID_LEN];
        strcpy(target_var , cur_text);
        next_token();
        expect(T_OP_ASSIGN, "Expected ':=' after identifier.");
        
        
        Value val1 = get_value();
        
        if (cur_token == T_SEMI) {
            assign_var(target_var, val1);
        }else if (cur_token == T_OP_PLUS || cur_token == T_OP_MINUS) {
            TokenType op = cur_token;
            next_token();
            Value val2 = get_value();
            Value res;
            if (val1.type == TYPE_INT && val2.type == TYPE_INT) {
                res.type = TYPE_INT;
                if (op == T_OP_PLUS) res.i = val1.i+val2.i;
                else {
                    if (val2.i > val1.i) throw_error("Subtracting larger value from smaller integer.");
                    res.i = val1.i -val2.i;
                }
            }else if (val1.type == TYPE_TEXT && val2.type == TYPE_TEXT) {
                res.type = TYPE_TEXT;
                if (op == T_OP_PLUS) {
                    res.s = malloc(strlen(val1.s) + strlen(val2.s) + 1);
                    strcpy(res.s, val1.s); strcat(res.s , val2.s);
                } else {
                    if (strlen(val2.s) > strlen(val1.s)) throw_error("Subtrahend larger than minuend.");
                    char *p = strstr(val1.s, val2.s); //does val1 contains val2?
                    if(!p) res.s = strdup(val1.s);
                    else {
                        int pre = p - val1.s;
                        int sub_len = strlen(val2.s);
                        res.s = malloc(strlen(val1.s) - sub_len + 1);
                        strncpy(res.s, val1.s, pre);
                        strcpy(res.s + pre, p + sub_len);
                    }
                }
            } else throw_error("Type mismatch in arithmetic expression.");

            assign_var(target_var, res);
        }
        expect (T_SEMI , "Expected ';' after statement.");
    
    } else {
        throw_error("Syntax error. Unrecognized statement.");
    }
}





int main(int argc, char *argv[]) {
    if (argc != 2){
        printf("Usage: TextJedi <script_name>\n");
        return 1;
    }

    char in_filename[256];
    snprintf(in_filename , sizeof(in_filename) , "%s.tj" , argv[1]);

    fin = fopen(in_filename, "r");
    if (!fin) { 
        printf("Error: %s Could not open the input file!\n", in_filename);
        return 1;
    }

    //Start reading:
    next_token();
    while (cur_token !=T_EOF) {
        parse_statement();
    }


    fclose(fin);
    return 0;
}