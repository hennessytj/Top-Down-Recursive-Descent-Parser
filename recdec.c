/* 
 * Top-Down Parser
 * Recursive Descent Parser Specification
 * <program>	::= program <block> .
 * <block>		::= begin <stmtlist> end
 * <stmtlist>	::= <stmt> <morestmts>
 * <morestmts>	::= ; <stmtlist> | empty
 * <stmt>		::= <assign> | <ifstmt> | <whilestmt> | <block>
 * <assign>		::= <variable> = <expr>
 * <ifstmt>		::= if <testexpr> then <stmt> else <stmt>
 * <whilestmt>	::= while <testexpr> do <stmt>
 * <expr>		::= + <expr> <expr> | * <expr> <expr>
 *                          | <variable> | <digit>
 * <variable>	::= a | b | c
 * <digit>		::= 0 | 1 | 2
 *
 */

//***********************************
// definitions, header files, include
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE    2048
#define TRUE    1
#define FALSE   0
#define PROGRAM_ERROR       100
#define BLOCK_ERROR         101
#define STMTLIST_ERROR      102
#define MORESTMTS_ERROR     103
#define STMT_ERROR          104
#define ASSIGN_ERROR        105
#define IFSTMT_ERROR        106
#define WHILESTMT_ERROR     107
#define TESTEXPR_ERROR      108
#define EXPR_ERROR          109
#define VARIABLE_ERROR      110
#define DIGIT_ERROR         111

//************
// global vars
// version 2 won't use globals:-D
char buffer[SIZE];
char token[SIZE];
int lookahead = 0;
int var_ref = 0;
int assign_count = 0;

//******************** 
// function prototypes
// auxiliary function prototypes
void get_buffer(void);
void remove_whitespace(void);
void get_token(void);

// non-terminal function prototypes
void program(void);
int block(void);
void stmtlist(void);
int morestmts(void);
int stmt(void);
int assign(void);
int ifstmt(void);
int whilestmt(void);
int testexpr(void);
void expr(void);
int variable(void);
int digit(void);

//*****
// main
int main(void)
{
    get_buffer();
    //printf("%s\n", buffer);
    // only takes first line of code, will need to grab other lines later
	program();
    // program is syntactically correct
    printf("%d assignments, %d variable references\n", assign_count, var_ref);
	printf("Code successfully parsed.\n");

	return 0;
}

//**********
// functions
// auxiliary functions
// grabs next line of code
// replaces \n with nul byte
void get_buffer(void)
{
    int i = 0;
    int bcontinue = TRUE;
    lookahead = 0;
    fgets(buffer, SIZE, stdin);
    while (bcontinue)
    {
        if (buffer[0] == '\n')
        {
            fgets(buffer, SIZE, stdin);
        }
        else
            bcontinue = FALSE;
            
    }
    while (buffer[i] != '\n')
    { i++; }
    buffer[i] = '\0';
    printf("%s\n", buffer);
}

// increment index while whitespace
void remove_whitespace(void)
{
    while (buffer[lookahead] == ' ' || buffer[lookahead] == '\t')
    { lookahead++; }
}

// simple tokenizer, definitely not perfect:-)
void get_token(void)
{
    int i = 0;
    char tmp;
    // remove leading whitespace
    remove_whitespace();
    // next token is now ready or
    // if next char in buffer is nul byte grab another line
    if (buffer[lookahead] == '\0')
        get_buffer();
    // if another line was grabbed, we need to make sure
    // we remove leading whitespace
    // if there is no whitespace, the call to remove_whitespace
    // does nothing
    remove_whitespace();
    // grab next token, use space and nul byte to delimit
    tmp = buffer[lookahead];    // prime the loop
    while (tmp != ' ' && tmp != '\0')
    {
        token[i] = buffer[lookahead];   // copy token from buffer char-by-char
        i++;
        lookahead++;    // affects global variable
        tmp = buffer[lookahead];
    }
    token[i] = '\0';    // add nul byte
}

// Non-terminal functions
void program(void)
{
    // form: <program> ::= program <block> .
    get_token();    // grab first token in buffer
    if (strcmp("program", token) != 0)
    {
        printf("Error: reserved word: \"program\" missing\n");
        exit(PROGRAM_ERROR);
    }
    // post condition: <program> ::= program
    get_token();    // consume program
	block();
    // post condition: <program> ::= program <block>
	if (strcmp(".", token) != 0 && strcmp(token, "end.") != 0)
    {
        printf("Error: program missing terminating \".\"\n");
        exit(PROGRAM_ERROR);
    }
    // post condition: <program> ::= program <block> .
}

int block(void)
{
    // form: <block> ::= begin <stmtlist> end
    if (strcmp("begin", token) != 0)
    {
        printf("Error: block missing reserved word: \"begin\"\n");
        exit(BLOCK_ERROR);
    }
    // post condition: <block> ::= begin
    get_token();    // consume begin
	stmtlist();
    // post condition: <block> ::= begin <stmtlist>
    if (strcmp(token, "end") != 0 && strcmp(token, "end.") != 0)
    {
        printf("Error: block missing reserved word: \"end\"\n");
        exit(BLOCK_ERROR);
    }
    if (strcmp(token, "end") == 0)
        get_token();    // consume end
     // post condition: <block> ::= begin <stmtlist> end
    return TRUE;
}

void stmtlist(void)
{
    // form: <stmtlist> ::= <stmt> <morestmts>
    if (!stmt())
    {
        // post condition: <stmtlist> ::= improperly formatted syntax
        printf("ERROR: bad stmt in stmtlist\"%s\"\n", buffer);
        exit(STMTLIST_ERROR);
    }
    // post condition: <stmtlist> ::= <stmt>
    if (!morestmts())
    {
        // post condition: <stmtlist> ::= <stmt> improperly formatted syntax
        printf("ERROR: bad morestmts in stmtlist \"%s\"\n", buffer);
        exit(STMTLIST_ERROR);
    }
    // post condition: <stmtlist> ::= <stmt> <stmtlist>
}

int morestmts(void)
{
    // form: <morestmts> ::= ; <stmtlist> | empty
    if (strcmp(token, "end") == 0 || strcmp(token, "end.") == 0)
    {
        // post condition: <stmtlist> ::= empty
        // if next token is end, stmtlist is empty
        return TRUE;
    }
    else if (strcmp(token, ";") == 0)
    {
        // post condition: <morestmts> ::= ;
        get_token();    // consume ;
        stmtlist();
        // post condition: <morestmts> ::= ; <stmtlist>
        return TRUE;
    }
    else
    {
        // post condition: <morestmts> ::= improperly formatted syntax
        printf("morestmts token = %s\n", token);
        printf("ERROR: bad morestmts \"%s\"\n", buffer);
        exit(MORESTMTS_ERROR);
    }
    return FALSE;
}

int stmt(void)
{
    // form: <stmt> ::= <assign> | <ifstmt> | <whilestmt> | <block>
    if (assign())
    {
        // post condition: <stmt> ::= <assign>
        return TRUE;
    }
    else if (ifstmt())
    {
        // post condition: <stmt> ::= <ifstmt>
        return TRUE;
    }
    else if (whilestmt())
    {
        // post condition: <stmt> ::= <whilestmt>
        return TRUE;
    }
    else if (block())
    {
        // post condition: <stmt> ::= <block>
        return TRUE;
    }
    else
    {
        // post condition: <stmt> ::= improperly formatted syntax
        printf("ERROR: bad stmt \"%s\"\n", buffer);
        printf("<stmt> ::= <assign> | <ifstmt> | <whilestmt> | <block>\n");
        exit(STMT_ERROR);
    }
    return FALSE;
}

int assign(void)
{
    // form: <assign> ::= <variable> = <expr>
    if (variable())
    {
        // post condition: <assign> ::= <variable>
        if (strcmp(token, "=") == 0)
        {
            // post condition: <assign> ::= <variable> =
            get_token();    // consume =
            expr();
            assign_count++;
            // post condition: <assign> ::= <variable> = <expr>
            return TRUE;
        }
        // post condition: <assign> ::= <variable> but missing "="
    }
    // post condition: <assign> ::= improperly formatted syntax
    return FALSE;
}

int ifstmt(void)
{
    // form: <ifstmt> ::= if <testexpr> then <stmt> else <stmt>
    if (strcmp(token, "if") == 0)
    {
        // post condition: <ifstmt> ::= if
        get_token();    // consume if
        if (!testexpr())
        {
            printf("Error: bad testexpr in ifstmt \"%s\"\n", buffer);
            printf("<testexpr> ::= <variable> <= <expr>\n");
            exit(IFSTMT_ERROR);
        }
        // post condition: <ifstmt> ::= if <testexpr>
        if (strcmp(token, "then") != 0)
        {
            printf("Error: improperly formed ifstmt \"%s\"\n", buffer);
            printf("<ifstmt> ::= if <testexpr> then <stmt> else <stmt>\n");
            exit(IFSTMT_ERROR);
        }
        if (strcmp(token, "then") == 0)
        {
            get_token();    // consume then
        }
        // post condition: <ifstmt> ::= if <testexpr> then
        stmt();
        // post condition: <ifstmt> ::= if <testexpr> then <stmt>
        if (strcmp(token, "else") != 0)
        {
            printf("Error: improperly formed ifstmt \"%s\"\n", buffer);
            printf("<ifstmt> ::= if <testexpr> then <stmt> else <stmt>\n");
            exit(IFSTMT_ERROR);
        }
        if (strcmp(token, "else") == 0)
        {
            get_token();    // consume else
        }
        // post condition: <ifstmt> ::= if <testexpr> then <stmt> else
        stmt();
        // <ifstmt> ::= if <testexpr> then <stmt> else <stmt>
        return TRUE;
    }
    // post condition: <ifstmt> ::= improperly formatted syntax
    return FALSE;
}

int whilestmt(void)
{
    // form: <whilestmt> ::= while <testexpr> do <stmt>
    if (strcmp(token, "while") == 0)
    {
        // post condition: <whilestmt> ::= while
        get_token();    // consume while
        if (!testexpr())
        {
            // if not testexpr the whilestmt is jacked up
            printf("Error: bad testexpr in whilestmt \"%s\"\n", buffer);
            printf("<testexpr> ::= <variable> <= <expr>\n");
            exit(WHILESTMT_ERROR);
        }
        // post condition: <whilestmt> ::= while <testexpr>
        if (strcmp(token, "do") != 0)
        {
            // if not do the whilestmt is jacked up
            printf("Error: improperly formed whilestmt \"%s\"\n", buffer);
            printf("<whilestmt> ::= while <testexpr> do <stmt>\n");
            exit(WHILESTMT_ERROR);
        }
        // post condition: <whilestmt> ::= while <testexpr> do
        if (strcmp(token, "do") == 0)
        {
            get_token();    // consume do
        }
        stmt();
        // if a stmt is syntactically correct the whilestmt is:
        // post condition: <whilestmt> ::= while <testexpr> do <stmt>
        return TRUE;
    }
    // post condition: <whilestmt> ::= improperly formatted syntax
    return FALSE;
}

int testexpr(void)
{
    // form: <testexpr> ::= <variable> <= <expr>
    if (variable())
    {
        // post condition: <testexpr> ::= <variable>
        if (strcmp(token, "<=") == 0)
        {
            // post condition: <testexpr> ::= <variable> <=
            get_token();    // consume <=
            expr();
            // post condition: <testexpr> ::= <variable> <= <expr>
            return TRUE;
        }
        // post condition: <testexpr> ::= <variable> but missing <=
    }
    // post condition: improperly formatted syntax
    return FALSE;
}

void expr(void)
{
    // <expr> ::= + <expr> <expr> | * <expr> <expr> | <variable> | <digit>
    int i;
    // check for + <expr> <expr> or * <expr> <expr> case
    if (strcmp(token, "+") == 0 || strcmp(token, "*") == 0)
    {
        // check if two expr's follow
        // if so return true
        get_token();    // consume + or *
        for (i = 0; i < 2; i++)
        {
            if (!variable() && !digit())
            {
                printf("Error: invalid expr operation \"%s\"\n", buffer);
                printf("<expr> ::= + <expr> <expr> | * <expr> <expr> | <variable> | <digit>\n");
                exit(EXPR_ERROR);
            }
        }
        
    } // check for <variable> or <digit> case
    else if (!variable() && !digit())
    {
        printf("Error: invalid expr \"%s\"\n", token);
        printf("<expr> ::= + <expr> <expr> | * <expr> <expr> | <variable> | <digit>\n");
        exit(EXPR_ERROR);
    }
}

int variable(void)
{
    // <variable> ::= a | b | c
    // if a variable consume it and put next lexeme into token array then return true
    if (strcmp(token, "a") == 0 || strcmp(token, "b") == 0|| strcmp(token, "c") == 0)
    {
        get_token();       // consume variable
        var_ref++;
        return TRUE;
    }
    else
        return FALSE;
    // consider incrementing a count each time called
    // to keeep track var references
}
int digit(void)
{
    // <digit> ::= 0 | 1 | 2
    // if a digit consume it and put next lexeme into token array then return true
    if (strcmp(token, "0") == 0 || strcmp(token, "1") == 0 || strcmp(token, "2") == 0)
    {
        get_token();    // consume digit
        return TRUE;
    }
    else
        return FALSE;
}
