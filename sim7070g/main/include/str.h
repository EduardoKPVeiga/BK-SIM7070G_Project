#ifndef STR_H_
#define STR_H_

#include "string.h"

#define SIZE(x) ((sizeof(x)) - 1)

bool StrCompare(char *str1, char *str2, int size_str1, int size_str2);

bool StrCompareMacroWithArray(const char *str, char *char_array, int size_str, int size_char_array);

void StrConcatenate(char *str1, char *str2, char *str_result, int size_str1, int size_str2);

bool ValidCharArray(char *char_array, int length);

bool StrContainsSubstr(char *str, char *sub_str, int size_str, int size_sub_str);

bool StrContainsChar(char *str, char a, int size_str);

char *strrev(char *str);

#endif