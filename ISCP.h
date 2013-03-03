#ifndef ISCP_H
#define ISCP_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ISCP_PORT 60128

class ISCP
{
public:
    ISCP();

    struct MSG {
        struct {
            char id[4];          // "ISCP"
            uint32_t headerSize; // Big-Endian header size
            uint32_t dataSize;   // Big-Endian data size
            uint8_t  version;    // 1
            uint8_t  reserved[3];
        } header;
        struct {
            uint8_t start;       // '!'
            uint8_t dest;        // '1' == Receiver
            char  cmd[3];        // Three character command string
            char  param[];       // Variable length parameter, terminated with \r\n
        } data;
    };
};

#endif // ISCP_H
