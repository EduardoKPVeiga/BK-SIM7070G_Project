#include "str.h"

bool StrCompare(char *str1, char *str2)
{
    if (SIZE(str1) != SIZE(str2))
        return 0;
    for (int i = 0; i < SIZE(str1); i++)
    {
        if (str1[i] != str2[i])
            return 0;
    }
    return 1;
}

bool StrCompare(char *str1, char *str2, int size_str1, int size_str2)
{
    if (size_str1 != size_str2)
        return 0;
    for (int i = 0; i < size_str1; i++)
    {
        if (str1[i] != str2[i])
            return 0;
    }
    return 1;
}

void StrConcatenate(char *str1, char *str2, char *str_result, int size_str1, int size_str2)
{
    int i = 0;
    for (; i < size_str1; i++)
    {
        str_result[i] = str1[i];
    }
    for (int j = 0; j < size_str2; j++)
    {
        str_result[i + j] = str2[j];
    }
}