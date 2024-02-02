#include "../include/str.h"

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

bool StrCompareMacroWithArray(const char *str, char *char_array, int size_str, int size_char_array)
{
    char str_aux[size_char_array] = {0};
    for (int i = 0; i < size_str; i++)
        str_aux[i] = str[i];

    if (size_str != size_char_array)
        return 0;
    for (int i = 0; i < size_str; i++)
    {
        if (str_aux[i] != char_array[i])
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

bool ValidCharArray(char *char_array, int length)
{
    for (int i = 0; i < length; i++)
    {
        if ((char_array[i] < 33 || char_array[i] > 125) && char_array[i] != '\0' && char_array[i] != '\r' && char_array[i] != '\n')
            return 0;
    }
    return 1;
}

bool StrContainsSubstr(char *str, char *sub_str, int size_str, int size_sub_str)
{
    int i = 0, j = 0;
    for (; i < size_str; i++)
    {
        if (str[i] == sub_str[j] && j < size_str)
        {
            if (j == size_sub_str - 1)
                return 1;
            j++;
        }
        else
        {
            j = 0;
        }
    }
    return 0;
}

char *strrev(char *str)
{
    char *p1, *p2;

    if (!str || !*str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}