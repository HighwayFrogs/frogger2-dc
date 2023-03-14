#pragma once

typedef enum __ast_node_type AST_NODE_TYPE;
typedef struct __ast_node_statement AST_NODE_STATEMENT;
typedef struct __ast_statement_list_node AST_STATEMENT_LIST_NODE;
typedef struct __ast_node_block AST_NODE_BLOCK;
typedef struct __ast_node_template AST_NODE_TEMPLATE;
typedef struct __ast_node_string AST_NODE_STRING;
typedef struct __ast_node_byte AST_NODE_BYTE;
typedef struct __ast_node_short AST_NODE_SHORT;
typedef struct __ast_node_integer AST_NODE_INTEGER;
typedef struct __ast_node_decimal AST_NODE_DECIMAL;

#include "main.h"
#include "commands.h"
#include "token.h"
#include <stdarg.h>
#include <stdio.h>
#include "preproc.h"


typedef enum __ast_node_type {
	AST_NODE_TYPE_EVENT, // Command
	AST_NODE_TYPE_TRIGGER,
	AST_NODE_TYPE_BLOCK,
	AST_NODE_TYPE_STRING,
	AST_NODE_TYPE_BYTE,
	AST_NODE_TYPE_SHORT,
	AST_NODE_TYPE_INTEGER,
	AST_NODE_TYPE_DECIMAL
} AST_NODE_TYPE;

typedef struct __ast_node {
	AST_NODE_TYPE type;
	SCRIPT_PREPROCESSOR_MACRO* macro;
} AST_NODE;

typedef struct __ast_node_statement {
	AST_NODE_TYPE type;
	SCRIPT_PREPROCESSOR_MACRO* macro;
	REGISTERED_COMMAND* command;
	AST_NODE** arguments;
} AST_NODE_STATEMENT;

typedef struct __ast_statement_list_node {
	AST_NODE_STATEMENT* statement;
	AST_STATEMENT_LIST_NODE* next;
} AST_STATEMENT_LIST_NODE;

typedef struct __ast_node_block {
	AST_NODE_TYPE type;
	SCRIPT_PREPROCESSOR_MACRO* macro;
	uint16_t statement_count;
	AST_STATEMENT_LIST_NODE* statements;
} AST_NODE_BLOCK;

typedef struct __ast_node_string {
	AST_NODE_TYPE type;
	SCRIPT_PREPROCESSOR_MACRO* macro;
	STRING_REFERENCE* ref;
} AST_NODE_STRING;

typedef struct __ast_node_byte {
	AST_NODE_TYPE type;
	SCRIPT_PREPROCESSOR_MACRO* macro;
	uint8_t value;
} AST_NODE_BYTE;

typedef struct __ast_node_short {
	AST_NODE_TYPE type;
	SCRIPT_PREPROCESSOR_MACRO* macro;
	uint16_t value;
} AST_NODE_SHORT;

typedef struct __ast_node_integer {
	AST_NODE_TYPE type;
	SCRIPT_PREPROCESSOR_MACRO* macro;
	uint32_t value;
} AST_NODE_INTEGER;

typedef struct __ast_node_decimal {
	AST_NODE_TYPE type;
	SCRIPT_PREPROCESSOR_MACRO* macro;
	uint32_t value;
} AST_NODE_DECIMAL;


// Builds the AST from provided tokens.
extern AST_NODE_BLOCK* BuildTreeFromTokens(SCRIPT_TOKEN *token);
extern char* Decompile(AST_NODE* node);
extern void FreeAST(AST_NODE* node);
extern char* GetNodeTypeName(AST_NODE_TYPE type);
extern void WriteASTNode(const AST_NODE* node, FILE* file);
extern AST_NODE_BLOCK* LoadCompiledScript(FILE* file);
extern void SaveCompiledScript(const AST_NODE_BLOCK* root, FILE* file);
extern void DebugPrintAST(AST_NODE* root);