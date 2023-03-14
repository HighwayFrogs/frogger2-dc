#include "preproc.h"

typedef struct __script_preprocessor_macro SCRIPT_PREPROCESSOR_MACRO;
typedef struct __script_preprocessor_data SCRIPT_PREPROCESSOR_DATA;

void* ast_talloc(const SCRIPT_TOKEN* token, int size);
void token_error(const SCRIPT_TOKEN* token, const char* format, ...);
void RequireTokenType(const SCRIPT_TOKEN* token, SCRIPT_TOKEN_TYPE type);

extern MACRO_BANK* CreateMacroBank() {
	MACRO_BANK* bank = ast_talloc(nullptr, sizeof(MACRO_BANK));
	bank->list_head = nullptr;
	RegisterBuiltinMacros(bank);
	return bank;
}

void RegisterBuiltinMacro(MACRO_BANK* bank, const char* name, uint32_t value) {
	SCRIPT_PREPROCESSOR_MACRO* macro = ast_talloc(nullptr, sizeof(SCRIPT_PREPROCESSOR_MACRO));
	macro->location.line = -1;
	macro->location.column = -1;
	macro->name = StringRef_Create(CloneString(name)); // Cloning the string is necessary because this function receives hardcoded strings, which should not be free'd.
	macro->next = nullptr;
	macro->token = (SCRIPT_TOKEN*)MakeTokenValue32(TOKEN_INTEGER, value);
	macro->builtin = true;
	macro->usages = 0;
	RegisterMacro(bank, macro);
}

void RegisterMacroArray(MACRO_BANK* bank, MACRO_NAME_ARRAY* array) {
	for (uint16_t i = 0; i < array->count; i++)
		RegisterBuiltinMacro(bank, array->names[i], array->start_index + i);
}

// Equivalent to the FS_SET_ macros in script.c
MACRO_NAME_ARRAY g_ActorStateNames = {
	1,
	9,
	"ACTOR_MOVE",
	"ACTOR_STOP",
	"ACTOR_TOGGLE_MOVE",
	"ACTOR_INVISIBLE",
	"ACTOR_VISIBLE",
	"ACTOR_TOGGLE_VISIBILITY",
	"ACTOR_DRAW",
	"ACTOR_NODRAW",
	"ACTOR_TOGGLE_DRAW"
};

MACRO_NAME_ARRAY g_TileStateNames = {
	0,
	23,
	"TILE_NORMAL",
	"TILE_DEADLY",
	"TILE_SINK",
	"TILE_ICE",
	"TILE_SUPERHOP",
	"TILE_JOIN",
	"TILE_SAFE",
	"TILE_FINISHLINE",
	"TILE_GRAPPLE",
	"TILE_SMASH",
	"TILE_BARRED",
	"TILE_FALL",
	"FILE_FROGGER1ARENA",
	"FILE_FROGGER2ARENA",
	"FILE_FROGGER3ARENA",
	"FILE_FROGGER4ARENA",
	"TILE_DEADLYFALL",
	"TILE_HOT",
	"TILE_ELECTRIC",
	"TILE_NOSUPER",
	"TILE_PANTS",
	"TILE_NOSUPERHOT",
	"TILE_OCCUPIED"
};

MACRO_NAME_ARRAY g_ConveyerTileStateNames = {
	32,
	16,
	"TILE_CONVEYOR0_SLOW",
	"TILE_CONVEYOR1_SLOW",
	"TILE_CONVEYOR2_SLOW",
	"TILE_CONVEYOR3_SLOW",
	"TILE_CONVEYOR0_MED",
	"TILE_CONVEYOR1_MED",
	"TILE_CONVEYOR2_MED",
	"TILE_CONVEYOR3_MED",
	"TILE_CONVEYOR0_FAST",
	"TILE_CONVEYOR1_FAST",
	"TILE_CONVEYOR2_FAST",
	"TILE_CONVEYOR3_FAST",
	"TILE_CONVEYOR0_ONEWAY",
	"TILE_CONVEYOR1_ONEWAY",
	"TILE_CONVEYOR2_ONEWAY",
	"TILE_CONVEYOR3_ONEWAY"
};

MACRO_NAME_ARRAY g_AnimationFlags = {
	0,
	4,
	"ANIMATION_NONE",
	"ANIMATION_LOOP",
	"ANIMATION_QUEUE",
	"ANIMATION_LOOP_AND_QUEUE"
};

bool IsIndexInMacroArray(const MACRO_NAME_ARRAY* array, uint32_t index) {
	return index >= array->start_index && array->count > index - array->start_index;
}

char* GetNameFromMacroArray(const MACRO_NAME_ARRAY* array, uint32_t index) {
	return IsIndexInMacroArray(array, index) ? array->names[index - array->start_index] : nullptr;
}

void RegisterBuiltinMacros(MACRO_BANK* bank) {
	// The reason this isn't in a #include defs.inc file is because it allows us to incorporate this into the decompiler having it here.

	RegisterMacroArray(bank, &g_ActorStateNames);

	// The trigger flags found in event.h
	// Some enums have been omitted as they appear to only be used at runtime.
	// TRIGGER_DELAY is incompatible with TRIGGER_ONCE. We don't include it here, since we have no way of specifying the delay time parameter to the game. So, in other words, this functionality can't be used by a script without modifying game code.
	RegisterBuiltinMacro(bank, "TRIGGER_ONCE", 1); // If this is used, after the trigger fires, it will be unregistered.
	RegisterBuiltinMacro(bank, "TRIGGER_START", 16); // When the trigger fires, only fire once. The trigger is allowed to fire again after the condition stops being met.

	RegisterBuiltinMacro(bank, "MAIN_PLAYER", 0);
	RegisterBuiltinMacro(bank, "ACTOR_NONE", 0);
	RegisterBuiltinMacro(bank, "ACTOR_ALL", 0);
	RegisterBuiltinMacro(bank, "ENEMY_ALL", 0);
	RegisterBuiltinMacro(bank, "PLATFORM_ALL", 0);

	RegisterBuiltinMacro(bank, "ENEMYFLAG_NODAMAGE", 1 << 21);

	RegisterMacroArray(bank, &g_TileStateNames);
	RegisterMacroArray(bank, &g_ConveyerTileStateNames);
	RegisterMacroArray(bank, &g_AnimationFlags);
}

SCRIPT_PREPROCESSOR_MACRO* FindMacroByName(const MACRO_BANK* bank, const char* macroName) {
	if (!bank || !macroName)
		return nullptr;

	SCRIPT_PREPROCESSOR_MACRO* temp = bank->list_head;
	while (temp) {
		if (!strcasecmp(temp->name->value, macroName))
			return temp;

		temp = temp->next;
	}

	return nullptr;
}

void RegisterMacro(MACRO_BANK* bank, SCRIPT_PREPROCESSOR_MACRO* macro) {
	if (!bank->list_head) {
		bank->list_head = macro;
		return;
	}

	// Find the last node in the macro array.
	SCRIPT_PREPROCESSOR_MACRO* tail = bank->list_head;
	while (tail->next)
		tail = tail->next;

	tail->next = macro;
}

SCRIPT_TOKEN* CreateUsableTokensFromMacro(const SCRIPT_PREPROCESSOR_MACRO* macro, const SCRIPT_TOKEN_LOCATION* location, SCRIPT_TOKEN* next_token) {
	if (!macro->token)
		return next_token;

	SCRIPT_TOKEN* temp;
	SCRIPT_TOKEN* head = nullptr;
	SCRIPT_TOKEN* tail = nullptr;
	SCRIPT_TOKEN* source = macro->token;
	while (source) {
		temp = CloneToken(source, location);

		if (head)
			tail->next = temp;
		else
			head = temp;

		tail = temp;
		source = source->next;
	}

	if (tail)
		tail->next = next_token;
	return head;
}

SCRIPT_TOKEN* GetAndUnlinkNextToken(SCRIPT_TOKEN* token) {
	SCRIPT_TOKEN* next = token->next;
	token->next = nullptr;
	return next;
}

SCRIPT_TOKEN* RunPreprocessorDirective(SCRIPT_PREPROCESSOR_DATA* data, const char* directiveName, SCRIPT_TOKEN* token) {
	if (!strcasecmp(directiveName, "include")) {
		RequireTokenType(token, TOKEN_STRING);
		char* filename = ((SCRIPT_TOKEN_STRING*)token)->ref->value;

		// Read text.
		char* includedFileText = nullptr;
		if (!FileReadText(filename, &includedFileText))
			token_error(token, "Could not read text from the included file '%s'.", filename);

		// Tokenize.
		SCRIPT_TOKEN* included_file_root = Tokenize(includedFileText);
		free(includedFileText);

		if (!included_file_root) // There are no tokens in that file.
			return GetAndUnlinkNextToken(token);

		// Link the following tokens after the end of the included file.
		SCRIPT_TOKEN* tail = included_file_root;
		while (tail->next) {
			if (tail->next->type == TOKEN_EOF && !tail->next->next) { // Remove EOF.
				FreeTokens(tail->next);
				break;
			}

			tail = tail->next;
		}
		tail->next = GetAndUnlinkNextToken(token);

		// Return the tokens included from another file.
		return included_file_root;
	} else if (!strcasecmp(directiveName, "define")) {
		SCRIPT_PREPROCESSOR_MACRO* macro;
		RequireTokenType(token, TOKEN_IDENTIFIER);
		STRING_REFERENCE* macroNameRef = ((SCRIPT_TOKEN_STRING*)token)->ref;

		// Check if the macro is already registered.
		if (macro = FindMacroByName(&data->macros, macroNameRef->value))
			token_error(token, "A macro named '%s' is already defined at %s!", macro->name->value, GetLocationText(&macro->location));

		// Create macro.
		macro = ast_talloc(token, sizeof(SCRIPT_PREPROCESSOR_MACRO));
		macro->name = StringRef_AddUsage(macroNameRef);
		macro->location = token->location;
		macro->usages = 0;
		macro->next = nullptr;
		macro->token = GetAndUnlinkNextToken(token); // Unlink, so we don't free any tokens after the macro name
		macro->builtin = false;

		RegisterMacro(&data->macros, macro);
		return GetAndUnlinkNextToken(macro->token);
	}

	token_error(token, "Unsupported preprocessor directive '%s'.", directiveName);
	return nullptr;
}

SCRIPT_TOKEN* Preprocess(SCRIPT_PREPROCESSOR_DATA* data, SCRIPT_TOKEN* root) {
	SCRIPT_TOKEN* last = root;
	SCRIPT_TOKEN* token = root;

	SCRIPT_TOKEN* startToken;
	while (token) {
		startToken = token;

		debug_printf("Preprocessor is processing the '%s' token.\n", GetTokenName(token->type));

		if (token->type == TOKEN_POUND) {
			token = token->next;
			RequireTokenType(token, TOKEN_IDENTIFIER);
			char* name = ((SCRIPT_TOKEN_STRING*)token)->ref->value;

			// Run directive.
			token = RunPreprocessorDirective(data, name, token->next);
			debug_printf("Ran preprocessor directive '%s'.\n", name);
		} else if (token->type == TOKEN_IDENTIFIER) {
			char* macroName = ((SCRIPT_TOKEN_STRING*)token)->ref->value;
			SCRIPT_PREPROCESSOR_MACRO* macro = FindMacroByName(&data->macros, macroName);

			// If a macro was found, replace the node with macro data.
			if (macro) {
				macro->usages++;
				token = CreateUsableTokensFromMacro(macro, &token->location, GetAndUnlinkNextToken(token));
			}
		}

		// Apply results.
		if (token != startToken) {
			if (last != root)
				last->next = token;
			else
				root = token;

			// Frees any tokens attached to the preprocessor tokens which got removed.
			FreeTokens(startToken);
			last = token;
		} else if (token) { // Go to next node.
			last = token;
			token = token->next;
		}
	}

	return root;
}

SCRIPT_TOKEN* RunPreprocessor(SCRIPT_TOKEN* root) {
	SCRIPT_PREPROCESSOR_DATA* data = ast_talloc(root, sizeof(SCRIPT_PREPROCESSOR_DATA));
	data->macros.list_head = nullptr;
	RegisterBuiltinMacros(&data->macros);

	// Actually does the preprocessor stuff.
	SCRIPT_TOKEN* result = Preprocess(data, root);

	// Free data.
	FreeMacroBank(&data->macros);
	return result;
}

extern void FreeMacrosFromBank(MACRO_BANK* bank) {
	SCRIPT_PREPROCESSOR_MACRO* next;
	SCRIPT_PREPROCESSOR_MACRO* macro = bank->list_head;
	while (macro) {
		next = macro->next;
		StringRef_FreeUsage(macro->name);
		FreeTokens(macro->token);
		free(macro);
		macro = next;
	}

	bank->list_head = nullptr;
}

extern void FreeMacroBank(MACRO_BANK* bank) {
	if (!bank)
		return;

	FreeMacrosFromBank(bank);
	free(bank);
}