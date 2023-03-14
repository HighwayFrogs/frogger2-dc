#include "token.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

SCRIPT_TOKEN* MakeBasicToken(SCRIPT_TOKEN_TYPE type) {
    SCRIPT_TOKEN* result;

    result = malloc(sizeof(SCRIPT_TOKEN));
    if (!result)
        return nullptr;

    result->type = type;
    result->next = nullptr;
    return result;
}

SCRIPT_TOKEN_VALUE32* MakeTokenValue32(SCRIPT_TOKEN_TYPE type, uint32_t value) {
    SCRIPT_TOKEN_VALUE32* result;

    result = malloc(sizeof(SCRIPT_TOKEN_VALUE32));
    if (!result)
        return nullptr;

    result->type = type;
    result->next = nullptr;
    result->value = value;
    return result;
}

SCRIPT_TOKEN_STRING* MakeStringToken(SCRIPT_TOKEN_TYPE type, char* str) {
    SCRIPT_TOKEN_STRING* result;

    result = malloc(sizeof(SCRIPT_TOKEN_STRING));
    if (!result)
        return nullptr;

    result->type = type;
    result->next = nullptr;
    result->ref = StringRef_Create(str);
    return result;
}

SCRIPT_TOKEN_STRING* MakeStringRefToken(SCRIPT_TOKEN_TYPE type, STRING_REFERENCE* ref) {
    SCRIPT_TOKEN_STRING* result;

    result = malloc(sizeof(SCRIPT_TOKEN_STRING));
    if (!result)
        return nullptr;

    result->type = type;
    result->next = nullptr;
    result->ref = StringRef_AddUsage(ref);
    return result;
}

SCRIPT_TOKEN* ReadToken(const char source[], int* pos, int length, SCRIPT_TOKEN_LOCATION* location);

char* ExtractString(const char source[], int startPos, size_t length) {
    char* result = malloc(length + 1);
    if (!result)
        return nullptr;

    errno_t err = strncpy_s(result, length + 1, &source[startPos], length);
    result[length] = '\0';
    return result;
}

bool IsValidIdentifier(char c, bool isFirstChar) {
    return isalpha(c) || (c == '_') || (!isFirstChar && isdigit(c));
}

bool isHex(char c) {
    return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

void FindFilePosition(const char source[], int pos, SCRIPT_TOKEN_LOCATION* result) {
    result->line = 1;
    result->column = 1;

    for (int i = 0; i < pos; i++) {
        if (source[i] == (char)'\n') {
            result->line++;
            result->column = 1;
        } else if (source[i] == (char)'\t') {
            result->column += 4;
        } else {
            result->column++;
        }
    }
}

char* FindFilePositionText(const char source[], int pos) {
    SCRIPT_TOKEN_LOCATION location;
    FindFilePosition(source, pos, &location);
    return GetLocationText(&location);
}

char* GetLocationText(const SCRIPT_TOKEN_LOCATION* location) {
    return location ? StringBuilder_Format("line: %d, col: %d", location->line, location->column) : "(NULL)";
}


// Adapted from http://csci.viu.ca/~wesselsd/courses/csci485_2018/code/cpp/tokenizer/tokens.c
// If an error occurs, the error will be returned as the root, with all successfully parsed tokens attached. It is up to the caller to handle this error.
// Otherwise, a linked list of tokens is returned.
SCRIPT_TOKEN* Tokenize(const char source[]) {
    SCRIPT_TOKEN* head = nullptr;
    SCRIPT_TOKEN* tail = nullptr;
    SCRIPT_TOKEN* next;

    int pos = 0;
    int length = strlen(source);

    // Read tokens.
    SCRIPT_TOKEN_LOCATION location = {1, 1};
    while (pos < length) {
        next = ReadToken(source, &pos, length, &location);
        next->location = location;

        // If there was an error, return it.
        if (next->type == TOKEN_ERROR) {
            next->next = head;
            return next;
        }

        // Link into list.
        if (!head)
            head = next;
        if (tail)
            tail->next = next;
        tail = next;
    }

    if (!tail)
        head = tail = MakeBasicToken(TOKEN_EOF);
    if (tail->type != TOKEN_EOF) {
        tail->next = MakeBasicToken(TOKEN_EOF);
        tail = tail->next;
    }

    return head;
}

bool IsSingleLineCommentStart(const char source[], const int* pos, int length) {
    return (length > *pos + 1 && source[*pos] == '/' && source[*pos + 1] == '/');
}

bool isBinary(char c) {
    return c == '0' || c == '1';
}

// Reads the token found at the current position in the text.
// Returns an error token if the token cannot be parsed.
SCRIPT_TOKEN* ReadToken(const char source[], int *pos, int length, SCRIPT_TOKEN_LOCATION* location) {
    // Skip whitespace.
    bool isSingleLineComment = false;
    while (length > *pos && (isspace(source[*pos]) || isSingleLineComment || IsSingleLineCommentStart(source, pos, length))) {
        // Skip comments.
        if (!isSingleLineComment && IsSingleLineCommentStart(source, pos, length))
            isSingleLineComment = true; else if (source[*pos] == '\n')
            isSingleLineComment = false;

        (*pos)++;
    }

    int startPos = *pos;
    FindFilePosition(source, *pos, location);

    // Reached end.
    if (*pos >= length)
        return MakeBasicToken(TOKEN_EOF);

    /* look at the next character to identify the potential token type */

    /* handle variables (lowercase alphabetic) */
    if (IsValidIdentifier(source[*pos], true)) {
        while (length > *pos && IsValidIdentifier(source[*pos], false))
            (*pos)++;
        
        char* value = ExtractString(source, startPos, *pos - startPos);
        return (SCRIPT_TOKEN*)MakeStringToken(TOKEN_IDENTIFIER, value);
    }

    /* handle binary numbers */ else if (length > *pos + 2 && source[*pos] == '0' && source[*pos + 1] == 'b' && isBinary(source[*pos + 2])) {
        *pos += 2;
        while (length > *pos && isBinary(source[*pos]))
            (*pos)++;

        char* numStr = ExtractString(source, startPos + 2, *pos - (startPos + 2));
        char* end;
        uint32_t value = strtoul(numStr, &end, 2);
        free(numStr);
        return (SCRIPT_TOKEN*)MakeTokenValue32(TOKEN_INTEGER, value);
    }

    /* handle hexadecimal numbers */ else if (length > *pos + 2 && source[*pos] == '0' && source[*pos + 1] == 'x' && isHex(source[*pos + 2])) {
        *pos += 2;
        while (length > *pos && isHex(source[*pos]))
            (*pos)++;

        char* numStr = ExtractString(source, startPos + 2, *pos - (startPos + 2));
        char* end;
        uint32_t value = strtoul(numStr, &end, 16);
        free(numStr);
        return (SCRIPT_TOKEN*)MakeTokenValue32(TOKEN_INTEGER, value);
    }

    /* handle numbers (one or more digits) */ else if (isdigit(source[*pos]) || (source[*pos] == '.' && isdigit(source[*pos + 1]))) {
        bool decimal = false;
        while (length > *pos && isdigit(source[*pos]))
            (*pos)++;

        char* i_part_str = ExtractString(source, startPos, *pos - startPos);

        /* if next char is a . and that is followed by a digit this is a decimal number */
        int decimal_start = -1;
        if (length > *pos && source[*pos] == '.') {
            (*pos)++;

            // Ensure next char exists and is digit.
            if (length > *pos && isdigit(source[*pos])) {
                decimal = true;
                decimal_start = *pos;
                while (length > *pos && isdigit(source[*pos]))
                    (*pos)++;
            }
        }

        char* end;
        uint32_t i_part = strtoul(i_part_str, &end, 10);
        free(i_part_str);

        if (decimal) {
            if (i_part >= 65536) {
                char* fullNum = ExtractString(source, startPos, *pos - startPos);
                char* posStr = FindFilePositionText(source, *pos);
                char* error = StringBuilder_Format("Decimal value %s at %s was too large. (Maximum: 65535.99999)\n", fullNum, posStr);
                free(posStr);
                free(fullNum);
                return (SCRIPT_TOKEN*)MakeStringToken(TOKEN_ERROR, error);
            }

            char* f_part_str = ExtractString(source, decimal_start, *pos - decimal_start);
            uint32_t f_part = strtoul(f_part_str, &end, 10);
            double flt_pt = (double)f_part;
            for (size_t i = 0; i < strlen(f_part_str); i++)
                flt_pt /= 10;
            
            f_part = (uint32_t)floor(flt_pt * 65536);
            free(f_part_str);
            return (SCRIPT_TOKEN*)MakeTokenValue32(TOKEN_DECIMAL, (i_part << 16) | f_part);
        } else {
            return (SCRIPT_TOKEN*)MakeTokenValue32(TOKEN_INTEGER, i_part);
        }
    }

    /* handle string token */ else if ((source[*pos] == '"')) {
        bool hasMore = false;
        (*pos)++;
        while (length > *pos && source[*pos] != '\n' && source[*pos] != '"')
            (*pos)++;

        char* value = ExtractString(source, startPos + 1, *pos - startPos - 1);

        if (source[*pos] == '"') {
            (*pos)++;
        } else {
            char* posStr = FindFilePositionText(source, *pos);
            char* error = StringBuilder_Format("String starting at %s was not closed: '%s'\n", posStr, value);
            free(posStr);
            return (SCRIPT_TOKEN*)MakeStringToken(TOKEN_ERROR, error);
        }

        return (SCRIPT_TOKEN*)MakeStringToken(TOKEN_STRING, value);
    } else if (source[*pos] == '(') {
        (*pos)++;
        return MakeBasicToken(TOKEN_OPEN_PARENTHESIS);
    } else if (source[*pos] == ')') {
        (*pos)++;
        return MakeBasicToken(TOKEN_CLOSE_PARENTHESIS);
    } else if (source[*pos] == ',') {
        (*pos)++;
        return MakeBasicToken(TOKEN_COMMA);
    } else if (source[*pos] == '{') {
        (*pos)++;
        return MakeBasicToken(TOKEN_OPEN_BRACKET);
    } else if (source[*pos] == '}') {
        (*pos)++;
        return MakeBasicToken(TOKEN_CLOSE_BRACKET);
    } else if (source[*pos] == '#') {
        (*pos)++;
        return MakeBasicToken(TOKEN_POUND);
    }

    /* handle unrecognized tokens */ else {
        char* posStr = FindFilePositionText(source, *pos);
        char* error = StringBuilder_Format("Unrecognized token type, starting with '%c' at %s.", source[*pos], posStr);
        free(posStr);
        return (SCRIPT_TOKEN*)MakeStringToken(TOKEN_ERROR, error);
    }
}

// Frees the token, and all subsequent tokens.
void FreeTokens(SCRIPT_TOKEN* head) {
    SCRIPT_TOKEN* next;

    while (head) {
        next = head->next;

        // Frees the token value.
        if (head->type == TOKEN_ERROR || head->type == TOKEN_STRING || head->type == TOKEN_IDENTIFIER)
            StringRef_FreeUsage(((SCRIPT_TOKEN_STRING*)head)->ref);

        free(head);
        head = next;
    }
}

SCRIPT_TOKEN* CloneToken(const SCRIPT_TOKEN* token, const SCRIPT_TOKEN_LOCATION* location) {
    SCRIPT_TOKEN* newToken = nullptr;

    switch (token->type) {
    case TOKEN_EOF:
    case TOKEN_COMMA:
    case TOKEN_OPEN_PARENTHESIS:
    case TOKEN_CLOSE_PARENTHESIS:
    case TOKEN_OPEN_BRACKET:
    case TOKEN_CLOSE_BRACKET:
    case TOKEN_POUND:
        newToken = MakeBasicToken(token->type);
        break;
    case TOKEN_ERROR:
    case TOKEN_IDENTIFIER:
    case TOKEN_STRING:
        newToken = (SCRIPT_TOKEN*)MakeStringRefToken(token->type, ((SCRIPT_TOKEN_STRING*)token)->ref);
        break;
    case TOKEN_INTEGER:
    case TOKEN_DECIMAL:
        newToken = (SCRIPT_TOKEN*)MakeTokenValue32(token->type, ((SCRIPT_TOKEN_VALUE32*)token)->value);
        break;
    default:
        printf("Failed to clone unsupported token '%s'.\n", GetTokenName(token->type));
        exit(1);
    }

    newToken->location = *location;
    return newToken;
}

char* g_TokenMnemonics[] = {
    nameof(TOKEN_EOF),
    nameof(TOKEN_ERROR),
    nameof(TOKEN_INTEGER),
    nameof(TOKEN_DECIMAL),
    nameof(TOKEN_STRING),
    nameof(TOKEN_IDENTIFIER),
    nameof(TOKEN_COMMA),
    nameof(TOKEN_OPEN_PARENTHESIS),
    nameof(TOKEN_CLOSE_PARENTHESIS),
    nameof(TOKEN_OPEN_BRACKET),
    nameof(TOKEN_CLOSE_BRACKET),
    nameof(TOKEN_POUND)
};

char* GetTokenName(SCRIPT_TOKEN_TYPE type) {
    return g_TokenMnemonics[type];
}

char* TokenToString(const SCRIPT_TOKEN* token) {
    STRING_BUILDER* builder = StringBuilder_Create(32);

    if (!token) {
        StringBuilder_AppendString(builder, "<NULL_TOKEN>");
        return StringBuilder_FreeKeepResult(builder);
    }

    switch (token->type) {
    case TOKEN_EOF:
    case TOKEN_COMMA:
    case TOKEN_OPEN_PARENTHESIS:
    case TOKEN_CLOSE_PARENTHESIS:
    case TOKEN_OPEN_BRACKET:
    case TOKEN_CLOSE_BRACKET:
    case TOKEN_POUND:
        StringBuilder_AppendString(builder, GetTokenName(token->type));
        break;
    case TOKEN_ERROR:
    case TOKEN_IDENTIFIER:
    case TOKEN_STRING:
        StringBuilder_Append(builder, "%s[value=\"%s\"]", GetTokenName(token->type), ((SCRIPT_TOKEN_STRING*)token)->ref->value);
        break;
    case TOKEN_INTEGER:
        StringBuilder_Append(builder, "%s[value=%lu]", GetTokenName(token->type), ((SCRIPT_TOKEN_VALUE32*)token)->value);
        break;
    case TOKEN_DECIMAL:
        StringBuilder_Append(builder, "%s[value=", GetTokenName(token->type));
        FixedPointU16ToStringBuilder(builder, ((SCRIPT_TOKEN_VALUE32*)token)->value);
        StringBuilder_AppendChar(builder, ']');
        break;
    default:
        printf("Failed to write string for unsupported token '%s'.\n", GetTokenName(token->type));
        exit(1);
    }

    return StringBuilder_FreeKeepResult(builder);
}

void DebugPrintTokens(SCRIPT_TOKEN* root) {
    SCRIPT_TOKEN* temp = root;
    while (temp) {
        char* message = TokenToString(temp);
        printf(" - %s\n", message);
        free(message);
        temp = temp->next;
    }
}