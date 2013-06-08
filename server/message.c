#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "message.h"

#define NTOHS(x) (x = ntohs(x))
#define NTOHL(x) (x = ntohl(x))
#define HTONS(x) (x = htons(x))
#define HTONL(x) (x = htonl(x))

ssize_t
full_recv(int sockfd, void *buf, size_t len, int flags)
{
    ssize_t recvd = 0, rv = 0;

    flags |= MSG_WAITALL;

    while (recvd < len) {
        rv = recv(sockfd, buf, len, flags);
        if (rv < 0) {
            if (errno != EINTR)
                return rv;
        } else if (rv == 0)
            return recvd;
        else
            recvd += rv;
    }

    return recvd;
}

ssize_t
full_send(int sockfd, const void *buf, size_t len, int flags)
{
    ssize_t sent = 0, rv = 0;

    while (sent < len) {
        rv = send(sockfd, buf, len, flags);
        if (rv < 0) {
            if (errno != EINTR)
                return rv;
        } else if (rv == 0)
            return sent;
        else
            sent += rv;
    }

    return sent;
}

int
recv_head(int sockfd, CTP_head_t *head)
{
    ssize_t rv = full_recv(sockfd, head, sizeof(*head), 0);
    head->type = ntohs(head->type);
    head->length = ntohs(head->length);
    if (rv < 0)
        return -1;
    if (rv < sizeof(*head))
        return 0;
    return 1;
}

/* auxiliaries for recv_mesg
 * all return 0 on success, -1 on error */

inline int
recv_ERROR(CTP_ERROR_t *mesg, char *rest, int restlen)
{
    mesg->error_code = ntohs(mesg->error_code);
    return restlen ? -1 : 1;
}

inline int
recv_CONNECT(CTP_CONNECT_t *mesg, char *rest, int restlen)
{
    mesg->version = ntohs(mesg->version);
    return parse_options(rest, restlen, &mesg->options);
}

inline int
recv_ACKCONNECT(CTP_ACKCONNECT_t *mesg, char *rest, int restlen)
{
    mesg->version = ntohs(mesg->version);
    mesg->client_id = ntohs(mesg->client_id);
    return parse_options(rest, restlen, &mesg->options);
}

inline int
recv_OPEN(CTP_OPEN_t *mesg, char *rest, int restlen)
{
    mesg->client_id = ntohs(mesg->client_id);
    mesg->file_id = ntohl(mesg->file_id);
    return parse_options(rest, restlen, &mesg->options);
}

inline int
recv_serv_base(struct CTP_serv_base *mesg, char *rest, int restlen)
{
    return parse_options(rest, restlen, &mesg->options);
}

inline int
recv_REQCONTENTS(CTP_REQCONTENTS_t *mesg, char *rest, int restlen)
{
    mesg->client_id = ntohs(mesg->client_id);
    mesg->pos = ntohl(mesg->pos);
    mesg->len = ntohl(mesg->len);
    return parse_options(rest, restlen, &mesg->options);
}

inline int
recv_CONTENTS(CTP_CONTENTS_t *mesg, char *rest, int restlen)
{
    mesg->totallen = ntohl(mesg->totallen);
    mesg->pos = ntohl(mesg->pos);
    mesg->datalen = restlen;
    mesg->data = rest;
    return 1;
}

inline int
recv_MOVE(CTP_MOVE_t *mesg, char *rest, int restlen)
{
    mesg->client_id = ntohs(mesg->client_id);
    mesg->pos = ntohl(mesg->pos);
    return restlen ? -1 : 1;
}

inline int
recv_cli_base(struct CTP_cli_base *mesg, char *rest, int restlen)
{
    mesg->client_id = ntohs(mesg->client_id);
    return parse_options(rest, restlen, &mesg->options);
}

inline int
recv_STATUS(CTP_STATUS_t *mesg, char *rest, int restlen)
{
    return parse_options(rest, restlen, &mesg->options);
}

inline int
recv_EDIT(CTP_EDIT_t *mesg, char *rest, int restlen)
{
    mesg->pos = ntohl(mesg->pos);
    mesg->len = ntohl(mesg->len);
    mesg->datalen = restlen;
    mesg->data = rest;
    return 1;
}

/* length (bytes) of "constant" parts of messages */
size_t sizes[] = {
    0,  /* (empty) */
    2,  /* ERROR */
    2,  /* CONNECT */
    4,  /* ACKCONNECT */
    6,  /* OPEN */
    0,  /* ACKOPEN */
    0,  /* ACKLOCK */
    0,  /* ACKEDIT */
    0,  /* SERVRELEASE */
    10, /* REQCONTENTS */
    8, /* CONTENTS */
    6,  /* MOVE */
    2,  /* REQLOCK */
    2,  /* RELEASE */
    2,  /* SYNC */
    32, /* STATUS */
    9, /* EDIT (eww...) */
    0   /* CLOSE */
};

int
recv_mesg(int sockfd, void *dest, CTP_head_t head)
{
    /* receive from sock and fill message struct at dest */
    size_t expected = sizes[head.type];
    ssize_t rv = full_recv(sockfd, dest, expected, 0);
    if (rv < 0) 
        return -1;
    if (rv < expected)
        return 0;

    /* receive variable-length data part, if any */
    expected = head.length - sizes[head.type];
    char *rest = malloc(expected);
    rv = full_recv(sockfd, rest, expected, 0);
    if (rv < 0) 
        return -1;
    if (rv < expected)
        return 0;

    /* message-specific handling */
    switch (head.type) {
        case ERROR:
            return recv_ERROR(dest, rest, expected);
        case CONNECT:
            return recv_CONNECT(dest, rest, expected);
        case ACKCONNECT:
            return recv_ACKCONNECT(dest, rest, expected);
        case OPEN:
            return recv_OPEN(dest, rest, expected);
        case ACKOPEN: case ACKLOCK: case ACKEDIT: case SERVRELEASE:
            return recv_serv_base(dest, rest, expected);
        case REQCONTENTS:
            return recv_REQCONTENTS(dest, rest, expected);
        case CONTENTS:
            return recv_CONTENTS(dest, rest, expected);
        case MOVE:
            return recv_MOVE(dest, rest, expected);
        case REQLOCK: case RELEASE: case SYNC:
            return recv_cli_base(dest, rest, expected);
        case STATUS:
            return recv_STATUS(dest, rest, expected);
        case EDIT:
            return recv_EDIT(dest, rest, expected);
    }

    /* something else--failure */
    return -1;
}

/* send a message in parts; used by send_* */
inline int
send_parts(int sockfd, CTP_head_t *head,
        void *mesg, size_t mesglen, char *rest, size_t restlen)
{
    size_t rv;

    /* send head */
    rv = full_send(sockfd, head, sizeof(*head), 0);
    if (rv < 0) return -1;
    if (rv < sizeof(head)) return 0;

    /* send constant-length part of message */
    rv = full_send(sockfd, mesg, mesglen, 0);
    if (rv < 0) return -1;
    if (rv < mesglen) return 0;

    /* send variable-length part of message */
    rv = full_send(sockfd, rest, restlen, 0);
    if (rv < 0) return -1;
    if (rv < restlen) return 0;

    return 1;
}

int
send_ERROR(int sockfd, CTP_ERROR_t mesg)
{
    const size_t size = 2;

    CTP_head_t head;

    head.type = ERROR;
    head.length = sizeof(head) + size + 0;

    HTONS(mesg.error_code);

    return send_parts(sockfd, &head, &mesg, size, NULL, 0);
}

int
send_ACKCONNECT(int sockfd, CTP_ACKCONNECT_t mesg)
{
    const size_t size = 4;

    CTP_head_t head;
    size_t optlen, rv;
    char *optstr;

    optlen = print_options(mesg.options, &optstr);

    head.type = ACKCONNECT;
    head.length = sizeof(head) + size + optlen;

    HTONS(mesg.version);
    HTONS(mesg.client_id);

    rv = send_parts(sockfd, &head, &mesg, size, optstr, optlen);
    free(optstr);
    return rv;
}

int
send_ACKOPEN(int sockfd, CTP_ACKOPEN_t mesg)
{
    const size_t size = 0;

    CTP_head_t head;
    size_t optlen, rv;
    char *optstr;

    optlen = print_options(mesg.options, &optstr);

    head.type = ACKOPEN;
    head.length = sizeof(head) + size + optlen;

    rv = send_parts(sockfd, &head, &mesg, size, optstr, optlen);
    free(optstr);
    return rv;
}

int
send_ACKLOCK(int sockfd, CTP_ACKLOCK_t mesg)
{
    const size_t size = 0;

    CTP_head_t head;
    size_t optlen, rv;
    char *optstr;

    optlen = print_options(mesg.options, &optstr);

    head.type = ACKLOCK;
    head.length = sizeof(head) + size + optlen;

    rv = send_parts(sockfd, &head, &mesg, size, optstr, optlen);
    free(optstr);
    return rv;
}

int
send_ACKEDIT(int sockfd, CTP_ACKEDIT_t mesg)
{
    const size_t size = 0;

    CTP_head_t head;
    size_t optlen, rv;
    char *optstr;

    optlen = print_options(mesg.options, &optstr);

    head.type = ACKEDIT;
    head.length = sizeof(head) + size + optlen;

    rv = send_parts(sockfd, &head, &mesg, size, optstr, optlen);
    free(optstr);
    return rv;
}

int
send_SERVRELEASE(int sockfd, CTP_SERVRELEASE_t mesg)
{
    const size_t size = 0;

    CTP_head_t head;
    size_t optlen, rv;
    char *optstr;

    optlen = print_options(mesg.options, &optstr);

    head.type = SERVRELEASE;
    head.length = sizeof(head) + size + optlen;

    rv = send_parts(sockfd, &head, &mesg, size, optstr, optlen);
    free(optstr);
    return rv;
}

int
send_CONTENTS(int sockfd, CTP_CONTENTS_t mesg)
{
    const size_t size = 8;
    size_t rv;

    CTP_head_t head;

    head.type = CONTENTS;
    head.length = sizeof(head) + size + mesg.datalen;

    HTONL(mesg.totallen);
    HTONL(mesg.pos);

    return send_parts(sockfd, &head, &mesg, size, mesg.data, mesg.datalen);
}

int
send_MOVE(int sockfd, CTP_MOVE_t mesg)
{
    const size_t size = 6;
    size_t rv;

    CTP_head_t head;

    head.type = MOVE;
    head.length = sizeof(head) + size;

    return send_parts(sockfd, &head, &mesg, size, NULL, 0);
}

int
send_STATUS(int sockfd, CTP_STATUS_t mesg)
{
    const size_t size = 32;

    CTP_head_t head;
    size_t optlen, rv;
    char *optstr;

    optlen = print_options(mesg.options, &optstr);

    head.type = STATUS;
    head.length = sizeof(head) + size + optlen;

    return send_parts(sockfd, &head, &mesg, size, optstr, optlen);
}

int
send_EDIT(int sockfd, CTP_EDIT_t mesg)
{
    const size_t size = 9;
    size_t rv;

    CTP_head_t head;

    head.type = EDIT;
    head.length = sizeof(head) + size + mesg.datalen;

    HTONL(mesg.pos);
    HTONL(mesg.len);

    return send_parts(sockfd, &head, &mesg, size, mesg.data, mesg.datalen);
}
