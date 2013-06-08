#include "options.h"
#include "message.h"
#include "protocol.h"

#define PROTOCOL_VERSION 1

void
err_and_close(client_t *client, uint16_t error_code);

int
Init_CONNECT(client_t *client, textbuf *buf, CTP_head_t head)
{
    /* get the message body */
    CTP_CONNECT_t mesg;
    rv = recv_mesg(client->sockfd, &mesg, head);
    if (rv < 1)
        return rv;

    /* check version: only support current version */
    if (mesg.version != PROTOCOL_VERSION) {
        free_mesg(mesg);
        err_and_close(client, UNSUPPORTED_VERSION);
        return 0;
    }

    free_mesg(mesg);

    /* transient... */
    client->state = VERSION;

    /* build ACKCONNECT response */
    CTP_ACKCONNECT_t response;
    response.version = PROTOCOL_VERSION;
    response.client_id = client->id;
    response.options = new_options();

    /* send response */
    rv = send_ACKCONNECT(client->sockfd, response);
    free_options(response.options);
    client->state = CONNECTED;
    return rv;
}

int
Connected_OPEN(client_t *client, textbuf *buf, CTP_head_t head)
{
    CTP_OPEN_t mesg;
    rv = recv_mesg(client->sockfd, &mesg, head);
    if (rv < 1)
        return rv;

    /* ... */
}

int
Handle(client_t *client, textbuf *buf)
{
    int rv;
    CTP_head_t head;

    /* get message head 
     * (doing this first lets us ignore the body if it's invalid) */
    rv = recv_head(client->sockfd, &head);
    if (rv < 1)
        return rv;

    /* dispatch to appropriate function */
    switch (client->state) {
    case INIT:
        switch (head.type) {
        case CONNECT:
            return Init_CONNECT(client, buf, head);
        }
    }

    return -1;
}

int
Init(client_t *client, textbuf *buf)
{
    int rv;
    CTP_head_t head;

    rv = recv_head(client->sockfd, &head);
    if (rv < 1)
        return rv;

    switch (head.type) {
        case CONNECT:
            return Init_CONNECT(client, buf, head);
    }

    return -1;
}

int
CONNECTED(client_t *client, textbuf *buf)
{
    int rv;
    CTP_head_t head;

    rv = recv_head(client->sockfd, &head);
    if (rv < 1)
        return rv;

    switch (head.type) {

        case OPEN:

            CTP_OPEN_t mesg;
            rv = recv_mesg
    }
}
