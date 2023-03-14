#pragma once

// This file contains the token definitions.
typedef enum __script_token_type SCRIPT_TOKEN_TYPE;
typedef struct __script_token SCRIPT_TOKEN;
typedef struct __script_token_location SCRIPT_TOKEN_LOCATION;

#include "main.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>

typedef enum __script_token_type {
	TOKEN_EOF,
	TOKEN_ERROR,
	TOKEN_INTEGER,
	TOKEN_DECIMAL,
	TOKEN_STRING,
	TOKEN_IDENTIFIER,
	TOKEN_COMMA,
	TOKEN_OPEN_PARENTHESIS,
	TOKEN_CLOSE_PARENTHESIS,
	TOKEN_OPEN_BRACKET,
	TOKEN_CLOSE_BRACKET,
	TOKEN_POUND
} SCRIPT_TOKEN_TYPE;

typedef struct __script_token_location {
	uint32_t line;
	uint32_t column;
} SCRIPT_TOKEN_LOCATION;

typedef struct __script_token {
	SCRIPT_TOKEN_TYPE type;
	SCRIPT_TOKEN_LOCATION location;
	SCRIPT_TOKEN *next;
} SCRIPT_TOKEN;

typedef struct __script_token_value32 {
	SCRIPT_TOKEN_TYPE type;
	SCRIPT_TOKEN_LOCATION location;
	SCRIPT_TOKEN* next;
	uint32_t value;
} SCRIPT_TOKEN_VALUE32;

typedef struct __script_token_string {
	SCRIPT_TOKEN_TYPE type;
	SCRIPT_TOKEN_LOCATION location;
	SCRIPT_TOKEN* next;
	STRING_REFERENCE* ref;
} SCRIPT_TOKEN_STRING;

extern char* GetLocationText(const SCRIPT_TOKEN_LOCATION* location);
extern bool IsValidIdentifier(char c, bool isFirstChar);

extern SCRIPT_TOKEN* MakeBasicToken(SCRIPT_TOKEN_TYPE type);
extern SCRIPT_TOKEN_VALUE32* MakeTokenValue32(SCRIPT_TOKEN_TYPE type, uint32_t value);
extern SCRIPT_TOKEN_STRING* MakeStringToken(SCRIPT_TOKEN_TYPE type, char* str);

extern SCRIPT_TOKEN* Tokenize(const char source[]);
extern SCRIPT_TOKEN* CloneToken(const SCRIPT_TOKEN* token, const SCRIPT_TOKEN_LOCATION* location);
extern void FreeTokens(SCRIPT_TOKEN* head);
extern char* TokenToString(const SCRIPT_TOKEN* token);
extern char* GetTokenName(SCRIPT_TOKEN_TYPE type);
extern void DebugPrintTokens(SCRIPT_TOKEN* root);