// main.c : This file contains the 'main' function. Program execution begins and ends there.

#include "main.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "ast.h"
#include "token.h"
#include "commands.h"
#include "preproc.h"
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <direct.h>
#endif

bool CompileFile(const char* srcFile, const char* destFile, bool messages);
bool DecompileFile(const char* srcFile, const char* destFile, bool messages);
void ShowUsage(const char* argv[]);
void ShowCommandList();
#ifdef PLATFORM_WINDOWS
void TestScripts(const char* directory);
#endif

int main(int argc, char* argv[]) {
    if (argc < 2) {
        ShowUsage(argv);
        return 1;
    }

    if (!strcasecmp(argv[1], "compile")) {
        if (argc < 4) {
            ShowUsage(argv);
            return 1;
        }

        char* srcFile = argv[2];
        char* destFile = argv[3];
        if (!CompileFile(srcFile, destFile, true))
            return 1;
    } else if (!strcasecmp(argv[1], "decompile")) {
        if (argc < 4) {
            ShowUsage(argv);
            return 1;
        }

        char* srcFile = argv[2];
        char* destFile = argv[3];
        if (!DecompileFile(srcFile, destFile, true))
            return 1;
    } else if (!strcasecmp(argv[1], "info")) {
        ShowCommandList();
    }
#ifdef PLATFORM_WINDOWS
    else if (!strcasecmp(argv[1], "test")) {
        if (argc < 3) {
            ShowUsage(argv);
            return 1;
        }

        // This undocumented feature lets me test the compiler is working.
        TestScripts(argv[2]);
    }
#endif
    else {
        printf("Unknown action '%s'.\n", argv[1]);
        return 1;
    }

#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}

void ShowUsage(const char* argv[]) {
    printf("Scripty - A recreation of the offical Frogger 2 Script Compiler (etc) by Kneesnap.\n");
    printf("Usage: %s [compile|decompile] <input.etc> <output.fev>\n", argv[0]);
    printf("Usage: %s [info] - Show a list of supported functions.\n", argv[0]);
    printf("\n");
}

void ShowCommandList() {
    printf("   ==== Active Functions / Commands: ====   \n");
    printf("\n");

    printf("Events [%d]:\n", REGISTERED_COMMAND_COUNT);
    for (int i = 0; i < REGISTERED_COMMAND_COUNT; i++) {
        if (!&g_RegisteredCommands[i].mnemonic)
            continue;

        printf(" - ");
        PrintCommandSignature(&g_RegisteredCommands[i]);
        printf("\n");
    }

    printf("\n");
    printf("Triggers [%d]:\n", REGISTERED_TRIGGER_COUNT);
    for (int i = 0; i < REGISTERED_TRIGGER_COUNT; i++) {
        if (!g_RegisteredTriggers[i].mnemonic)
            continue;

        printf(" - ");
        PrintCommandSignature(&g_RegisteredTriggers[i]);
        printf("\n");
    }

    // Print built-in macros.
    MACRO_BANK* bank = CreateMacroBank();

    printf("\n");
    printf("Builtin Macros:\n");

    SCRIPT_PREPROCESSOR_MACRO* macro = bank->list_head;
    while (macro) {
        char* token_str = TokenToString(macro->token);
        printf(" - '%s': %s\n", macro->name->value, token_str);
        free(token_str);
        macro = macro->next;
    }

    FreeMacroBank(bank);
}

bool CompileFile(const char* srcFile, const char* destFile, bool messages) {
    // Read file.
    if (messages)
        printf("Reading file '%s'...\n", srcFile);
    char* inputFileData = nullptr;
    if (!FileReadText(srcFile, &inputFileData))
        return false; // Failed to open file.

    // Tokenizing.
    if (messages)
        printf("Tokenizing...\n");
    SCRIPT_TOKEN* root_token = Tokenize(inputFileData);
#ifdef _DEBUG
    if (messages)
        DebugPrintTokens(root_token); // Debugging.
#endif
    
    if (root_token->type == TOKEN_ERROR) {
        printf("A syntax error occurred.\n");
        printf("Error: %s\n\nTokens:\n", ((SCRIPT_TOKEN_STRING*)root_token)->ref->value);
#ifndef _DEBUG
        DebugPrintTokens(root_token);
#endif
        FreeTokens(root_token);
        return false;
    }

    // Preprocessor.
    if (messages)
        printf("Preprocessing...\n");
    root_token = RunPreprocessor(root_token);
#ifdef _DEBUG
    if (messages)
        DebugPrintTokens(root_token); // Debugging.
#endif

    // Compiling.
    if (messages)
        printf("Compiling...\n");
    AST_NODE_BLOCK* root = BuildTreeFromTokens(root_token);
#ifdef _DEBUG
    if (messages)
        DebugPrintAST((AST_NODE*)root);
#endif
    
    // Writing.
    if (messages)
        printf("Saving...\n");

    FILE* fileptr;
    errno_t err = fopen_s(&fileptr, destFile, "wb");  // Open the file in binary mode to avoid replacing '\n' with '\r\n'.
    if (err) {
        printf("Could not open the file '%s'. (Error: %d)\n", destFile, err);
        return false;
    }

    SaveCompiledScript(root, fileptr);
    fclose(fileptr); // Close the file.

    // Cleanup.
    FreeTokens(root_token);
    FreeAST((AST_NODE*)root);
    free(inputFileData);
    if (messages)
        printf("Success.\n");
    return true;
}

bool DecompileFile(const char* srcFile, const char* destFile, bool messages) {
    errno_t err;
    FILE* fileptr;

    // Open script file.
    err = fopen_s(&fileptr, srcFile, "rb");
    if (err) {
        printf("Could not open the file '%s'. (Error: %d)\n", srcFile, err);
        return false;
    }

    // Load script from file.
    if (messages)
        printf("Loading compiled script...\n");
    AST_NODE_BLOCK* root = LoadCompiledScript(fileptr);
    if (!root) {
        printf("Failed to load compiled script '%s'.\n", srcFile);
        return false;
    }

    // Decompile script.
    if (messages)
        printf("Decompiling script...\n");
    char* decompiled_text = Decompile((AST_NODE*)root);
    FreeAST((AST_NODE*)root);

    // Save file.
    if (messages)
        printf("Saving decompiled script...\n");
    err = fopen_s(&fileptr, destFile, "w");
    if (err) {
        free(decompiled_text);
        printf("Could not open the file '%s'. (Error: %d)\n", destFile, err);
        return false;
    }

    FileWriteString(fileptr, decompiled_text); // Saves the text.
    fclose(fileptr); // Close the file
    free(decompiled_text);
    if (messages)
        printf("Success.\n");
    return true;
}

#ifdef PLATFORM_WINDOWS
LPWSTR ConvertCharArrayToLPWSTR(const char* charArray) {
    int bufferSize = strlen(charArray) * 2;
    wchar_t* wString = malloc(bufferSize * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, bufferSize);
    return (LPWSTR)wString;
}

char* ConvertLPWSTRToCharArray(LPWSTR str) {
    STRING_BUILDER* builder = StringBuilder_Create(32);
    StringBuilder_Append(builder, "%ls", str);
    return StringBuilder_FreeKeepResult(builder);
}


void TestScripts(const char* directory) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    int filesTested = 0;
    int filesMatched = 0;

    printf("Script Directory: %s\n", directory);
    char* decompiled = AppendString(directory, "\\decompiled\\");
    char* recompiled = AppendString(directory, "\\recompiled\\");
    char* fixedDir = AppendString(directory, "\\");
    char* windows = AppendString(directory, "\\*");

    _mkdir(decompiled);
    _mkdir(recompiled);

    LPWSTR wDirectory = ConvertCharArrayToLPWSTR(windows);
    hFind = FindFirstFile(wDirectory, &findFileData);
    free(wDirectory);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("FindFirstFile failed (%d)\n", GetLastError());
        return;
    }

    do {
        char* fileName = ConvertLPWSTRToCharArray(findFileData.cFileName);

        int fileNameLength = strlen(fileName);
        if (fileNameLength <= 4 || strcasecmp(".fev", &fileName[fileNameLength - 4])) {
            printf("Skipping file '%s'.\n", fileName);
            free(fileName);
            continue; // Not a match.
        }

        filesTested++;
        printf("Testing file '%s'.\n", fileName);

        char* filePath = AppendString(fixedDir, fileName);
        char* decompScrPath = AppendString(decompiled, fileName);
        char* recompScrPath = AppendString(recompiled, fileName);

        // Decompile then recompile.
        DecompileFile(filePath, decompScrPath, false);
        CompileFile(decompScrPath, recompScrPath, false);

        if (DoFilesMatch(filePath, recompScrPath)) {
            filesMatched++;
            printf("The file '%s' matched when recompiled.\n", fileName);
        } else {
            printf("The file '%s' did not match when recompiled.\n", fileName);
        }

        // Free data.
        free(fileName);
        free(filePath);
        free(decompScrPath);
        free(recompScrPath);
    } while (FindNextFile(hFind, &findFileData));

    FindClose(hFind);

    free(decompiled);
    free(recompiled);
    free(fixedDir);
    free(windows);

    printf("\nPerfectly Matching Files: [ %d / %d ]\n\n", filesMatched, filesTested);
}
#endif