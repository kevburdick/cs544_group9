#ifndef CTP_H_INCLUDED
#define CTP_H_INCLUDED

#define	CTP_PORT	5449


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



typedef struct ErrorMessage
{
    short int type;
    short int length;
    short int error_code;

} __attribute__((packed)) ErrorMessage;

typedef struct ConnectMessage
{
    short int type;
    short int length;
    short int version;
    //void *option[];
} __attribute__((packed)) ConnectMessage;


typedef struct AckconnectMessage
{
    short int type;
    short int length;
    short int version;
    short int client_id;
    //void *option[];

} __attribute__((packed)) AckconnectMessage;

typedef struct OpenMessage
{
    short int type;
    short int length;
    short int client_id;
    int file_id;
    //void *option[];

} __attribute__((packed)) OpenMessage;


typedef struct AckMessage
{
    short int type;
    short int length;
    //void *option[];

} __attribute__((packed)) AckMessage;

typedef struct ContentsMessage
{
    short int type;
    short int length;
    int totallen;
    int pos;
    //void *data[];

} __attribute__((packed)) ContentsMessage;

typedef struct MoveMessage
{
    short int type;
    short int length;
    short int client_id;
    __int64 pos;//maybe wrong
    //void *data[];

} __attribute__((packed)) MoveMessage;

typedef struct StatusMessage
{
    short int type;
    short int length;
    //checksum??
    //void *options[];

} __attribute__((packed)) StatusMessage;

typedef struct EditMessage
{
    short int type;
    short int length;
    char edit_action;
    int pos;
    int len;
    //checksum??
    //void *data[];

} __attribute__((packed)) EditMessage;

typedef struct CloseMessage
{
    short int type;
    short int length;

} __attribute__((packed)) CloseMessage;

/*Options fields*/
typedef struct ctpOptions
{
  unsigned int num;
  void   *val[256];
} __attribute__((packed)) ctpOptions;

typedef struct ReleaseMessage
{
    short int type;
    short int length;
    short int client_id;
    //void *option[];

} __attribute__((packed)) ReleaseMessage;

/*CTP status*/
void *Init();
void *Version(ConnectMessage *);
void *Connected();
void *Opening();
void *Open();
void *Wait_lock();
void *Locked();
void *Error();


#endif // CTP_H_INCLUDED
