#include "utils.h"
#include "math.h"

void FixedPointU16ToStringBuilder(STRING_BUILDER* builder, uint32_t value) {
    const int BitCount = 16;
    const int Divisor = (1 << BitCount);
    const int BitMask = (1 << BitCount) - 1; // FFFF
    const int DigitRounder = 100000; // This is the sweet spot to make the decompiler work properly. Don't change this.

    uint16_t fracPart = (value & BitMask);
    uint16_t integerPart = ((value >> BitCount) & BitMask);

    // Write decimal part.
    if (fracPart != 0) {
        double number = integerPart + (ceil(DigitRounder * ((double)fracPart / Divisor)) / DigitRounder);
        StringBuilder_Append(builder, "%.5g", number);
    } else {
        // Write integer part.
        StringBuilder_Append(builder, "%u", integerPart);
    }
}

char* FixedPointU16ToString(uint32_t value) {
    STRING_BUILDER* builder = StringBuilder_Create(12);
    if (!builder)
        return nullptr;

    FixedPointU16ToStringBuilder(builder, value);
    StringBuilder_AppendChar(builder, '\0');
    return StringBuilder_FreeKeepResult(builder);
}

char* AppendString(const char* a, const char* b) {
    int a_len = a ? strlen(a) : 0;
    int b_len = b ? strlen(b) : 0;

    int newStrLength = a_len + b_len + 1;
    char* result = malloc(newStrLength);
    if (!result) {
        printf("AppendString() - malloc failure.\n");
        exit(1);
        return nullptr;
    }

    // Copy strings to new string.
    if (a_len && strncpy_s(result, newStrLength, a, a_len)) {
        printf("Failed to copy first string to output. (A: '%s', B: '%s')\n", a, b);
        exit(1);
    }

    if (b_len && strncpy_s(&result[a_len], newStrLength - a_len, b, b_len)) {
        printf("Failed to copy second string to output. (A: '%s', B: '%s')\n", a, b);
        exit(1);
    }


    result[newStrLength - 1] = '\0'; // Write terminator.

    return result;
}

char* CloneString(const char* str) {
    int len = str ? strlen(str) : 0;

    char* result = malloc(len + 1);
    if (!result) {
        printf("AppendString() - malloc failure.\n");
        exit(1);
        return nullptr;
    }

    // Copy strings to new string.
    if (len && strncpy_s(result, len + 1, str, len)) {
        printf("Failed to copy string to output. (Input: '%s')\n", str);
        exit(1);
    }

    result[len] = '\0'; // Write terminator.

    return result;
}


bool DoFilesMatch(const char* file_1, const char* file_2) {
    FILE* file1ptr;
    FILE* file2ptr;
    long filelen;
    errno_t err;

    err = fopen_s(&file1ptr, file_1, "rb");  // Open the file in binary mode
    if (err != 0) {
        printf("DoFilesMatch: Could not open the file '%s'. (Error: %d)\n", file_1, err);
        return false;
    }

    err = fopen_s(&file2ptr, file_2, "rb");  // Open the file in binary mode
    if (err != 0) {
        printf("DoFilesMatch: Could not open the file '%s'. (Error: %d)\n", file_2, err);
        return false;
    }

    // Verify sizes match.
    if ((filelen = FileGetLength(file1ptr)) != FileGetLength(file2ptr)) {
        fclose(file1ptr);
        fclose(file2ptr);
        return false;
    }

    bool matching = true;
    for (long i = 0; i < filelen && matching; i++)
        if (FileReadUInt8(file1ptr) != FileReadUInt8(file2ptr))
            matching = false;

    fclose(file1ptr);
    fclose(file2ptr);
    return matching;
}

long FileGetLength(FILE* file) {
    fseek(file, 0, SEEK_END);          // Jump to the end of the file
    long filelen = ftell(file);             // Get the current byte offset in the file
    rewind(file);
    return filelen;
}

bool FileReadText(const char* filename, char** buffer) {
    FILE* fileptr;
    char* filebuf;
    errno_t err;
    long filelen;
    size_t result;

    err = fopen_s(&fileptr, filename, "rb");  // Open the file in binary mode
    if (err != 0) {
        printf("FileReadText: Could not open the file '%s'. (Error: %d)\n", filename, err);
        return false;
    }

    filelen = FileGetLength(fileptr);             // Get the current byte offset in the file
    if (filelen == -1) {
        printf("FileReadText: Failed to ftell() the file length for '%s'.\n", filename);
        fclose(fileptr);
        return false;
    }

    filebuf = (char*)malloc(filelen * sizeof(char) + 1); // Enough memory for the file
    if (!filebuf) {
        printf("FileReadText: Failed to allocate memory to hold contents of file '%s'.\n", filename);
        fclose(fileptr);
        return false;
    }

    result = fread(filebuf, sizeof(char), filelen, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file
    filebuf[result] = '\0';

    // Error checking.
    if (result != filelen)
        printf("FileReadText: Expected to read %d bytes, but we only got %d. Error??\n", filelen, result);

    *buffer = filebuf;
    return result == filelen;
}

char* FileReadString(FILE* file, uint32_t len) {
    char* result = malloc(len + 1);
    if (!result) {
        printf("Failed to allocate string buffer for FileReadString!\n");
        return nullptr;
    }

    if (!fread(result, sizeof(char), len, file))
        printf("Failed to read string of length %d from file!\n", len);
    result[len] = '\0';
    return result;
}

uint8_t FileReadUInt8(FILE* file) {
    uint8_t result;
    if (!fread(&result, sizeof(uint8_t), 1, file))
        printf("Failed to read uint8_t from file!\n");
    return result;
}

uint16_t FileReadUInt16(FILE* file) {
    uint16_t result;
    if (!fread(&result, sizeof(uint16_t), 1, file))
        printf("Failed to read uint16_t from file!\n");
    return result;
}

uint32_t FileReadUInt32(FILE* file) {
    uint32_t result;
    if (!fread(&result, sizeof(uint32_t), 1, file))
        printf("Failed to read uint32_t from file!\n");
    return result;
}

void FileWriteString(FILE* file, const char* value) {
    if (!fwrite(value, sizeof(char), strlen(value), file))
        printf("Failed to write string to file!\n");
}

void FileWriteUInt8(FILE* file, uint8_t value) {
    if (!fwrite(&value, sizeof(value), 1, file))
        printf("Failed to write uint8 to file!\n");
}

void FileWriteUInt16(FILE* file, uint16_t value) {
    if (!fwrite(&value, sizeof(value), 1, file))
        printf("Failed to write uint16 to file!\n");
}

void FileWriteUInt32(FILE* file, uint32_t value) {
    if (!fwrite(&value, sizeof(value), 1, file))
        printf("Failed to write uint32 to file!\n");
}

void FileWriteUInt32At(FILE* file, const fpos_t *pos, uint32_t value) {
    fpos_t current_pos;
    fgetpos(file, &current_pos);
    fsetpos(file, pos);
    FileWriteUInt32(file, value);
    fsetpos(file, &current_pos);
}


STRING_BUILDER* StringBuilder_Create(uint32_t initialSize) {
    STRING_BUILDER* builder = malloc(sizeof(STRING_BUILDER));
    if (!builder) {
        printf("Failed to allocate StringBuilder.");
        return nullptr;
    }

    builder->str = malloc(initialSize * sizeof(char));
    if (!builder->str) {
        free(builder);
        printf("Failed to allocate StringBuilder buffer.");
        return nullptr;
    }

    builder->curr_pos = 0;
    builder->curr_length = initialSize;
    builder->str[0] = '\0';
    return builder;
}

void StringBuilder_Clear(STRING_BUILDER* builder) {
    if (!builder)
        return;

    builder->str[0] = '\0';
    builder->curr_pos = 0;
}

void StringBuilder_Free(STRING_BUILDER* builder) {
    if (!builder)
        return;

    free(builder->str);
    free(builder);
}

char* StringBuilder_FreeKeepResult(STRING_BUILDER* builder) {
    if (!builder)
        return nullptr;

    char* result = builder->str;
    free(builder);
    return result;
}

char* StringBuilder_Format(const char* format, ...) {
    va_list args;

    va_start(args, format);
    STRING_BUILDER* builder = StringBuilder_Create(32);
    StringBuilder_Append(builder, format, args);
    va_end(args);

    return StringBuilder_FreeKeepResult(builder);
}

void StringBuilder_AppendChar(STRING_BUILDER* builder, char value) {
    StringBuilder_Append(builder, "%c", value);
}

void StringBuilder_AppendString(STRING_BUILDER* builder, const char* value) {
    StringBuilder_Append(builder, "%s", value);
}

void StringBuilder_Append(STRING_BUILDER* builder, const char* format, ...) {
    va_list args;
    va_start(args, format);

    int chars_written;

    do {
        chars_written = _vsnprintf_s(&builder->str[builder->curr_pos], _TRUNCATE, builder->curr_length - builder->curr_pos - 1, format, args);
        if (chars_written == -1) {
            char* old_str = builder->str;
            uint32_t old_len = builder->curr_length;

            builder->curr_length *= 2;
            builder->str = malloc(builder->curr_length * sizeof(char));
            if (!builder->str) {
                printf("Failed to allocate new string buffer of size %d.\n", builder->curr_length);
                exit(1);
                return;
            }

            if (strncpy_s(builder->str, builder->curr_length, old_str, old_len)) {
                printf("Failed to copy old string to new buffer.\n");
                exit(1);
                return;
            }

            free(old_str);
        }
    } while (chars_written == -1);

    builder->curr_pos += chars_written;
    va_end(args);
}

STRING_REFERENCE* StringRef_Create(char* str) {
    STRING_REFERENCE* ref = malloc(sizeof(STRING_REFERENCE));
    if (!ref)
        return nullptr;

    ref->value = str;
    ref->usages = 1;
    return ref;
}

STRING_REFERENCE* StringRef_AddUsage(STRING_REFERENCE* ref) {
    if (ref)
        ref->usages++;
    return ref;
}

void StringRef_FreeUsage(STRING_REFERENCE* ref) {
    if (!ref || !ref->usages || --ref->usages)
        return;

    free(ref->value);
    free(ref);
}