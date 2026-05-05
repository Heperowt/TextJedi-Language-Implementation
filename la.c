#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ID_LEN 30 //Maximum identifier size = 30
#define MAX_INT_LEN 10 //Maximum int size in C = 10

//Store keywords in a String array:
const char *keywords[] = {"new","int","text","size","subs","locate","insert","override","read","write","from","to","input","output","asText","asString"};



//Checks if this keyword is valid(contains in keywords array):
int isKeyword(const char *str) {
    for (int i = 0; i < 16; i++) { //We have 16 keywords
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}




int main(int argc, char *argv[]) {
    if (argc !=2) {
        printf("Use: la <script_name>\n");
        return 1;
    }


    //Add .tj and .lx and store it in in_filename and out_filename
    char in_filename[256];
    char out_filename[256];
    
    snprintf(in_filename, sizeof(in_filename), "%s.tj", argv[1]);
    snprintf(out_filename, sizeof(out_filename),"%s.lx", argv[1]);



    FILE *fin = fopen(in_filename , "r");
    //Check if fin fails to open:
    if(!fin) { 
        printf("Error: %s Could not open the input file!\n", in_filename);
        return 1;
    }



    FILE *fout = fopen(out_filename, "w");

    //Check if fout fails to open:
    if(!fout) {
        printf("Error: %s Could not create the output file!\n", out_filename);
        fclose(fin);
        return 1;
    }


    

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
                    fprintf(fout, "Lexical Error: A big lexeme just left open\n");
                    break;
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
                fprintf(fout, "Lexical Error: Identifier size exceeded (%s)\n", buffer);
            } else if (isKeyword(buffer)) {
                fprintf(fout, "Keyword(%s)\n", buffer);
            } else {
                fprintf(fout, "Identifier(%s)\n", buffer);
            }
            continue;
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
                fprintf(fout, "Lexical Error: Integer exceeded the integer limit (%s)\n", buffer);
            } else {
                fprintf(fout, "IntConst(%s)\n", buffer);
            }
            continue;
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
                fprintf(fout, "Lexical Error: A big lexeme just left open\n");
            } else {
                fprintf(fout, "StringConst(\"%s\")\n", buffer);
            }
            
            free(buffer);
            continue;
        }






        // Operators and Punctuation
        if (current == ':') {
            int next = fgetc(fin);
            if (next == '=') {
                fprintf(fout, "Operator(:=)\n");
            } else {
                ungetc(next, fin);
                fprintf(fout, "Lexical Error: invalid character ':'\n");
            }
        } else if (current == '+') {
            fprintf(fout, "Operator(+)\n");
        } else if (current == '-') {
            fprintf(fout, "Operator(-)\n");
        } else if (current == ';') {
            fprintf(fout, "EndOfLine\n");
        } else if (current == '(') {
            fprintf(fout, "OpenParanthesis\n");
        } else if (current == ')') {
            fprintf(fout, "CloseParanthesis\n");
        } else {
            fprintf(fout, "Lexical Error: invalid character '%c'\n", current);
        }
    }

    fclose(fin);
    fclose(fout);
    return 0;
}