#pragma once

typedef struct __string_reference STRING_REFERENCE;
typedef struct __string_builder STRING_BUILDER;

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

extern void FixedPointU16ToStringBuilder(STRING_BUILDER* builder, uint32_t value);
extern char* FixedPointU16ToString(uint32_t value);
extern char* AppendString(const char* a, const char* b);
extern char* CloneString(const char* str);

// IO Utils
extern bool DoFilesMatch(const char* file_1, const char* file_2);
extern long FileGetLength(FILE* file);
extern bool FileReadText(const char* filename, char** buffer);
extern char* FileReadString(FILE* file, uint32_t len);
extern uint8_t FileReadUInt8(FILE* file);
extern uint16_t FileReadUInt16(FILE* file);
extern uint32_t FileReadUInt32(FILE* file);
extern void FileWriteString(FILE* file, const char* value);
extern void FileWriteUInt8(FILE* file, uint8_t value);
extern void FileWriteUInt16(FILE* file, uint16_t value);
extern void FileWriteUInt32(FILE* file, uint32_t value);
extern void FileWriteUInt32At(FILE* file, const fpos_t* pos, uint32_t value);

typedef struct __string_reference {
	char* value;
	uint32_t usages;
} STRING_REFERENCE;

typedef struct __string_builder {
	char* str;
	uint32_t curr_pos;
	uint32_t curr_length;
} STRING_BUILDER;

// String Builder
extern STRING_BUILDER* StringBuilder_Create(uint32_t initialSize);
extern void StringBuilder_Clear(STRING_BUILDER* builder);
extern void StringBuilder_Free(STRING_BUILDER* builder);
extern char* StringBuilder_FreeKeepResult(STRING_BUILDER* builder);
extern char* StringBuilder_Format(const char* format, ...);
extern void StringBuilder_AppendChar(STRING_BUILDER* builder, char value);
extern void StringBuilder_AppendString(STRING_BUILDER* builder, const char* value);
extern void StringBuilder_Append(STRING_BUILDER* builder, const char* format, ...);

// String Reference
extern STRING_REFERENCE* StringRef_Create(char* str);
extern STRING_REFERENCE* StringRef_AddUsage(STRING_REFERENCE* ref);
extern void StringRef_FreeUsage(STRING_REFERENCE* ref);