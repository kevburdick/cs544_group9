#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <stdint.h>
#include "options.h"

/* Message types */
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

typedef struct __attribute__ ((__packed__)) CTP_head {
    uint16_t type;
    uint16_t length;
} CTP_head_t;

typedef struct __attribute__ ((__packed__)) CTP_ERROR {
    uint16_t error_code;
} CTP_ERROR_t;

typedef struct __attribute__ ((__packed__)) CTP_CONNECT {
    uint16_t version;
    options_t options;
} CTP_CONNECT_t;

typedef struct __attribute__ ((__packed__)) CTP_ACKCONNECT {
    uint16_t version;
    uint16_t client_id;
    options_t options;
} CTP_ACKCONNECT_t;

typedef struct __attribute__ ((__packed__)) CTP_OPEN {
    uint16_t client_id;
    uint32_t file_id;
    options_t options;
} CTP_OPEN_t;

struct __attribute__ ((__packed__)) CTP_serv_base {
    options_t options;
};

typedef struct CTP_serv_base CTP_ACKOPEN_t;
typedef struct CTP_serv_base CTP_ACKLOCK_t;
typedef struct CTP_serv_base CTP_ACKEDIT_t;
typedef struct CTP_serv_base CTP_SERVRELEASE_t;

typedef struct __attribute__ ((__packed__)) CTP_REQCONTENTS {
    uint16_t client_id;
    uint32_t pos;
    uint32_t len;
    options_t options;
} CTP_REQCONTENTS_t;

typedef struct __attribute__ ((__packed__)) CTP_CONTENTS {
    uint32_t totallen;
    uint32_t pos;
    size_t datalen;
    char *data;
} CTP_CONTENTS_t;

typedef struct __attribute__ ((__packed__)) CTP_MOVE {
    uint16_t client_id;
    uint32_t pos;
} CTP_MOVE_t;

struct __attribute__ ((__packed__)) CTP_cli_base {
    uint16_t client_id;
    options_t options;
};

typedef struct CTP_cli_opts_only CTP_REQLOCK_t;
typedef struct CTP_cli_opts_only CTP_RELEASE_t;
typedef struct CTP_cli_opts_only CTP_SYNC_t;

typedef struct __attribute__ ((__packed__)) CTP_STATUS {
    unsigned char checksum[32];
    options_t options;
} CTP_STATUS_t;

typedef struct __attribute__ ((__packed__)) CTP_EDIT {
    unsigned char edit_action;
    uint32_t pos;
    uint32_t len;
    size_t datalen;
    char *data;
} CTP_EDIT_t;

/* returns 1 on success, 0 on close, -1 on error */
int recv_head(int sockfd, struct CTP_head *head);

int recv_mesg(int sockfd, void *dest, CTP_head_t head);

int send_mesg(int sockfd, void *src, uint16_t type);

#endif
