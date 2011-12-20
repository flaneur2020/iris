#include "inc/iris.h"

static int ir_lex_step(IrLex *lp);
static int ir_lex_step_until(IrLex *lp, char *str);
static int ir_lex_consume(IrLex *lp, char c);
static int ir_lex_getc(IrLex *lp);
static int ir_lex_reset_buf(IrLex *lp, int size);
static int ir_lex(IrLex *lp);

static char ir_lex_name(IrLex *lp);
static char ir_lex_number(IrLex *lp);
static char ir_lex_string(IrLex *lp, char qc);
static char ir_lex_spaces(IrLex *lp);
static char ir_lex_digits(IrLex *lp);

char *tkstr[] = {
    ['.'] = ".",
    ['+'] = "+",
    ['-'] = "-",
    ['*'] = "*",
    ['/'] = "/",
    [':'] = ":",
    [','] = ",",
    ['>'] = ">",
    ['<'] = "<",
    ['{'] = "{",
    ['}'] = "}",
    ['('] = "(",
    [')'] = ")",
    [';'] = ";",
    [TK_NEWLINE] = "NEWLINE",
    [TK_GTE] = "GTE",
    [TK_LTE] = "LTE",
    [TK_NUMBER] = "<number>",
    [TK_STRING] = "<string>",
    [TK_NAME]  = "<name>",
    [TK_AND] = "and",
    [TK_OR] = "or",
    [TK_IF] = "if", 
    [TK_NIL] = "nil",
    [TK_FOR] = "for",
    [TK_WHILE] = "while",
    [TK_BREAK] = "break",
    [TK_CONTINUE] = "continue",
    [TK_LOCAL] = "local",
    [TK_FUNCTION] = "function"
};

static int tkkeywords[] = { 
    TK_AND, TK_OR, TK_IF, TK_LOCAL, TK_NIL, 
    TK_FOR, TK_WHILE, TK_BREAK, TK_CONTINUE, TK_FUNCTION 
};

// a reverse associated map of kwstr[];
static struct st_table *kwtab = NULL;

#define ir_lex_error(lp, fmt, ...) \
    do { fprintf(stderr, "Lex Error: %s:%d:%d: " fmt "\n", (lp)->path, (lp)->line, (lp)->col, ##__VA_ARGS__); exit(1); } while (0)

/* ------------------------------------------ */

int ir_token_init(IrToken *tp){
    tp->token = 0;
    tp->buf = malloc(NTOKEN_SIZE);
    memset(tp->buf, 0, NTOKEN_SIZE);
    return 0;
}

int ir_lex_init(IrLex *lp, char *path) {
    int nkw;
    int i;
    st_data_t tk;
    FILE *fp;

    fp = fopen(path, "r");
    if (fp == NULL)
        return -1;
    // misc init 
    lp->file = fp;
    lp->path = path;
    lp->file = fp;
    lp->line = 0;
    lp->col  = 0;
    lp->ch = '\0';
    ir_token_init(&lp->current);
    ir_token_init(&lp->lookahead);
    // read the first char
    ir_lex_step(lp);
    // init the kwtab with keywords
    if (kwtab)
        return 0;
    kwtab = st_init_strtable();
    nkw = sizeof(tkkeywords) / sizeof(int);
    for (i=0; i<nkw; i++) {
        tk = tkkeywords[i];
        st_insert(kwtab, (st_data_t)tkstr[tk], (st_data_t)tk);
    }
    // 
    ir_lex_next(lp);
    return 0;
}

int ir_lex_close(IrLex *lex){
    fclose(lex->file);
    free(lex->current.buf);
    free(lex->lookahead.buf);
    return 0;
}

/* --------------------------------------------- */

int ir_lex_next(IrLex *lp){
    lp->current.token = lp->lookahead.token;
    strcpy(lp->current.buf, lp->lookahead.buf);
    lp->lookahead.token = ir_lex(lp);
    return lp->current.token;
}

// Fetch one token each time, returns the type of token,
// and store the content of this token into lp->lookahead.buf[].
// On fetching finished, returns 0.
static int ir_lex(IrLex *lp) {
    char c;
    int r;
    int tk;

    while((c = lp->ch) != EOF && c != '\0') {
        ir_lex_reset_buf(lp, 0);
        if (c == '<') {
            c = ir_lex_step(lp);
            if (c == '=')  
                return TK_LTE;
            return '<';
        }
        if (c == '>') {
            c = ir_lex_step(lp);
            if (c == '=')  
                return TK_GTE;
            return '>';
        }
        if (c == '-') {
            c = ir_lex_step(lp);
            if (c != '-')
                return '-';
            // else it's a comment
            ir_lex_step_until(lp, "\n\r");
            continue;
        }
        // assign or eual
        if (c == '=') {
            c = ir_lex_step(lp);
            if (c != '=')
                return '=';
            ir_lex_step(lp);
            return TK_EQ;
        }
        // newline
        if (strchr("\n\r", c)) {
            ir_lex_step(lp);
            return TK_NEWLINE;
        }
        // skip the whitespaces
        if (isspace(c)) {
            ir_lex_spaces(lp);
            continue;
        }
        // single character tokens
        if (strchr(",.+:*/{}()", c)) {
            ir_lex_step(lp);
            return c;
        }
        // string
        if (strchr("\"\'", c)) {
            ir_lex_string(lp, c);
            return TK_STRING;
        }
        //
        if (isdigit(c)) {
            ir_lex_number(lp);
            return TK_NUMBER;
        }
        // keyword
        if (isalpha(c)) {
            ir_lex_name(lp);
            r = st_lookup(kwtab, (st_data_t)lp->lookahead.buf, (st_data_t*)&tk);
            if (r) {
                return tk;
            }
            return TK_NAME;
        }
        ir_lex_error(lp, "unkown token: %c", c);
    }
    return 0;
}

int ir_lex_step(IrLex *lp){
    char c;

    c = ir_lex_getc(lp);
    if (c == '\n') {
        lp->line++;
        lp->col = 0;
    }
    else {
        lp->col++;
    }
    return (lp->ch = c);
}

static int ir_lex_step_until(IrLex *lp, char *str){
    while (!strchr(str, ir_lex_step(lp)));
    return 0;
}

static int ir_lex_consume(IrLex *lp, char c) {
    lp->lookahead.buf[lp->lookahead.buf_size++] = c;
    return 0;
}

static int ir_lex_reset_buf(IrLex *lp, int size){
    lp->lookahead.buf_size = 0;
    return 0;
}

/* -------------------------------------------- */

static int ir_lex_getc(IrLex *lp) {
    return fgetc(lp->file);
}

/* -------------------------------------------- */

// digits { . digits }?
static char ir_lex_number(IrLex *lp){
    char c = lp->ch;

    ir_lex_digits(lp);
    // if it's an decimal
    if ((c = lp->ch) == '.') {
        ir_lex_consume(lp, c);
        ir_lex_digits(lp);
    }
    ir_lex_consume(lp, '\0');
    return c;
}

// \d+
static char ir_lex_digits(IrLex *lp) {
    char c = lp->ch;
    
    if (!isdigit(c)) 
        ir_lex_error(lp, "number expected, but got: %c", c);
    while (isdigit(c)) {
        ir_lex_consume(lp, c);
        c = ir_lex_step(lp);
    }
    ir_lex_consume(lp, '\0');
    if (isalpha(c)) {
        ir_lex_error(lp, "malformed number near %s, expected number, but got: %c", lp->lookahead.buf, c);
    }
    return c;
}

// \w[\d\w]*
static char ir_lex_name(IrLex *lp){
    char c = lp->ch;

    if (!isalpha(c))  
        return c;
    while(isalpha(c) || isdigit(c)){
        ir_lex_consume(lp, c);
        c = ir_lex_step(lp);
    }
    ir_lex_consume(lp, '\0');
    return 0;
}

// \s*
static char ir_lex_spaces(IrLex *lp) {
    char c = lp->ch;

    if (!isspace(c)) 
        return c;
    while (isspace(c)) {
        c = ir_lex_step(lp);
    }
    return c;
}

// ' '
static char ir_lex_string(IrLex *lp, char qc) {
    char c = lp->ch;

    while ((c = ir_lex_step(lp)) != qc) {
        switch(c) {
        case EOF:
        case '\0':
        case '\n':
        case '\r':
            ir_lex_error(lp, "unfinished string");
        case '\\':
            switch(c = ir_lex_step(lp)) {
            case 'n': ir_lex_consume(lp, '\n'); break;
            case 'r': ir_lex_consume(lp, '\r'); break;
            case 'a': ir_lex_consume(lp, '\a'); break;
            case 'b': ir_lex_consume(lp, '\b'); break;
            case 'f': ir_lex_consume(lp, '\f'); break;
            case 'v': ir_lex_consume(lp, '\v'); break;
            case '\\': ir_lex_consume(lp, '\\'); break;
            case '\"': ir_lex_consume(lp, '\"'); break;
            case '\'': ir_lex_consume(lp, '\''); break;
            case '\?': ir_lex_consume(lp, '\?'); break;
            // go through
            case '\n':
            case '\r': ir_lex_consume(lp, '\n'); break;
            }
        default:
            ir_lex_consume(lp, c);
        }
    }
    ir_lex_step(lp);
    ir_lex_consume(lp, '\0');
    return c;
}

/* ----------------------------- */
