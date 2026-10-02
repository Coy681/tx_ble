/*
 * log.c
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 *
 *  Formatting layer only. Transport is handled by debug_port.
 */

#include"log.h"
#include"debug/debug_port.h"

volatile static _u8  strToHexU[24] = "0123456789ABCDEF";

/* build output in local block then hand to port */
void log_str(const char *str)
{
    _u8 block[DEBUG_PORT_OUTPUT_BUFFER_SIZE];
    _u32 len;
    _u8* buf;

    if(str == NULL)
    {
        return;
    }
    len = txStringLength(str);
    if(len > DEBUG_PORT_OUTPUT_BUFFER_SIZE - 7)
    {
        len = DEBUG_PORT_OUTPUT_BUFFER_SIZE - 7;//defensive programing
    }
    buf = block;
    while(len--)
    {
        *buf++ = *str++;
    }
    *buf++ = '\r';
    *buf++ = '\n';
    debug_port_write(block,(_u32)(buf - block));
}

void log_hex(const char *prefix, const _u8 *data, _u32 len)
{
    _u8 block[DEBUG_PORT_OUTPUT_BUFFER_SIZE];
    char *buf = (char *)block;
    _u32 pos = 0;

    if(prefix)
    {
        while(*prefix && pos < DEBUG_PORT_OUTPUT_BUFFER_SIZE - 9)
        {
            buf[pos++] = *prefix++;
        }
        if(pos < DEBUG_PORT_OUTPUT_BUFFER_SIZE - 9) buf[pos++] = ':';
    }

    for(_u32 i = 0; i < len; i++)
    {
        if(pos + 3 >= DEBUG_PORT_OUTPUT_BUFFER_SIZE - 5)
        {
            break;
        }
        buf[pos++] = ' ';
        buf[pos++] = strToHexU[(data[i] >> 4) & 0x0f];
        buf[pos++] = strToHexU[data[i] & 0x0f];
    }
    buf[pos++] = '\r';
    buf[pos++] = '\n';

    debug_port_write(block, pos);
}
