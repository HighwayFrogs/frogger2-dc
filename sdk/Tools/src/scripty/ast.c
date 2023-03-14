#include "ast.h"
#include "preproc.h"
#include "utils.h"

void token_error(const SCRIPT_TOKEN* token, const char* format, ...) {
	va_list args;
	va_start(args, format);

	printf("\n");
	if (token) {
		printf("Syntax Error at ");
		printf("line %d, col %d...", token->location.line, token->location.column);
		printf("\n");
	} else {
		printf("Syntax Error:\n");
	}
	
	printf("  ");
	vprintf(format, args);
	printf("\n");
	printf("\n");
	exit(1);

	va_end(args);
}

void ast_error(const char* format, ...) {
	va_list args;
	va_start(args, format);
	token_error(nullptr, format, args);
	va_end(args);
}

void RequireTokenType(const SCRIPT_TOKEN* token, SCRIPT_TOKEN_TYPE expectedType) {
	if (!token || token->type != expectedType) {
		char* tokenAsString = TokenToString(token);
		token_error(token, "Expected a '%s' token, but got '%s' instead.", GetTokenName(expectedType), tokenAsString);
		free(tokenAsString);
	}
}

void* ast_talloc(const SCRIPT_TOKEN* token, int size) {
	void* data = malloc(size);
	if (!data)
		token_error(token, "AST Allocation of %d bytes failed.", size);
	return data;
}

void* ast_alloc(int size) {
	void* data = malloc(size);
	if (!data)
		ast_error("AST Allocation of %d bytes failed.", size);
	return data;
}

SCRIPT_TOKEN* GetNextToken(const SCRIPT_TOKEN* token) {
	if (!token || !token->next) {
		token_error(token, "Unexpected end of file.");
		return nullptr;
	}

	return token->next;
}

AST_NODE_TYPE GetASTNodeTypeFromCommandType(const SCRIPT_TOKEN* token, COMMAND_TYPE type) {
	switch (type) {
	case COMMAND_TYPE_EVENT:
		return AST_NODE_TYPE_EVENT;
	case COMMAND_TYPE_TRIGGER:
		return AST_NODE_TYPE_TRIGGER;
	default:
		token_error(token, "The provided COMMAND_TYPE %d is not supported by the AST builder.", type);
		return -1;
	}
}

SCRIPT_TOKEN* ParseASTBlock(SCRIPT_TOKEN* token, bool bracesEnabled, AST_NODE_BLOCK** node);

SCRIPT_TOKEN* ParseASTStatement(SCRIPT_TOKEN* token, COMMAND_TYPE type, AST_NODE_STATEMENT** node) {
	SCRIPT_TOKEN* startToken = token;

	AST_NODE_STATEMENT* statement = ast_talloc(token, sizeof(AST_NODE_STATEMENT));
	statement->type = GetASTNodeTypeFromCommandType(token, type);

	RequireTokenType(token, TOKEN_IDENTIFIER);
	char* commandName = ((SCRIPT_TOKEN_STRING*)token)->ref->value;
	REGISTERED_COMMAND* command = FindCommandByName(commandName, type);
	token = GetNextToken(token);

	if (!command) {
		token_error(token, "'%s' is not a recognized function name.", commandName);
		return nullptr; // Adding 'return' fixes warning from lines below about null deref.
	}

	statement->command = command;
	statement->arguments = ast_talloc(token, command->argument_count * sizeof(AST_NODE*));

	bool hasParenthesis = token->type == TOKEN_OPEN_PARENTHESIS;
	if (hasParenthesis)
		token = GetNextToken(token);

#ifdef _DEBUG
	char* token_str = TokenToString(token);
	debug_printf("Parsing AST Statement (Command: %s), first argument token: '%s'.\n", command->mnemonic, token_str);
	free(token_str);
#endif

	// Read arguments.
	int i;
	for (i = 0; i < command->argument_count && token->type != TOKEN_CLOSE_PARENTHESIS; i++) {
		AST_NODE_TYPE node_type = CommandGetArgumentType(&command->argument_types[i]);
		void* argument = CommandGetArgumentData(&command->argument_types[i]);

#ifdef _DEBUG
		token_str = TokenToString(token);
		debug_printf("AST Statement[%s]: Loading Argument %d as '%s' from token '%s'.\n", command->mnemonic, i, GetNodeTypeName(node_type), token_str);
		free(token_str);
#endif

		uint32_t value;
		switch (node_type) {
		case AST_NODE_TYPE_EVENT:
			RequireTokenType(token, TOKEN_IDENTIFIER);
			AST_NODE_STATEMENT* event_node = nullptr;
			token = ParseASTStatement(token, COMMAND_TYPE_EVENT, &event_node);
			statement->arguments[i] = (AST_NODE*)event_node;
			break;
		case AST_NODE_TYPE_TRIGGER:
			RequireTokenType(token, TOKEN_IDENTIFIER);
			AST_NODE_STATEMENT* trigger_node = nullptr;
			token = ParseASTStatement(token, COMMAND_TYPE_TRIGGER, &trigger_node);
			statement->arguments[i] = (AST_NODE*)trigger_node;
			break;
		case AST_NODE_TYPE_BLOCK:
			;
			AST_NODE_BLOCK* block_node = nullptr;
			token = ParseASTBlock(token, true, &block_node);
			statement->arguments[i] = (AST_NODE*)block_node;
			break;
		case AST_NODE_TYPE_STRING:
			RequireTokenType(token, TOKEN_STRING);
			AST_NODE_STRING* str_node = ast_talloc(token, sizeof(AST_NODE_STRING));
			str_node->type = AST_NODE_TYPE_STRING;
			str_node->macro = nullptr;

			STRING_REFERENCE* ref = ((SCRIPT_TOKEN_STRING*)token)->ref;
			uint8_t max_str_length = argument ? ((COMMAND_ARGUMENT_STRING_DATA*)argument)->max_length : 255;
			if (strlen(ref->value) > max_str_length)
				token_error(token, "The string is too large, having %d characters where only %d are allowed.", strlen(ref->value), max_str_length);

			str_node->ref = StringRef_AddUsage(ref);
			statement->arguments[i] = (AST_NODE*)str_node;
			token = GetNextToken(token);
			break;
		case AST_NODE_TYPE_BYTE:
			RequireTokenType(token, TOKEN_INTEGER);
			AST_NODE_BYTE* b_node = ast_talloc(token, sizeof(AST_NODE_BYTE));
			b_node->type = AST_NODE_TYPE_BYTE;
			b_node->macro = nullptr;
			value = ((SCRIPT_TOKEN_VALUE32*)token)->value;

			// Verify OK.
			COMMAND_ARGUMENT_BYTE_DATA* arg_byte = (COMMAND_ARGUMENT_BYTE_DATA*)argument;
			if (arg_byte && value < arg_byte->min_value)
				token_error(token, "The value is too small. (Value: %u, Minimum: %u)", value, arg_byte->min_value);

			uint8_t max_value8 = arg_byte ? arg_byte->max_value : 255;
			if (value > max_value8)
				token_error(token, "The value is too large. (Value: %u, Max Allowed: %u)", value, max_value8);

			b_node->value = (uint8_t)value;
			statement->arguments[i] = (AST_NODE*)b_node;
			token = GetNextToken(token);
			break;
		case AST_NODE_TYPE_SHORT:
			RequireTokenType(token, TOKEN_INTEGER);
			AST_NODE_SHORT* s_node = ast_talloc(token, sizeof(AST_NODE_SHORT));
			s_node->type = AST_NODE_TYPE_SHORT;
			s_node->macro = nullptr;
			value = ((SCRIPT_TOKEN_VALUE32*)token)->value;

			// Verify OK.
			COMMAND_ARGUMENT_SHORT_DATA* arg_short = (COMMAND_ARGUMENT_SHORT_DATA*)argument;
			if (arg_short && value < arg_short->min_value)
				token_error(token, "The value is too small. (Value: %u, Minimum: %u)", value, arg_short->min_value);

			uint16_t max_value16 = arg_short ? arg_short->max_value : 65535;
			if (value > max_value16)
				token_error(token, "The value is too large. (Value: %u, Max Allowed: %u)", value, max_value16);

			s_node->value = (uint16_t)value;
			statement->arguments[i] = (AST_NODE*)s_node;
			token = GetNextToken(token);
			break;
		case AST_NODE_TYPE_INTEGER:
			RequireTokenType(token, TOKEN_INTEGER);
			AST_NODE_INTEGER* i_node = ast_talloc(token, sizeof(AST_NODE_INTEGER));
			i_node->type = AST_NODE_TYPE_INTEGER;
			i_node->macro = nullptr;
			i_node->value = ((SCRIPT_TOKEN_VALUE32*)token)->value;
			statement->arguments[i] = (AST_NODE*)i_node;
			token = GetNextToken(token);
			break;
		case AST_NODE_TYPE_DECIMAL:
			;
			AST_NODE_DECIMAL* d_node = ast_talloc(token, sizeof(AST_NODE_DECIMAL));
			d_node->type = AST_NODE_TYPE_DECIMAL;
			d_node->macro = nullptr;

			if (token->type == TOKEN_INTEGER) {
				value = ((SCRIPT_TOKEN_VALUE32*)token)->value;
				if (value >= 65536)
					token_error(token, "The value %d is used as a fixed point decimal, which can only represent integer values up to 65535.", value);

				d_node->value = value << 16;
			} else if (token->type == TOKEN_DECIMAL) {
				d_node->value = ((SCRIPT_TOKEN_VALUE32*)token)->value;
			} else {
				token_error(token, "Don't know how to create the '%s' node by creating from %s.", GetNodeTypeName(node_type), GetTokenName(token->type));
			}
			statement->arguments[i] = (AST_NODE*)d_node;
			token = GetNextToken(token);
			break;
		default:
			token_error(token, "Don't know how to create the '%s' node by creating from the '%s' token.", GetNodeTypeName(node_type), GetTokenName(token->type));
		}

		if (token->type == TOKEN_COMMA)
			token = GetNextToken(token); else if (hasParenthesis && token->type != TOKEN_CLOSE_PARENTHESIS)
			token_error(token, "Expected another argument, but got '%s' instead.", GetTokenName(token->type));
	}

	// Verify the correct number of arguments are present.
	if (i != command->argument_count)
		token_error(startToken, "Function '%s' takes %d arguments, but got %d.", command->mnemonic, command->argument_count, i);

	// Verify closing parenthesis are present.
	if (hasParenthesis) {
		RequireTokenType(token, TOKEN_CLOSE_PARENTHESIS);
		token = GetNextToken(token);
	}

	// Done.
	*node = statement;
	return token;
}

SCRIPT_TOKEN* ParseASTBlock(SCRIPT_TOKEN* token, bool bracesEnabled, AST_NODE_BLOCK** node) {
	AST_NODE_BLOCK* root = ast_talloc(token, sizeof(AST_NODE_BLOCK));
	root->type = AST_NODE_TYPE_BLOCK;
	root->statement_count = 0;
	root->macro = nullptr;

	// Braces are enabled, but the next token isn't a brace, so we only read a single statement.
	AST_NODE_STATEMENT* statement;
	AST_STATEMENT_LIST_NODE* statement_node = nullptr;
	if (bracesEnabled) {
		if (token->type != TOKEN_OPEN_BRACKET) {
			while (token) {
				token = ParseASTStatement(token, COMMAND_TYPE_EVENT, &statement);
				statement_node = ast_talloc(token, sizeof(AST_STATEMENT_LIST_NODE));
				statement_node->statement = statement;
				statement_node->next = nullptr;
				root->statement_count = 1;
				root->statements = statement_node;
				*node = root;
				return token;
			}
		} else {
			token = GetNextToken(token);
		}
	}

	// Read statements.
	AST_STATEMENT_LIST_NODE* last_statement_node = nullptr;
	while (token && token->type != TOKEN_EOF) {
		if (bracesEnabled && token->type == TOKEN_CLOSE_BRACKET) {
			token = GetNextToken(token);
			break;
		}

		token = ParseASTStatement(token, COMMAND_TYPE_EVENT, &statement);
		statement_node = ast_talloc(token, sizeof(AST_STATEMENT_LIST_NODE));
		statement_node->statement = statement;
		statement_node->next = nullptr;

		if (last_statement_node) {
			last_statement_node->next = statement_node;
		} else { // Set head node.
			root->statements = statement_node;
		}

		root->statement_count++;
		last_statement_node = statement_node;
	}

	*node = root;
	return token;
}

AST_NODE_BLOCK* BuildTreeFromTokens(SCRIPT_TOKEN* token) {
	AST_NODE_BLOCK* result = nullptr;
	ParseASTBlock(token, false, &result);
	return result;
}

char* g_NodeTypeNames[] = {
	nameof(AST_NODE_TYPE_EVENT),
	nameof(AST_NODE_TYPE_TRIGGER),
	nameof(AST_NODE_TYPE_BLOCK),
	nameof(AST_NODE_TYPE_STRING),
	nameof(AST_NODE_TYPE_BYTE),
	nameof(AST_NODE_TYPE_SHORT),
	nameof(AST_NODE_TYPE_INTEGER),
	nameof(AST_NODE_TYPE_DECIMAL)
};

char* GetNodeTypeName(AST_NODE_TYPE type) {
	return g_NodeTypeNames[type];
}

bool ShouldNodeUseBraces(const AST_NODE_BLOCK* node) {
	if (node->statement_count != 1)
		return true; // We can only skip braces when there's a single line.

	AST_NODE_STATEMENT* nested = node->statements->statement;
	for (int i = 0; i < nested->command->argument_count; i++) {
		AST_NODE_TYPE node_type = CommandGetArgumentType(&nested->command->argument_types[i]);
		if (node_type == AST_NODE_TYPE_BLOCK)
			return ShouldNodeUseBraces((AST_NODE_BLOCK*)nested->arguments[i]);
	}

	return false;
}

// Includes AST IO operations.
void GenerateMacrosFromAST(MACRO_BANK* bank, const AST_NODE* node) {
	debug_printf("GenerateMacrosFromAST - Current node is '%s'.\n", GetNodeTypeName(node->type));
	switch (node->type) {
	case AST_NODE_TYPE_EVENT:
	case AST_NODE_TYPE_TRIGGER:
		;
		AST_NODE_STATEMENT* statement = (AST_NODE_STATEMENT*)node;
		for (int i = 0; i < statement->command->argument_count; i++) {
			COMMAND_ARGUMENT* argument = &statement->command->argument_types[i];
			void* argument_data = CommandGetArgumentData(argument);

			char* macroName = nullptr;
			SCRIPT_TOKEN* token = nullptr;
			AST_NODE_TYPE type = CommandGetArgumentType(argument);
			if (type == AST_NODE_TYPE_BYTE) {
				COMMAND_ARGUMENT_BYTE_DATA* data = (COMMAND_ARGUMENT_BYTE_DATA*)argument_data;
				if (!data || !data->get_macro_name)
					continue;

				STRING_BUILDER* builder = StringBuilder_Create(32);
				data->get_macro_name(builder, ((AST_NODE_BYTE*)statement->arguments[i]));
				macroName = StringBuilder_FreeKeepResult(builder);
				if (IsValidIdentifier(*macroName, true))
					token = (SCRIPT_TOKEN*)MakeTokenValue32(TOKEN_INTEGER, ((AST_NODE_BYTE*)statement->arguments[i])->value);
			} else if (type == AST_NODE_TYPE_SHORT) {
				COMMAND_ARGUMENT_SHORT_DATA* data = (COMMAND_ARGUMENT_SHORT_DATA*)argument_data;
				if (!data || !data->get_macro_name)
					continue;

				STRING_BUILDER* builder = StringBuilder_Create(32);
				data->get_macro_name(builder, ((AST_NODE_SHORT*)statement->arguments[i]));
				macroName = StringBuilder_FreeKeepResult(builder);
				if (IsValidIdentifier(*macroName, true))
					token = (SCRIPT_TOKEN*)MakeTokenValue32(TOKEN_INTEGER, ((AST_NODE_SHORT*)statement->arguments[i])->value);
			} else if (type == AST_NODE_TYPE_INTEGER) {
				COMMAND_ARGUMENT_INTEGER_DATA* data = (COMMAND_ARGUMENT_INTEGER_DATA*)argument_data;
				if (!data || !data->get_macro_name)
					continue;

				STRING_BUILDER* builder = StringBuilder_Create(32);
				data->get_macro_name(builder, ((AST_NODE_INTEGER*)statement->arguments[i]));
				macroName = StringBuilder_FreeKeepResult(builder);
				if (IsValidIdentifier(*macroName, true))
					token = (SCRIPT_TOKEN*)MakeTokenValue32(TOKEN_INTEGER, ((AST_NODE_INTEGER*)statement->arguments[i])->value);
			} else {
				GenerateMacrosFromAST(bank, (AST_NODE*)statement->arguments[i]);
			}

			if (macroName && token) {
				SCRIPT_PREPROCESSOR_MACRO* macro = FindMacroByName(bank, macroName);
				if (macro) {
					free(macroName);
					FreeTokens(token);
				} else {
					macro = ast_alloc(sizeof(SCRIPT_PREPROCESSOR_MACRO));
					macro->next = nullptr;
					macro->name = StringRef_Create(macroName);
					macro->token = token;
					macro->location.line = -1;
					macro->location.column = -1;
					macro->builtin = false;
					macro->usages = 0;
					RegisterMacro(bank, macro);
					debug_printf("Created decompile output macro named '%s'.\n", macroName);
				}

				macro->usages++;
				statement->arguments[i]->macro = macro;
			} else {
				free(macroName);
				FreeTokens(token);
			}
		}

		break;
	case AST_NODE_TYPE_BLOCK:
		;
		// Write statements.
		AST_STATEMENT_LIST_NODE* statement_node = ((AST_NODE_BLOCK*)node)->statements;
		while (statement_node) {
			GenerateMacrosFromAST(bank, (AST_NODE*)statement_node->statement);
			statement_node = statement_node->next;
		}
		break;
	case AST_NODE_TYPE_STRING:
	case AST_NODE_TYPE_BYTE:
	case AST_NODE_TYPE_SHORT:
	case AST_NODE_TYPE_INTEGER:
	case AST_NODE_TYPE_DECIMAL:
		break;
	default:
		ast_error("Couldn't perform macro creation for unsupported AST node type '%s'.", GetNodeTypeName(node->type));
		break;
	}
}

// ToString()
void AST_NodeToString(STRING_BUILDER* builder, AST_NODE* node, const void* argument_data, char* padding, bool isRootNode) {
	if (node == nullptr) {
		StringBuilder_AppendString(builder, "null");
		return;
	}

	if (node->macro && (node->macro->usages >= MINIMUM_DECOMPILER_MACRO_USAGES || node->macro->builtin)) {
		StringBuilder_AppendString(builder, node->macro->name->value);
		node->macro = nullptr; // Unlinking the macro from the node cleans it up after usage.
		return;
	}

	switch (node->type) {
	case AST_NODE_TYPE_EVENT:
	case AST_NODE_TYPE_TRIGGER:
		;
		AST_NODE_STATEMENT* statement_node = ((AST_NODE_STATEMENT*)node);
		REGISTERED_COMMAND* command = statement_node->command;

		StringBuilder_AppendString(builder, command->mnemonic);

		bool showParenthesis = AreParenthesisShownForCommand(command);
		StringBuilder_AppendString(builder, showParenthesis ? "(" : " ");

		for (int i = 0; i < command->argument_count; i++) {
			if (i > 0)
				StringBuilder_AppendString(builder, showParenthesis ? ", " : " ");
			AST_NodeToString(builder, statement_node->arguments[i], CommandGetArgumentData(&command->argument_types[i]), padding, false);
		}

		if (showParenthesis)
			StringBuilder_AppendString(builder, ")");

		break;
	case AST_NODE_TYPE_BLOCK:
		; // Semicolon required for the following statement to work...?
		bool braces = !isRootNode && ShouldNodeUseBraces((AST_NODE_BLOCK*)node);

		char* new_padding = padding;
		if (!isRootNode) {
			new_padding = AppendString(padding, "  ");
			StringBuilder_AppendString(builder, braces ? "{\n" : "\n");
		}

		AST_STATEMENT_LIST_NODE *last_node = nullptr;
		AST_STATEMENT_LIST_NODE *curr_node = ((AST_NODE_BLOCK*)node)->statements;
		while (curr_node) {
			if (last_node && IsSpaceShownForCommand(curr_node->statement->command) && !IsSpaceShownForCommand(last_node->statement->command)) {
				StringBuilder_AppendString(builder, new_padding);
				StringBuilder_AppendString(builder, "\n");
			}

			StringBuilder_AppendString(builder, new_padding);
			AST_NodeToString(builder, (AST_NODE*)curr_node->statement, nullptr, new_padding, false);

			if (curr_node->next) {
				StringBuilder_AppendString(builder, "\n");
				if (IsSpaceShownForCommand(curr_node->statement->command)) {
					StringBuilder_AppendString(builder, new_padding);
					StringBuilder_AppendString(builder, "\n");
				}
			}

			last_node = curr_node;
			curr_node = curr_node->next;
		}

		if (braces) {
			StringBuilder_AppendString(builder, "\n");
			StringBuilder_AppendString(builder, padding);
			StringBuilder_AppendString(builder, "}");
		}

		if (!isRootNode)
			free(new_padding);
		break;
	case AST_NODE_TYPE_STRING:
		StringBuilder_Append(builder, "\"%s\"", ((AST_NODE_STRING*)node)->ref->value);
		break;
	case AST_NODE_TYPE_BYTE:
		if (argument_data && ((COMMAND_ARGUMENT_BYTE_DATA*)argument_data)->to_string)
			((COMMAND_ARGUMENT_BYTE_DATA*)argument_data)->to_string(builder, ((AST_NODE_BYTE*)node));
		else
			AstNodeByte_ToString(builder, (AST_NODE_BYTE*)node);
		break;
	case AST_NODE_TYPE_SHORT:
		if (argument_data && ((COMMAND_ARGUMENT_SHORT_DATA*)argument_data)->to_string)
			((COMMAND_ARGUMENT_SHORT_DATA*)argument_data)->to_string(builder, ((AST_NODE_SHORT*)node));
		else
			AstNodeShort_ToString(builder, (AST_NODE_SHORT*)node);
		break;
	case AST_NODE_TYPE_INTEGER:
		if (argument_data && ((COMMAND_ARGUMENT_INTEGER_DATA*)argument_data)->to_string)
			((COMMAND_ARGUMENT_INTEGER_DATA*)argument_data)->to_string(builder, ((AST_NODE_INTEGER*)node));
		else
			AstNodeInteger_ToString(builder, (AST_NODE_INTEGER*)node);
		break;
	case AST_NODE_TYPE_DECIMAL:
		FixedPointU16ToStringBuilder(builder, ((AST_NODE_DECIMAL*)node)->value);
		break;
	default:
		StringBuilder_AppendString(builder, "ERROR UNKNOWN AST NODE TYPE");
	}
}

char* Decompile(AST_NODE* node) {
	STRING_BUILDER* builder = StringBuilder_Create(128);
	if (!builder) {
		printf("Failed to allocate STRING_BUILDER* for script decompiler.\n");
		return nullptr;
	}

	MACRO_BANK* bank = CreateMacroBank();
	GenerateMacrosFromAST(bank, node);

	// Write macros.
	if (bank->list_head) {
		SCRIPT_PREPROCESSOR_MACRO* macro = bank->list_head;
		bool wroteAny = false;
		while (macro) {
			if (!macro->builtin && macro->usages >= MINIMUM_DECOMPILER_MACRO_USAGES) {
				StringBuilder_AppendString(builder, "#define ");
				StringBuilder_AppendString(builder, macro->name->value);
				StringBuilder_Append(builder, " %u\n", ((SCRIPT_TOKEN_VALUE32*)macro->token)->value);
				wroteAny = true;
			}

			macro = macro->next;
		}

		if (wroteAny)
			StringBuilder_AppendString(builder, "\n");
	}

	// Write nodes.
	AST_NodeToString(builder, node, nullptr, "", true);

	// Cleanup macros. (Must occur after doing ToString()).
	FreeMacroBank(bank);

	// Done:
	StringBuilder_AppendString(builder, "\n");
	return StringBuilder_FreeKeepResult(builder);
}

AST_NODE* ReadASTNode(AST_NODE_TYPE type, FILE* file) {
	debug_printf("Reading AST Node of type '%s'.\n", GetNodeTypeName(type));

	switch (type) {
	case AST_NODE_TYPE_EVENT:
	case AST_NODE_TYPE_TRIGGER:
		;
		uint8_t opcode = FileReadUInt8(file);
		REGISTERED_COMMAND* command = FindCommandByOpcode(opcode, GetCommandTypeFromASTNodeType(type));
		if (!command) {
			ast_error("Failed to load command with unknown opcode %d of type %s.", opcode, GetNodeTypeName(type));
			return nullptr;
		}

		AST_NODE_STATEMENT* statement = ast_alloc(sizeof(AST_NODE_STATEMENT));
		statement->macro = nullptr;
		statement->type = type;
		statement->macro = nullptr;
		statement->command = command;

		statement->arguments = ast_alloc(command->argument_count * sizeof(AST_NODE*));;
		for (int i = 0; i < command->argument_count; i++) {
			AST_NODE_TYPE argument_type = CommandGetArgumentType(&command->argument_types[i]);
			statement->arguments[i] = ReadASTNode(argument_type, file);
		}

		return (AST_NODE*)statement;
	case AST_NODE_TYPE_BLOCK:
		;
		fpos_t block_start;
		fpos_t block_end;

		uint32_t size = FileReadUInt32(file);
		fgetpos(file, &block_start);
		uint16_t events = FileReadUInt16(file);

		AST_NODE_BLOCK* block_node = ast_alloc(sizeof(AST_NODE_BLOCK));
		block_node->type = AST_NODE_TYPE_BLOCK;
		block_node->statement_count = events;
		block_node->macro = nullptr;

		AST_STATEMENT_LIST_NODE* last_node = nullptr;
		AST_STATEMENT_LIST_NODE* new_node = nullptr;
		for (int i = 0; i < events; i++) {
			new_node = ast_alloc(sizeof(AST_STATEMENT_LIST_NODE));
			new_node->next = nullptr;
			new_node->statement = (AST_NODE_STATEMENT*)ReadASTNode(AST_NODE_TYPE_EVENT, file);

			if (last_node)
				last_node->next = new_node;
			else
				block_node->statements = new_node;

			last_node = new_node;
		}

		fgetpos(file, &block_end);
		if ((block_end - block_start) != size)
			ast_error("The size of the block node was supposed to be %lld, but we read %lld bytes instead.", size, block_end - block_start);

		return (AST_NODE*)block_node;
	case AST_NODE_TYPE_STRING:
		;
		uint8_t len = FileReadUInt8(file);

		AST_NODE_STRING* str_node = ast_alloc(sizeof(AST_NODE_STRING));
		str_node->type = AST_NODE_TYPE_STRING;
		str_node->macro = nullptr;
		str_node->ref = StringRef_Create(FileReadString(file, len));
		return (AST_NODE*)str_node;
	case AST_NODE_TYPE_BYTE:
		;
		AST_NODE_BYTE* b_node = ast_alloc(sizeof(AST_NODE_BYTE));
		b_node->type = AST_NODE_TYPE_BYTE;
		b_node->macro = nullptr;
		b_node->value = FileReadUInt8(file);
		return (AST_NODE*)b_node;
	case AST_NODE_TYPE_SHORT:
		;
		AST_NODE_SHORT* s_node = ast_alloc(sizeof(AST_NODE_SHORT));
		s_node->type = AST_NODE_TYPE_SHORT;
		s_node->macro = nullptr;
		s_node->value = FileReadUInt16(file);
		return (AST_NODE*)s_node;
	case AST_NODE_TYPE_INTEGER:
		;
		AST_NODE_INTEGER* i_node = ast_alloc(sizeof(AST_NODE_INTEGER));
		i_node->type = AST_NODE_TYPE_INTEGER;
		i_node->macro = nullptr;
		i_node->value = FileReadUInt32(file);
		return (AST_NODE*)i_node;
	case AST_NODE_TYPE_DECIMAL:
		;
		AST_NODE_DECIMAL* d_node = ast_alloc(sizeof(AST_NODE_DECIMAL));
		d_node->type = AST_NODE_TYPE_DECIMAL;
		d_node->macro = nullptr;
		d_node->value = FileReadUInt32(file);
		return (AST_NODE*)d_node;
	}

	ast_error("Couldn't read unsupported AST node type '%s'.", GetNodeTypeName(type));
	return nullptr;
}

AST_NODE_BLOCK* LoadCompiledScript(FILE* file) {
	uint8_t fileVersion = FileReadUInt8(file);
	if (fileVersion != SCRIPT_FILE_VERSION) {
		printf("Script version is %d, but the compiler only supports version %d.", fileVersion, SCRIPT_FILE_VERSION);
		return nullptr;
	}

	return (AST_NODE_BLOCK*)ReadASTNode(AST_NODE_TYPE_BLOCK, file);
}

// Includes AST IO operations.
void WriteASTNode(const AST_NODE* node, FILE* file) {
	debug_printf("Writing AST Node of type '%s'.\n", GetNodeTypeName(node->type));

	switch (node->type) {
	case AST_NODE_TYPE_EVENT:
	case AST_NODE_TYPE_TRIGGER:
		;
		AST_NODE_STATEMENT* statement = (AST_NODE_STATEMENT*)node;
		FileWriteUInt8(file, statement->command->opcode);

		for (int i = 0; i < statement->command->argument_count; i++)
			WriteASTNode(statement->arguments[i], file);

		break;
	case AST_NODE_TYPE_BLOCK:
		;
		fpos_t block_size_pos;
		fpos_t block_start_pos;
		fpos_t block_end_pos;
		fgetpos(file, &block_size_pos);
		FileWriteUInt32(file, 0); // Dummy value.
		fgetpos(file, &block_start_pos);

		// Statement count.
		uint16_t statement_count = ((AST_NODE_BLOCK*)node)->statement_count;
		FileWriteUInt16(file, statement_count);

		// Write statements.
		AST_STATEMENT_LIST_NODE* statement_node = ((AST_NODE_BLOCK*)node)->statements;
		while (statement_node) {
			WriteASTNode((AST_NODE*)statement_node->statement, file);
			statement_node = statement_node->next;
		}

		// Write size.
		fgetpos(file, &block_end_pos);
		FileWriteUInt32At(file, &block_size_pos, (uint32_t)(block_end_pos - block_start_pos));
		break;
	case AST_NODE_TYPE_STRING:
		;
		char* str = ((AST_NODE_STRING*)node)->ref->value;
		FileWriteUInt8(file, (uint8_t)strlen(str));
		FileWriteString(file, str);
		break;
	case AST_NODE_TYPE_BYTE:
		FileWriteUInt8(file, ((AST_NODE_BYTE*)node)->value);
		break;
	case AST_NODE_TYPE_SHORT:
		FileWriteUInt16(file, ((AST_NODE_SHORT*)node)->value);
		break;
	case AST_NODE_TYPE_INTEGER:
		FileWriteUInt32(file, ((AST_NODE_INTEGER*)node)->value);
		break;
	case AST_NODE_TYPE_DECIMAL:
		FileWriteUInt32(file, ((AST_NODE_DECIMAL*)node)->value);
		break;
	default:
		ast_error("Couldn't write unsupported AST node type '%s'.", GetNodeTypeName(node->type));
		break;
	}
}

void SaveCompiledScript(const AST_NODE_BLOCK* root, FILE* file) {
	FileWriteUInt8(file, SCRIPT_FILE_VERSION);
	WriteASTNode((AST_NODE*)root, file);
}

// Frees an AST node, and all attached nodes.
void FreeAST(AST_NODE* node) {
	switch (node->type) {
	case AST_NODE_TYPE_EVENT:
	case AST_NODE_TYPE_TRIGGER:
		;
		AST_NODE_STATEMENT* statement = (AST_NODE_STATEMENT*)node;
		for (int i = 0; i < statement->command->argument_count; i++)
			FreeAST(statement->arguments[i]);

		free(statement->arguments);
		statement->arguments = nullptr;
		break;
	case AST_NODE_TYPE_BLOCK:
		;
		AST_STATEMENT_LIST_NODE* next;
		AST_STATEMENT_LIST_NODE* list_node = ((AST_NODE_BLOCK*)node)->statements;
		while (list_node) {
			next = list_node->next;
			FreeAST((AST_NODE*)list_node->statement);
			free(list_node);
			list_node = next;
		}

		break;
	case AST_NODE_TYPE_STRING: // We don't free the string here, since it's freed on the token.
		StringRef_FreeUsage(((AST_NODE_STRING*)node)->ref);
		break;
	case AST_NODE_TYPE_BYTE:
	case AST_NODE_TYPE_SHORT:
	case AST_NODE_TYPE_INTEGER:
	case AST_NODE_TYPE_DECIMAL:
		// Do nothing.
		break;
	default:
		ast_error("Couldn't free unsupported node type '%s'.", GetNodeTypeName(node->type));
		break;
	}

	free(node);
}

void DebugPrintAST(AST_NODE* node) {
	char* decompiled_text = Decompile(node);
	printf("%s", decompiled_text);
	free(decompiled_text);
}