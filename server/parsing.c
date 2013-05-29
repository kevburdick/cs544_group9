#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

// structure to string and parsing test

/* message contents*/
/*
Type (16 bits)
Length (16)
Error Code (16)
Version (16)
Client ID (16)
File ID (32)
Pos (32)/Pos (64)????
Len (32)
TotalLen (32)
Checksum (256)
Data (variable)
Options (variable)
*/

typedef struct M2M_HEAD
{
    short int type;
    short int length;
    short int errorcode;
    short int version;
    short int clientID;
    short int fileID;
    short int checksum;

};

int M2M_HEAD_to_str(char **dest, struct M2M_HEAD *pHead)
{
    unsigned int size = 0;

    if (dest == NULL || pHead == NULL)
        return -1;

    size = 128 + 5 * sizeof(int) + 2 *sizeof(char);

    *dest = (char *)malloc(size);
    if (*dest == NULL)
        return -2;

    memset(*dest, 0, size);

    sprintf(*dest,
        "type=%d, "
        "length=%d, "
        "errorcode=%d, "
        "version=%d, "
        "clientID=%d, "
        "fileID=%d, "
        "checksum=%d"
        , pHead->type, pHead->length, pHead->errorcode, pHead->version,
        pHead->clientID, pHead->fileID, pHead->checksum);
    puts(*dest);
    return 0;
}

int M2M_HEAD_parse(char *str, struct M2M_HEAD **pHead)
{
    if (str == NULL || pHead == NULL)
        return -1;

    *pHead = (struct M2M_HEAD *)malloc(sizeof(struct M2M_HEAD));
    if (*pHead == NULL)
        return -2;

    memset(*pHead, 0, sizeof(struct M2M_HEAD));

    sscanf(str,
        "type=%d, "
        "length=%d, "
        "errorcode=%d, "
        "version=%d, "
        "clientID=%d, "
        "fileID=%d, "
        "checksum=%d"
        , &((*pHead)->type), &((*pHead)->length), &((*pHead)->errorcode), &((*pHead)->version),
        &((*pHead)->clientID), &((*pHead)->fileID), &((*pHead)->checksum));


    return 0;
}

int main(int argc, char *argv[])
{
    struct M2M_HEAD head_a;
    struct M2M_HEAD *p = NULL;
    char *str = NULL;
    int res;

    head_a.type = 3;
    head_a.length = 467;
    head_a.errorcode = 5;
    head_a.version = 1;
    head_a.clientID = 33;
    head_a.fileID = 23;
    head_a.checksum = 2644636;

    res = M2M_HEAD_to_str(&str, &head_a);
    if (res != 0)
    {
        printf("Failed to execute message_to_str");
        return -1;
    }

    res = M2M_HEAD_parse(str, &p);
    if (res != 0)
    {
        free(str);
        str = NULL;
    }

    free(p);
    p = NULL;

    return 0;
}
