#ifndef VARHEXUTILS
#define VARHEXUTILS

#include "endian.h"
#include "multicodec/varint.h"
#include "multicodec/vhutils.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hex2str(const char *h, char *s, int sizs) {
    static char hex[] = "0123456789ABCDEF";
    int i = 0;
    int len = strlen(h) / 2;
    if (len > sizs)
        len = sizs;
    for (i = 0; i < len; ++i)

    {
        char *p = strchr(hex, *(h + 2 * i));
        if (p != NULL) {
            s[i] = (char)(unsigned char)((p - hex) << 4);
            p = strchr(hex, *(h + 2 * i + 1));
            if (p != NULL) {
                s[i] |= (char)(p - hex);
            } else
                s[i] = '?';
        } else
            s[i] = '?';
    }
    if (i < sizs * 2)
        *(s + i) = '\0';
}
void str2hex(unsigned char *hexstring, unsigned char *string) {
    unsigned char ch, i, j, len;

    len = strlen((char *)string);

    for (i = 0, j = 0; i < len; i++, j += 2) {
        ch = string[i];

        if (ch >= 0 && ch <= 0x0F) {
            hexstring[j] = 0x30;

            if (ch >= 0 && ch <= 9)
                hexstring[j + 1] = 0x30 + ch;
            else
                hexstring[j + 1] = 0x37 + ch;
        } else if (ch >= 0x10 && ch <= 0x1F) {
            hexstring[j] = 0x31;
            ch -= 0x10;

            if (ch >= 0 && ch <= 9)
                hexstring[j + 1] = 0x30 + ch;
            else
                hexstring[j + 1] = 0x37 + ch;
        } else if (ch >= 0x20 && ch <= 0x2F) {
            hexstring[j] = 0x32;
            ch -= 0x20;

            if (ch >= 0 && ch <= 9)
                hexstring[j + 1] = 0x30 + ch;
            else
                hexstring[j + 1] = 0x37 + ch;
        } else if (ch >= 0x30 && ch <= 0x3F) {
            hexstring[j] = 0x33;
            ch -= 0x30;

            if (ch >= 0 && ch <= 9)
                hexstring[j + 1] = 0x30 + ch;
            else
                hexstring[j + 1] = 0x37 + ch;
        } else if (ch >= 0x40 && ch <= 0x4F) {
            hexstring[j] = 0x34;
            ch -= 0x40;

            if (ch >= 0 && ch <= 9)
                hexstring[j + 1] = 0x30 + ch;
            else
                hexstring[j + 1] = 0x37 + ch;
        } else if (ch >= 0x50 && ch <= 0x5F) {
            hexstring[j] = 0x35;
            ch -= 0x50;

            if (ch >= 0 && ch <= 9)
                hexstring[j + 1] = 0x30 + ch;
            else
                hexstring[j + 1] = 0x37 + ch;
        } else if (ch >= 0x60 && ch <= 0x6F) {
            hexstring[j] = 0x36;
            ch -= 0x60;

            if (ch >= 0 && ch <= 9)
                hexstring[j + 1] = 0x30 + ch;
            else
                hexstring[j + 1] = 0x37 + ch;
        } else if (ch >= 0x70 && ch <= 0x7F) {
            hexstring[j] = 0x37;
            ch -= 0x70;

            if (ch >= 0 && ch <= 9)
                hexstring[j + 1] = 0x30 + ch;
            else
                hexstring[j + 1] = 0x37 + ch;
        }
    }
    hexstring[j] = 0x00;
}
int8_t Var_Bytes_Count(uint8_t *countbytesofthis) {
    static int8_t xrzk_bytescnt = 0;
    for (int8_t i = 0; i < 10; i++) {
        if (countbytesofthis[i] != 0) {
            xrzk_bytescnt++;
        }
    }
    return xrzk_bytescnt;
}
uint8_t *Num_To_Varint_64(uint64_t TOV64INPUT) // UINT64_T TO VARINT
{
    static uint8_t buffy_001[60] = {0};
    uvarint_encode64(TOV64INPUT, buffy_001, 60);
    return buffy_001;
}
uint8_t *Num_To_Varint_32(uint32_t TOV32INPUT) // UINT32_T TO VARINT
{
    static uint8_t buffy_032[60] = {0};
    uvarint_encode32(TOV32INPUT, buffy_032, 60);
    return buffy_032;
}
uint64_t *Varint_To_Num_64(uint8_t TON64INPUT[60]) // VARINT TO UINT64_t
{
    static uint64_t varintdecode_001 = 0;
    uvarint_decode64(TON64INPUT, 60, &varintdecode_001);
    return &varintdecode_001;
}
uint32_t *Varint_To_Num_32(uint8_t TON32INPUT[60]) // VARINT TO UINT32_t
{
    static uint32_t varintdecode_032 = 0;
    uvarint_decode32(TON32INPUT, 60, &varintdecode_032);
    return &varintdecode_032;
}
//
char *Int_To_Hex(uint64_t int2hex) // VAR[binformat] TO HEX
{
    static char int2hex_result[800] = "\0";
    memset(int2hex_result, 0, sizeof(int2hex_result));
    sprintf(int2hex_result, "%02lX", int2hex);
    return int2hex_result;
}
uint64_t Hex_To_Int(char *hax) {
    char *hex = NULL;
    hex = hax;
    uint64_t val = 0;
    while (*hex) {
        // get current character then increment
        uint8_t byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii
        // table indexes
        if (byte >= '0' && byte <= '9')
            byte = byte - '0';
        else if (byte >= 'a' && byte <= 'f')
            byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <= 'F')
            byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}
//
void vthconvert(int size, char *crrz01, uint8_t *xbuf) {
    uint8_t buf[400];
    bzero(buf, 400);

    // fixing the buf
    for (int cz = 0; cz < size; cz++) {
        buf[cz] = xbuf[cz];
    }
    //
    if (crrz01 != NULL) {
        char *crrz1 = NULL;
        crrz1 = crrz01;
        char conv_proc[800] = "\0";
        int i;
        for (i = 0; i < (size * 2); i++) {
            if (buf[i] != '\0') {
                sprintf(conv_proc, "%02X", buf[i]);
                // printf("%d:%d\n",i, buf[i]);
                strcat(crrz1, conv_proc);
            }
        }
        crrz1 = NULL;
    }
}
char *Var_To_Hex(int realsize, uint8_t *TOHEXINPUT) // VAR[binformat] TO HEX
{
    for (int ix = realsize; ix < 400; ix++) {
        TOHEXINPUT[ix] = '\0';
    }
    if (TOHEXINPUT != NULL) {
        static char convert_resultz1[800] = "\0";
        bzero(convert_resultz1, 800);
        vthconvert(realsize, convert_resultz1, TOHEXINPUT);
        return convert_resultz1;
    }
}
uint8_t *Hex_To_Var(char *Hexstr) // HEX TO VAR[BINFORMAT]
{
    static uint8_t buffy_HEX[400] = {0};
    bzero(buffy_HEX, 400);
    int i;
    char codo[800] = "\0";
    bzero(codo, 800);
    strcpy(codo, Hexstr);
    char code[3];
    bzero(code, 3);
    code[3] = '\0';
    int x = 0;
    int fori001 = 0;
    for (fori001 = 0; fori001 < 800; fori001++) {
        strncpy(&code[0], &codo[fori001], 1);
        strncpy(&code[1], &codo[fori001 + 1], 1);
        char *ck = NULL;
        uint64_t lu = 0;
        lu = strtoul(code, &ck, 16);
        buffy_HEX[x] = lu;
        // printf("%s - %lu\n",code,lu);
        fori001++;
        x++;
    }
    return buffy_HEX;
}
//
void convert(char *convert_result, uint8_t *buf) // Both of them read them properly.
{
    char conv_proc[800] = "\0";
    bzero(conv_proc, 800);
    int i;
    for (i = 0; i < 10; i++) {
        sprintf(conv_proc, "%02X", buf[i]);
        // printf("%d:%d\n",i, buf[i]);
        strcat(convert_result, conv_proc);
    }
}
char *Num_To_HexVar_64(uint64_t TOHVINPUT) // UINT64 TO HEXIFIED VAR
{                                          // Code to varint - py
    static char convert_result[800] =
        "\0"; // Note that the hex resulted from this will differ from py
    bzero(convert_result, 800);
    memset(convert_result, 0,
           sizeof(convert_result)); // But if you make sure the string is always 20
                                    // chars in size
    uint8_t buf[400] = {0};
    bzero(buf, 400);
    uvarint_encode64(TOHVINPUT, buf, 800);
    convert(convert_result, buf);
    return convert_result;
}
void convert2(char *convert_result2, uint8_t *bufhx) {
    uint8_t *buf = NULL;
    buf = bufhx;
    char conv_proc[3] = "\0";
    conv_proc[3] = '\0';
    bzero(conv_proc, 3);
    int i;
    for (i = 0; i == 0; i++) {
        sprintf(conv_proc, "%02X", buf[i]);
        // printf("aaaaaaaaaaah%d:%d\n",i, buf[i]);
        strcat(convert_result2, conv_proc);
    }
    buf = NULL;
}
char *Num_To_HexVar_32(uint32_t TOHVINPUT) // UINT32 TO HEXIFIED VAR
{                                          // Code to varint - py
    static char convert_result2[3] = "\0";
    bzero(convert_result2, 3);
    convert_result2[2] = '\0';
    memset(convert_result2, 0, sizeof(convert_result2));
    uint8_t buf[1] = {0};
    bzero(buf, 1);
    uvarint_encode32(TOHVINPUT, buf, 1);
    convert2(convert_result2, buf);
    return convert_result2;
}

uint64_t HexVar_To_Num_64(char *theHEXstring) // HEXIFIED VAR TO UINT64_T
{                                             // Varint to code - py
    uint8_t buffy[400] = {0};
    char codo[800] = "\0";
    bzero(codo, 800);
    strcpy(codo, theHEXstring);
    char code[3] = "\0";
    int x = 0;
    for (int i = 0; i < 399; i++) {
        strncpy(&code[0], &codo[i], 1);
        strncpy(&code[1], &codo[i + 1], 1);
        char *ck = NULL;
        uint64_t lu = 0;
        lu = strtoul(code, &ck, 16);
        buffy[x] = lu;
        i++;
        x++;
    }
    static uint64_t decoded;
    uvarint_decode64(buffy, 400, &decoded);
    return decoded;
}
uint32_t HexVar_To_Num_32(char theHEXstring[]) // HEXIFIED VAR TO UINT32_T
{                                              // Varint to code py
    uint8_t buffy[400] = {0};
    bzero(buffy, 400);
    char codo[800] = "\0";
    bzero(codo, 800);
    strcpy(codo, theHEXstring);
    char code[3] = "\0";
    bzero(code, 3);
    code[3] = '\0';
    int x = 0;
    for (int i = 0; i < 399; i++) {
        strncpy(&code[0], &codo[i], 1);
        strncpy(&code[1], &codo[i + 1], 1);
        char *ck = NULL;
        uint32_t lu = {0};
        lu = strtoul(code, &ck, 16);
        buffy[x] = lu;
        i++;
        x++;
    }
    static uint32_t decoded;
    uvarint_decode32(buffy, 10, &decoded);
    return decoded;
}

/*///Header - Usage Example:
 * unsigned char str[] = "/lol/meow/";
 * unsigned char strhex[((strlen(str)-1)*2)+1];
 * str2hex(strhex,str);
 * unsigned char header_result[sizeof(strhex+2)]; // Extra 2 for size
 * bzero(header_result, sizeof(header_result));
 * header(header_result, strhex);
 * printf("header - Result: %s\n", header_result);
 * unsigned char header_path_result[strlen(header_result)-2];
 */
int header(unsigned char *dest, unsigned char *src) {
    if (!src) {
        return 0;
    }
    char size[3];
    bzero(size, 3);
    strcpy(size, Int_To_Hex((strlen(src) / 2) - 1));
    strcat(dest, size);
    strcat(dest, src);
    return 1;
}
/*
 * 	//Header_Path - Usage Example
 * 	header_path(header_path_result,header_result);
 * 	printf("header_path - Result: %s\n",header_path_result);
 */
int header_path(unsigned char *dest, unsigned char *src) {
    if (!src) {
        return 0;
    }
    sprintf(dest, "%s", src + 2);
    return 1;
}
/*
 * 	//Add Header - Usage Example:
 * 	char continut[] = "1234";
 * 	char continuthex[((sizeof(continut)-1)*2)+1];
 * 	bzero(continuthex, sizeof(continuthex));
 * 	str2hex(continuthex, continut);
 * 	char cap[] = "/lol/";
 * 	char caphex[((sizeof(cap)-1)*2)+1];
 * 	bzero(caphex,sizeof(caphex));
 * 	str2hex(caphex,cap);
 * 	//printf("caphex = %s\n", caphex);
 * 	//printf("conhex = %s\n", continuthex);
 * 	char add_hdr_dest[sizeof(continuthex)-1+sizeof(caphex)-1+3];
 * 	bzero(add_hdr_dest, sizeof(add_hdr_dest));
 * 	add_header(add_hdr_dest, caphex,sizeof(caphex), continuthex,
 * sizeof(continuthex)); printf("add_header - Result: %s\n",add_hdr_dest);
 */
int add_header(unsigned char *dest, unsigned char *header, size_t headersize,
               unsigned char *content, size_t contentsize) {
    if (!header || !content) {
        return 0;
    }
    int realsize = (headersize - 1 + contentsize - 1) / 2;
    char szhex[3];
    bzero(szhex, 3);
    strcat(szhex, Int_To_Hex(realsize));
    strcat(dest, szhex);
    strcat(dest, header);
    strcat(dest, content);
    return 1;
}
/*//Remove Header - Usage Example:
 *  char remove_hdr_dst[1000]; //Much bigger than needed tbh..
 *  bzero(remove_hdr_dst,1000);
 *  remove_header(remove_hdr_dst, add_hdr_dest);
 *  printf("remove_header - Result: %s\n",remove_hdr_dst);
 */
int remove_header(unsigned char *dest, unsigned char *lel) {
    if (!lel) {
        return 0;
    }
    int init = 0;
    char s[(strlen(lel) / 2) + 1];
    int sizs = strlen(lel) / 2 + 1;
    hex2str(lel, s, sizs);
    sprintf(s, "%s", s + 1);
    char *pch;
    char *end;
    pch = strtok_r(s, "/", &end);
    int pos = 0;
    while (pch) {
        // printf("PCH: %s\n", pch);
        if (pos == 1) {
            str2hex(dest, pch);
        }
        // sprintf(dest,"%s",pch);
        pch = strtok_r(NULL, "/", &end);
        pos = 1;
    }
    return 1;
}
/* 	//getHeader
 *  char geth_dest[1000];
 *  bzero(geth_dest,1000);
 *  get_header(geth_dest, add_hdr_dest);
 *  printf("get_header - Result: %s\n", geth_dest);
 */
int get_header(char *dest, unsigned char *src) {
    if (!src) {
        return 0;
    }
    char ascii[strlen(src) / 2];
    hex2str(src, ascii, strlen(src) / 2);
    sprintf(ascii, "%s", ascii + 1);
    char *pch;
    char *end;
    pch = strtok_r(ascii, "/", &end);
    while (pch) {
        strcat(dest, pch);
        break;
    }
    return 1;
}
/*
 *  //getcontent
 *  char getc_dest[1000];
 *  bzero(getc_dest,1000);
 *  get_content(getc_dest, add_hdr_dest);
 *  printf("get_content - Result: %s\n", getc_dest);
 */
int get_content(char *dest, unsigned char *src) {
    if (!src) {
        return 0;
    }
    char ascii[strlen(src) / 2];
    hex2str(src, ascii, strlen(src) / 2);
    sprintf(ascii, "%s", ascii + 1);
    char *pch;
    char *end;
    int ax = 0;
    pch = strtok_r(ascii, "/", &end);
    while (pch) {
        // printf("PCH: %s", pch);
        if (ax == 1) {
            strcat(dest, pch);
            break;
        }
        ax = 1;
        pch = strtok_r(NULL, "/", &end);
    }
    return 1;
}
#endif
