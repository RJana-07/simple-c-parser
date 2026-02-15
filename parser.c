#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_VARS 100

char varNames[MAX_VARS][100];
int varValues[MAX_VARS];
int varCount=0;

typedef enum{
    TOKEN_INT,
    TOKEN_PRINT,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_ASSIGN,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

typedef struct{
    TokenType type;
    char value[100];
} Token;

const char* tokenTypeToString(TokenType type){
    switch(type){
        case TOKEN_INT:
            return "TOKEN_INT";
        case TOKEN_PRINT:
            return "TOKEN_PRINT";
        case TOKEN_IDENTIFIER:
            return "TOKEN_IDENTIFIER";
        case TOKEN_NUMBER:
            return "TOKEN_NUMBER";
        case TOKEN_PLUS:
            return "TOKEN_PLUS";
        case TOKEN_MINUS:
            return "TOKEN_MINUS";
        case TOKEN_MUL:
            return "TOKEN_MUL";
        case TOKEN_DIV:
            return "TOKEN_DIV";
        case TOKEN_ASSIGN:
            return "TOKEN_ASSIGN";
        case TOKEN_SEMICOLON:
            return "TOKEN_SEMICOLON";
        case TOKEN_LPAREN:
            return "TOKEN_LPAREN";
        case TOKEN_RPAREN:
            return "TOKEN_RPAREN";
        case TOKEN_EOF:
            return "TOKEN_EOF";
        default: return "TOKEN_UNKNOWN";
    }
}

Token getNextToken(FILE *file){
    Token token;
    token.type = TOKEN_UNKNOWN;
    token.value[0]='\0';

    int ch;

    while((ch=fgetc(file)) != EOF && isspace(ch)){}
    
    if(ch==EOF){
        token.type=TOKEN_EOF;
        return token;
    }

    if(isalpha(ch)){
        char buffer[100];
        int i=0;
        buffer[i++] = (char)ch;

        while((ch=fgetc(file))!=EOF && isalnum(ch)){
            if(i<99) buffer[i++] = (char)ch;
        }
        buffer[i]='\0';

        if(ch != EOF) ungetc(ch, file);

        if(strcmp(buffer, "int")==0){
            token.type = TOKEN_INT;
        }
        else if
            (strcmp(buffer, "print")==0){
                token.type = TOKEN_PRINT;
            }
        else{
            token.type = TOKEN_IDENTIFIER;
            strcpy(token.value, buffer);
        }

        return token;
    }

    if(isdigit(ch)){
        char buffer[100];
        int i=0;
        
        buffer[i++]=(char)ch;

        while ((ch=fgetc(file)) != EOF && isdigit(ch)){
            if(i<99) buffer[i++]=(char)ch;
        }
        buffer[i]='\0';

        if(ch != EOF) ungetc(ch, file);

        token.type=TOKEN_NUMBER;
        strcpy(token.value, buffer);
        
        return token;
    }

    switch(ch){
        case '+': token.type = TOKEN_PLUS;
            break;
        case '-': token.type = TOKEN_MINUS;
            break;
        case '*': token.type = TOKEN_MUL;
            break;
        case '/': token.type = TOKEN_DIV;
            break;
        case '=': token.type = TOKEN_ASSIGN;
            break;
        case ';': token.type = TOKEN_SEMICOLON;
            break;
        case '(': token.type = TOKEN_LPAREN;
            break;
        case ')': token.type = TOKEN_RPAREN;
            break;
        default:
            token.type = TOKEN_UNKNOWN;
            token.value[0]=(char)ch;
            token.value[1]='\0';
    }

    return token;
}

FILE *src;
Token currentToken;

//Function to read next token into currentToken
void advance(){
    currentToken=getNextToken(src);
}

//Printing a syntax error msg and stop the program
void syntaxError(const char *msg){
    printf("Syntax Error: %s\n", msg);
    exit(1);
}

//If the token is expected specific token then take it or else error
void expect(TokenType t, const char *expectedName){
    if(currentToken.type != t){
        printf("Syntax Error : Expected %s but found %s", expectedName, tokenTypeToString(currentToken.type));

        if(currentToken.type == TOKEN_IDENTIFIER ||
            currentToken.type == TOKEN_NUMBER ||
            currentToken.type == TOKEN_UNKNOWN) {
                printf("(%s)", currentToken.value);
            }
            
            printf("\n");
            exit(1);
    }
    advance();
}

int findVar(const char *name){
    for(int i=0; i<varCount; i++){
        if(strcmp(varNames[i], name) == 0){
            return i;
        }
    }
    return -1;
}

void setVar(const char *name, int value){
    int idx = findVar(name);

    if(idx==-1){
        if(varCount>=MAX_VARS){
            printf("Semantic Error: Too many variables \n");
            exit(1);
        }
        strcpy(varNames[varCount], name);
        varValues[varCount]=value;
        varCount++;
    }
    else{
        varValues[idx]=value; //update the existing variable
    }
}

int getVar(const char *name){
    int idx=findVar(name);

    if(idx==-1){
        printf("Semantic Error : Variable '%s' used before declaration\n", name);
        exit(1);
    }
    return varValues[idx];
}

int parseExpression();
int parseTerm();
int parseFactor();

int parseFactor(){
    if(currentToken.type == TOKEN_NUMBER){
        int value=atoi(currentToken.value);
        advance();
        return value;
    }

    if(currentToken.type==TOKEN_IDENTIFIER){
        int value=getVar(currentToken.value);
        advance();
        return value;
    }

    syntaxError("Expected number or identifier in expression");
    return 0;
}

int parseTerm(){
    int left=parseFactor();

    while(currentToken.type == TOKEN_MUL || currentToken.type == TOKEN_DIV){
        TokenType op = currentToken.type;
        advance();

        int right=parseFactor();

        if(op == TOKEN_MUL){
            left=left*right;
        }
        else{
            if(right==0){
                printf("Semantic Error: Division by zero\n");
                exit(1);
            }
            left=left/right;
        }
    }
    return left;
}

int parseExpression(){
    int left=parseTerm();

    while(currentToken.type == TOKEN_PLUS || currentToken.type==TOKEN_MINUS){
        TokenType op = currentToken.type;
        advance();

        int right=parseTerm();

        if(op==TOKEN_PLUS){
            left=left+right;
        }
        else{
            left=left-right;
        }
    }
    return left;
}

void parseDeclaration(){
    expect(TOKEN_INT, "int");

    if(currentToken.type != TOKEN_IDENTIFIER){
        syntaxError("Expected identifier after int");
    }

    char name[100];
    strcpy(name, currentToken.value);
    advance();

    expect(TOKEN_ASSIGN, "=");

    int value=parseExpression();

    expect(TOKEN_SEMICOLON, ";");

    setVar(name, value);

}

void parsePrint(){
    expect(TOKEN_PRINT, "print");
    expect(TOKEN_LPAREN, "(");

    if(currentToken.type != TOKEN_IDENTIFIER){
        syntaxError("Expected identifier inside print()");
    }

    char name[100];
    strcpy(name, currentToken.value);
    advance();

    expect(TOKEN_RPAREN, ")");
    expect(TOKEN_SEMICOLON, ";");

    int value=getVar(name);
    printf("%d\n", value);
}

void parseStatement(){
    if(currentToken.type == TOKEN_INT){
        parseDeclaration();
    }
    else if(currentToken.type == TOKEN_PRINT){
        parsePrint();
    } 
    else if(currentToken.type == TOKEN_EOF){
        return;
    }
    else{
        syntaxError("Expected 'int' or 'print' statement");
    }
}

void parseProgram(){
    while(currentToken.type != TOKEN_EOF){
        parseStatement();
    }
}


int main( int argc, char *argv[]){

    if(argc<2){
        printf("Usage: %s <inputfile>\n", argv[0]);
        return 1;
    }
    
    FILE *file =fopen(argv[1], "r"); 
    
   if(file == NULL){
        printf("Error opening file \n");
        return 1;
    }

    src = file;     // give file to parser
    advance();      // load first token
    parseProgram(); // parse + execute

    fclose(file);
    return 0;
}