#pragma once

typedef struct __script_preprocessor_macro SCRIPT_PREPROCESSOR_MACRO;
typedef struct __macro_bank MACRO_BANK;
typedef struct __script_preprocessor_data SCRIPT_PREPROCESSOR_DATA;

#include "utils.h"
#include "main.h"
#include "ast.h"
#include "token.h"

#define MINIMUM_DECOMPILER_MACRO_USAGES 5

typedef struct __script_preprocessor_macro {
	SCRIPT_PREPROCESSOR_MACRO* next;
	STRING_REFERENCE* name;
	SCRIPT_TOKEN* token;
	uint32_t usages;
	bool builtin;
	SCRIPT_TOKEN_LOCATION location;
} SCRIPT_PREPROCESSOR_MACRO;

typedef struct __macro_bank {
	SCRIPT_PREPROCESSOR_MACRO* list_head;
} MACRO_BANK;

typedef struct __script_preprocessor_data {
	MACRO_BANK macros;
} SCRIPT_PREPROCESSOR_DATA;

typedef struct __macro_name_array {
	uint32_t start_index;
	uint16_t count;
	char* names[];
} MACRO_NAME_ARRAY;

extern MACRO_NAME_ARRAY g_ActorStateNames;
extern MACRO_NAME_ARRAY g_TileStateNames;
extern MACRO_NAME_ARRAY g_ConveyerTileStateNames;
extern MACRO_NAME_ARRAY g_AnimationFlags;

extern bool IsIndexInMacroArray(const MACRO_NAME_ARRAY* array, uint32_t index);
extern char* GetNameFromMacroArray(const MACRO_NAME_ARRAY* array, uint32_t index);
extern MACRO_BANK* CreateMacroBank();
extern void RegisterBuiltinMacros(MACRO_BANK* bank);
extern SCRIPT_PREPROCESSOR_MACRO* FindMacroByName(const MACRO_BANK* bank, const char* macroName);
extern void RegisterMacro(MACRO_BANK* bank, SCRIPT_PREPROCESSOR_MACRO* macro);
extern SCRIPT_TOKEN* RunPreprocessor(SCRIPT_TOKEN* root);
extern void FreeMacrosFromBank(MACRO_BANK* bank);
extern void FreeMacroBank(MACRO_BANK* bank);