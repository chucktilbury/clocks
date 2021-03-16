/**
    @file scanner.c

    @brief

**/
#include "common.h"
#include "scanner.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {

    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isAtEnd() {

    return *scanner.current == '\0';
}

static char advance() {

    scanner.current++;
    return scanner.current[-1];
}

static char backup() {

    scanner.current--;
    return scanner.current[-1];
}

static char peek() {

    return *scanner.current;
}

static char peekNext() {

    if (isAtEnd())
        return '\0';
    return scanner.current[1];
}

static bool match(char expected) {

    if (isAtEnd())
        return false;

    if (*scanner.current != expected)
        return false;

    scanner.current++;
    return true;
}

static Token makeToken(TokenType type) {

    Token token;

    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;

    return token;
}

static Token errorToken(const char* message) {

    Token token;

    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;

    return token;
}

static void skipWhitespace() {

    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;

            case '\n':
                scanner.line++;
                advance();
                break;

            case '/':
                if(peekNext() == '*') {
                    while(!isAtEnd()) {
                        if(peek() == '*') {
                            advance();
                            if(peek() == '/') {
                                if(peekNext() == '\n') {
                                    scanner.line++;
                                    advance();
                                }
                                advance();
                                break; // skip more white if there is any
                            }
                            else if(peek() == '\n') {
                                scanner.line++;
                                advance();
                            }
                        }
                        else if(peek() == '\n') {
                            scanner.line++;
                        }
                        advance();
                    }
                }
                else if(peekNext() == '/') {
                    // A comment goes until the end of the line.
                    while(!isAtEnd()) {
                        if(peek() == '\n') {
                            scanner.line++;
                            advance();
                            break;
                        }
                        advance();
                    }
                }
                else {
                    return;
                }
                break;

            default:
                return;
        }
    }
}

static TokenType checkKeyword(int start, int length,

    const char* rest, TokenType type) {
    if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {

    switch (scanner.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
        case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
                    case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
                }
            }
            break;
        case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
        //case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
        case 'n':
            if(scanner.current - scanner.start > 1) {
                switch(scanner.start[1]) {
                    case 'i': return checkKeyword(2, 1, "l", TOKEN_NIL);
                    case 'o': return checkKeyword(2, 1, "t", TOKEN_BANG);
                }
            }
            break;
        case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
        case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
        case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

static Token identifier() {

    while (isalpha(peek()) || isdigit(peek()))
        advance();

    return makeToken(identifierType());
}

/**
    @brief Scan an unsigned number starting with '0x'.

    When this is called, the '0' is in the string and the 'x' is the next
    character. The maximum number of hex digits is 16.

    @return Token
**/
static TokenType unum() {

    int count = 0;

    advance();
    while(isxdigit(peek()) && count <= 16 && !isAtEnd()) {
        count++;
        advance();
    }

    return TOKEN_UNUM;
}

/**
    @brief Scan a float (double) that has a '.' and optionally a mantissa.

    When this is called, the part of the number before the '.' has been read.
    The '.' is the next character in the buffer. Look for a signed mantissa or
    the end of the input.

    @return Token
**/
static TokenType fnum() {

    advance();
    while(isdigit(peek()) && !isAtEnd())
        advance();

    if((peek() == 'e' || peek() == 'E') && !isAtEnd())
        advance();
    else
        return TOKEN_FNUM;

    if(((peek() == '-' || peek() == '+') && isdigit(peekNext())) && !isAtEnd()) {
        advance();
        while(isdigit(peek()) && !isAtEnd())
            advance();
    }
    else
        return TOKEN_ERROR;

    return TOKEN_FNUM;
}

/**
    @brief Scan a number.

    The number could be one of float, unsigned, or signed. A signed number is
    a simple series of integer digits. An unsigned number is in C-like hex
    notation that starts with a '0x'. Up to 16 digits are allowed. A float is
    a float starts with a decimal digit that could be a zero and has a '.' in
    it. An optional signed mantissa is also supported, starting with '[eE]'.

    @return Token
**/
static Token number() {

    TokenType tok_val = TOKEN_INUM; // simplest one
    int finished = 0;
    int state = 0;

    backup();
    while(!finished && !isAtEnd()) {
        char ch = peek();
        switch(state) {
            // Starting state. Can be any type.
            case 0:
                switch(ch) {
                    case '0':
                        state = 2;  // might be hex
                        advance();  // accept the '0'
                        break;
                    default:
                        if(isdigit(ch)) {
                            state = 1;
                            advance();
                        }
                        else
                            finished++;
                        break;
                }
                break;

            // This is the main state where an integer would be copied.
            case 1:
                switch(ch) {
                    case '.':
                        if(TOKEN_ERROR == (tok_val = fnum()))
                            return errorToken("Malformed floating point number.");
                        finished++;
                        break;
                    default:
                        if(isdigit(ch))
                            advance();
                        else
                            finished++;
                        break;
                }
                break;

            // Might be hex, decimal, or float. This verifies that it's not
            // a hex number.
            case 2:
                switch(ch) {
                    case 'x':
                    case 'X':
                        tok_val = unum();
                        finished++;
                        break;
                    case '.':
                        if(TOKEN_ERROR == (tok_val = fnum()))
                            return errorToken("Malformed floating point number.");
                        break;
                    default:
                        if(isdigit(ch)) {
                            state = 1;  // not hex or float
                            advance();
                        }
                        else
                            finished++;
                        break;
                }
                break;
        }
    }

    return makeToken(tok_val);
}

//     while (isdigit(peek()))
//         advance();

//     // Look for a fractional part.
//     if (peek() == '.' && isdigit(peekNext())) {
//         // Consume the ".".
//         advance();

//         while (isdigit(peek()))
//             advance();
//     }

//     return makeToken(TOKEN_NUMBER);
// }

/**
    @brief Insert an escape into the input stream.

    The len is the length of characters that is being replaced.
    The esc is a single character value to replace it with.

    Note that this is horrible. The scanner reads the whole file as a single
    block and to convert the string space from two characters to a single
    character, the whole block as to be memmove()d.

    FIXME: Come up with a better way for the scanner to receive characters
    from the input. Needs to scan a single line at a time and allow for things
    like strings that bridge lines.

    @param len
    @param esc
**/
static void insertEsc(int len, char esc) {

    char* str = (char*)scanner.current;
    *str = esc;
    memmove(&str[1], &str[len], strlen(&str[len])+1);
}

/**
    @brief Read a 2 digit hex number and return it as a char value.

    Convert the 2 digit hex number to a char and return it so it can be
    inserted into the string.

    @return char
**/
static char hexEscape() {

    char* str = (char*)scanner.current;
    str += 2;   // skip the \x, now str points to the hex number
    char temp = str[2]; // save the char at the end of the number
    str[2] = 0; // now it's a 2 char zero teminated string
    char ch = (char)strtol(str, NULL, 16); // convert the string
    str[2] = temp; // restore the previous character
    return ch;
}

/**
    @brief Copy string with escapes supported.

    Strings can span multiple lines. Newlines are embedded in the string when
    it spans lines. Escapes are supported, similar to C and other languages.

    @return Token
**/
static Token dqString() {

    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n')
            scanner.line++;
        else if(peek() == '\\') {
            switch(peekNext()) {
                case 'a':   insertEsc(2, '\a');    break;
                case 'b':   insertEsc(2, '\b');    break;
                case 'e':   insertEsc(2, '\x1b');  break;
                case 'f':   insertEsc(2, '\f');    break;
                case 'n':   insertEsc(2, '\n');    break;
                case 'r':   insertEsc(2, '\r');    break;
                case 't':   insertEsc(2, '\t');    break;
                case '\\':  insertEsc(2, '\\');    break;
                case '\'':  insertEsc(2, '\'');    break;
                case '\"':  insertEsc(2, '\"');    break;
                case '0':   insertEsc(2, '\0');    break;
                case 'x':   insertEsc(4, hexEscape()); break;
                default:    // unknown escape
                    return errorToken("Unknown escape in string.");
            }
        }
        advance();
    }

    if (isAtEnd())
        return errorToken("Unterminated string.");

    // The closing quote.
    advance();
    return makeToken(TOKEN_STRING);
}

/**
    @brief Copy string as absolute literal.

    Strings can include line breaks.

    @return Token
**/
static Token sqString() {

    while (peek() != '\'' && !isAtEnd()) {
        if (peek() == '\n')
            scanner.line++;
        advance();
    }

    if (isAtEnd())
        return errorToken("Unterminated string.");

    // The closing quote.
    advance();
    return makeToken(TOKEN_STRING);
}

Token scanToken() {

    skipWhitespace();
    scanner.start = scanner.current;

    if(isAtEnd())
        return makeToken(TOKEN_EOF);

    char c = advance();
    if (isalpha(c))
        return identifier();

    if (isdigit(c))
        return number();

    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        case '%': return makeToken(TOKEN_PERCENT);
        case '!': return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=': return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"': return dqString();
        case '\'': return sqString();
    }

    return errorToken("Unexpected character.");
}
