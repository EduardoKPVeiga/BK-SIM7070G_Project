#ifndef STR_H_
#define STR_H_

#define SIZE(x) ((sizeof(x)) - 1)

bool StrCompare(char *str1, char *str2);

bool StrCompare(char *str1, char *str2, int size_str1, int size_str2);

void StrConcatenate(char *str1, char *str2, char *str_result, int size_str1, int size_str2);

#endif