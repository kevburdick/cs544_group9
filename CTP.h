#ifndef CTP_H_INCLUDED
#define CTP_H_INCLUDED

#define	CTP_SERVER_PORT	XX	/* from client to server */
#define CTP_CLIENT_PORT	XX	/* from server to client */


/* CTP error codes */
#define BadMessage            1
#define UnsupportedVersion    2
#define BadFileID             3
#define CantLock              4
#define RejectEdit            5

/* CTP message type */
#define ERROR                 1
#define CONNECT               2
#define ACKCONNECT            3
#define OPEN                  4
#define ACKOPEN               5
#define ACKLOCK               6
#define ACKEDIT               7
#define SERVRELEASE           8
#define REQCONTENTS           9
#define CONTENTS              10
#define MOVE                  11
#define REQLOCK               12
#define RELEASE               13
#define SYNC                  14
#define STATUS                15
#define EDIT                  16
#define CLOSE                 17

/* CTP Edit Actions */
#define INS                   1
#define OVR                   2
#define DEL                   3

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



typedef struct ctpMessage
{

} __attribute__((packed)) ctpMessage;


/*Options fields*/
typedef struct ctpOptions
{
  unsigned int num;
  void   *val[256];
} __attribute__((packed)) ctpOptions;


/*CTP status*/
void *Init();
void *Version();
void *Connected();
void *Opening();
void *Open();
void *Wait_lock();
void *Locked();
void *Error();


#endif // CTP_H_INCLUDED
