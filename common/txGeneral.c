#include "txGeneral.h"

_RAM_CODE _NO_INLINE
void txMemcpy(void *pO, void *pI, _u32 length)
{
	_u8 *pInput = (_u8*) pI;
	_u8 *pOuput = (_u8*) pO;
	while ((length--) > 0)
	{
		*pOuput++ = *pInput++;
	}
}

_RAM_CODE _NO_INLINE
void txMemcpy4(void *pO, void *pI, _u32 length)
{
	if((((int)pO)%4!=0) || (((int)pI)%4!=0))
	{
		__asm__ volatile("ecall");
		return;
	}
	unsigned char* source = pI;
	unsigned char* dest   = pO;
	unsigned int  int_length  = length/4;
	unsigned int  char_length = length%4;
	if(int_length != 0)
	{
		int* s_4 = (int*)source;
		int* d_4 = (int*)dest;
		for(unsigned int i=0;i<int_length;i++)
		{
			d_4[i] = s_4[i];
		}
	}
	if(char_length != 0)
	{
		source+=(4*int_length);
		dest  +=(4*int_length);
		while(char_length--)
		{
			*dest++ = *source++;
		}
	}
}

_RAM_CODE _NO_INLINE
int txMemcmp(void * str1, void * str2, _u32 length)
{
	_u8 *st1 = (_u8*)str1;
	_u8 *st2 = (_u8*)str2;

	while(length--)
    {
		if(*st1 != *st2)
        {
			return 1; 
		}
		st1++;
		st2++;
	}
	return 0;
}

_RAM_CODE _NO_INLINE
void txMemsetByte(_u8 * pO, _u8 pValue, _u32 length)
{
	_u8 *ptr = pO;
	while (length-- > 0)
    {
		*ptr++ = pValue;
    }
}

_RAM_CODE _NO_INLINE
void txMemsetWord(_u32 * pO, _u32 pValue, _u32 length)
{
	_u32 *ptr = pO;
	while (length-- > 0)
    {
		*ptr++ = pValue;
    }
}

_RAM_CODE _NO_INLINE
int txStringLength(_s8* string)
{
	_u32 length = 0;
	while(*string != '\0')
    {
		length ++;
		string ++;
	}
	return length;
}

int txCompareTime(_u32 val1,_u32 val2)
{
	if((val1-val2) < (1U<<30))
	{
		return 1;
	}
	else
	{
		return 0;
	}
} 

int swap_endian_32(int value)
{
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8)  |
           ((value & 0x00FF0000) >> 8)  |
           ((value & 0xFF000000) >> 24);
}

int swap_endian_16(int value)
{
    return ((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8);
}

