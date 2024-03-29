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

int StrContainsSubstr(char *str, char *sub_str, int size_str, int size_sub_str)
{
    bool first_time = false;
    int begin_position = -1;
    if (size_sub_str > size_str)
        return -1;
    int i = 0, j = 0;
    for (; i < size_str; i++)
    {
        if (str[i] == sub_str[j] && j < size_str)
        {
            if (!first_time)
            {
                first_time = true;
                begin_position = i;
            }
            if (j == size_sub_str - 1)
            {
                return begin_position;
            }
            j++;
        }
        else
        {
            first_time = false;
            begin_position = -1;
            j = 0;
        }
    }
    return -1;
}

bool StrContainsChar(char *str, char a, int size_str)
{
    for (int i = 0; i < size_str; i++)
    {
        if (str[i] == a)
            return true;
    }
    return false;
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

char *DecimalToCharArray(uint16_t decimal)
{
    if (decimal == 0)
        return "0";
    uint8_t digit = 0;
    string decimal_s = "";
    while (decimal != 0)
    {
        digit = decimal - ((uint16_t)(decimal / 10) * 10);
        decimal_s += (char)(digit + '0');
        decimal = (uint16_t)(decimal / 10);
    }
    char *decimal_array = strrev((char *)decimal_s.c_str());
    return decimal_array;
}

uint8_t DecimalToStr(uint16_t decimal, char *str, uint8_t str_size)
{
    if (decimal == 0)
    {
        str[0] = '0';
        return 1;
    }
    uint8_t digit = 0;
    uint8_t cont = 0;
    while (decimal != 0)
    {
        digit = decimal - ((uint16_t)(decimal / 10) * 10);
        str[cont] = (char)(digit + '0');
        decimal = (uint16_t)(decimal / 10);
        cont++;
    }
    return cont;
}

char *BintoCharArray(uint8_t bin)
{
    uint8_t bin_aux = bin;
    string bin_s = "";
    for (int i = 0; i < 8; i++)
    {
        if (bin_aux & 0b00000001)
            bin_s += '1';
        else
            bin_s += '0';
        bin_aux >>= 1;
    }
    char *bin_array = strrev((char *)bin_s.c_str());
    return bin_array;
}