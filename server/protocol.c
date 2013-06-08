#include "options.h"
#include "message.h"
#include "protocol.h"

#define PROTOCOL_VERSION 1

void
err_and_close(client_t *client, uint16_t error_code);

/* response.options MUST be free_options()ed! */
CTP_STATUS_t
get_STATUS(textbuf *buf)
{
    client_t *c;
    char id_buf[sizeof(client->id) + 1] = { 0 };
    CTP_STATUS_t response;
    /* XXX TODO set response.checksum */
    response.options = new_options();

    /* build list of peers in options */
    for (c = clients; c <= max_active_client; ++c) {
        if (c->active) {
            memcpy(id_buf, &c->id, sizeof(c->id));
            add_option(response.options, "Peer ID", id_buf);
            /* TODO Peer Nick? */
            if (c->state == Locked)
                add_option(response.options, "Lock ID", id_buf);
        }
    }

    return response;
}

int
Init_CONNECT(client_t *client, textbuf *buf, CTP_head_t head)
{
    /* get the message body */
    CTP_CONNECT_t mesg;
    int rv = recv_mesg(client->sockfd, &mesg, head);
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
    client->state = Version;

    /* build ACKCONNECT response */
    CTP_ACKCONNECT_t response;
    response.version = PROTOCOL_VERSION;
    response.client_id = client->id;
    response.options = new_options();

    /* send response */
    rv = send_ACKCONNECT(client->sockfd, response);
    free_options(response.options);
    client->state = Connected;
    return rv;
}

int
Connected_OPEN(client_t *client, textbuf *buf, CTP_head_t head)
{
    CTP_OPEN_t mesg;
    int rv = recv_mesg(client->sockfd, &mesg, head);
    if (rv < 1)
        return rv;

    /* check for correct client id */
    if (mesg.client_id != client->id) {
        free_mesg(mesg);
        return -1;
    }

    /* check file id
     * this server only supports a "default" file */
    if (mesg.file_id != 0) {
        free_mesg(mesg);
        send_err(client, BAD_FILE_ID);
        return 1;
    }

    free_mesg(mesg);

    /* transient... */
    client->state = Opening;

    /* build ACKOPEN response */
    CTP_ACKOPEN_t response;
    response.options = new_options();

    /* send response */
    rv = send_ACKOPEN(client->sockfd, response);
    free_options(response.options);
    client->state = Open;
    if (rv < 1)
        return rv;

    /* send STATUS message to all clients */
    CTP_STATUS_t status = get_status(buf);
    client_t *c;
    for (c = clients; c <= max_active_client; ++c) {
        if (c->active)
            send_STATUS(c->sockfd, status);
    }
    free_options(status.options);

    return 1;
}

int
Open_REQCONTENTS(client_t *client, textbuf *buf, CTP_head_t head)
{
    CTP_REQCONTENTS_t mesg;
    int rv = recv_mesg(client->sockfd, &mesg, head);
    if (rv < 1)
        return rv;

    /* check for correct client id */
    if (mesg.client_id != client->id) {
        free_mesg(mesg);
        return -1;
    }

    /* TODO implement IfChanged option handling */

    /* Len of 0 -> until end of file */
    if (mesg.len == 0)
        mesg.len = buf->len;

    /* cap pos and len to buffer size */
    unsigned int pos = MIN(mesg.pos, buf->len);
    unsigned int len = MIN(mesg.len, buf->len - pos);

    free_mesg(mesg);

    unsigned int chunk, numChunks = len / CHUNKSIZE + 1;

    CTP_CONTENTS_t contents;
    contents.totallen = buf->len;
    for (chunk = 0; chunk < numChunks; ++chunk) {
        char databuf[CHUNKSIZE]; /* TODO consider placement */
        contents.pos = pos + CHUNKSIZE * chunk;
        contents.datalen = tbread(buf, databuf, CHUNKSIZE);
        contents.data = databuf;
        rv = send_CONTENTS(client->sockfd, contents);
        if (rv < 1)
            return rv;
    }

    return 1;
}

int
Open_REQLOCK(client_t *client, textbuf *buf, CTP_head_t head)
{
    CTP_OPEN_t mesg;
    int rv = recv_mesg(client->sockfd, &mesg, head);
    if (rv < 1)
        return rv;

    /* check for correct client id */
    if (mesg.client_id != client->id) {
        free_mesg(mesg);
        return -1;
    }

    /* only one client may lock at a time */
    if (locking_client != NULL) {
        free_mesg(mesg);
        send_err(client, CANT_LOCK);
        return 1;
    }

    free_mesg(mesg);

    /* transient... */
    client->state = WaitLock;

    /* build ACKLOCK response */
    CTP_ACKLOCK_t response;
    response.options = new_options();

    /* send response */
    rv = send_ACKOPEN(client->sockfd, response);
    free_options(response.options);
    client->state = Locked;
    locking_client = client;
    return rv;
}

/* While this is invalid, we may still get it in this state from a
 * well-behaved client if we send a SERVRELEASE at about the same
 * time the client sends an EDIT. So we reject it and continue. */
int
Open_EDIT(client_t *client, textbuf *buf, CTP_head_t head)
{
    CTP_EDIT_t mesg;
    int rv = recv_mesg(client->sockfd, &mesg, head);
    if (rv < 1)
        return rv;

    rv = send_err(client, REJECT_EDIT);
    free_mesg(mesg);
    return rv;
}

int
Locked_RELEASE(client_t *client, textbuf *buf, CTP_head_t head)
{
    CTP_RELEASE_t mesg;
    int rv = recv_mesg(client->sockfd, &mesg, head);
    if (rv < 1)
        return rv;

    /* check for correct client id */
    if (mesg.client_id != client->id) {
        free_mesg(mesg);
        return -1;
    }

    free_mesg(mesg);
    client->state = Open;
    return 1;
}

int
Locked_EDIT(client_t *client, textbuf *buf, CTP_head_t head)
{
    CTP_EDIT_t mesg;
    int rv = recv_mesg(client->sockfd, &mesg, head);
    if (rv < 1)
        return rv;

    /* check for correct client id */
    if (mesg.client_id != client->id) {
        free_mesg(mesg);
        return -1;
    }

    switch (mesg.edit_action) {
        case INS:
            rv = overwrite(buf, mesg.pos, 0, mesg.data, mesg.datalen);
            break;
        case OVR:
            rv = overwrite(buf, mesg.pos, mesg.len, mesg.data, mesg.datalen);
            break;
        case DEL:
            rv = overwrite(buf, mesg.pos, mesg.len, NULL, 0);
            break;
    }

    if (rv < 0) {
        /* reject the edit */
        free_mesg(mesg);
        rv = send_err(client, REJECT_EDIT);
    } else {

        /* prepare and send ACKEDIT */
        CTP_ACKEDIT_t response;
        response.options = new_options();
        rv = send_ACKEDIT(client->sockfd, response);
        free_options(response.options);

        /* relay EDIT to other clients */
        client_t *c;
        for (c = clients; c <= max_active_client; ++c) {
            if (c != client && c->active)
                send_EDIT(c->sockfd, mesg);
        }
    }

    free_mesg(mesg);
    return rv;
}

int
Any_MOVE(client_t *client, textbuf *buf, CTP_head_t head)
{
    client_t c;
    CTP_MOVE_t mesg;
    int rv = recv_mesg(client->sockfd, &mesg, head);
    if (rv < 1)
        return rv;

    /* check for correct client id */
    if (mesg.client_id != client->id) {
        free_mesg(mesg);
        return -1;
    }

    /* relay to all other clients */
    for (c = clients; c <= max_active_client; ++c)
        if (c != client && c->active)
            send_MOVE(c->sockfd, mesg);

    free_mesg(mesg);

    return 1;
}

int
Any_SYNC(client_t *client, textbuf *buf, CTP_head_t head)
{
    client_t c;
    CTP_MOVE_t mesg;
    int rv = recv_mesg(client->sockfd, &mesg, head);
    if (rv < 1)
        return rv;

    /* check for correct client id */
    if (mesg.client_id != client->id) {
        free_mesg(mesg);
        return -1;
    }

    free_mesg(mesg);

    /* get STATUS response */
    CTP_STATUS_t response = get_STATUS(buf);
    
    /* send response */
    rv = send_STATUS(client->sockfd, response);
    free_options(response.options);
    return rv;
}

/* STATEFUL
 * Dispatches to different functions based on incoming message type and
 * current state. */ 

int
handle_client_mesg(client_t *client, textbuf *buf)
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

    case Init: switch (head.type) {
        case CONNECT:
            return Init_CONNECT(client, buf, head);
        }
        break;

    case Version: /* we don't receive any messages in this state */
        break; 

    case Connected: switch (head.type) {
        case OPEN:
            return Connected_OPEN(client, buf, head);
        }
        break;

    case Opening: /* we don't receive any messages in this state */
        break;
    }

    case Open: switch (head.type) {
        case REQCONTENTS:
            return Open_REQCONTENTS(client, buf, head);
        case MOVE:
            return Any_MOVE(client, buf, head);
        case REQLOCK:
            return Open_REQLOCK(client, buf, head);
        case SYNC:
            return Any_SYNC(client, buf, head);
        case EDIT:
            /* see the note on Open_EDIT */
            return Open_EDIT(client, buf, head;
        }
        break;

    case WaitLock: /* we don't receive any messages in this state */
        break;

    case Locked: switch (head.type) {
        case MOVE:
            return Any_MOVE(client, buf, head);
        case RELEASE:
            return Locked_RELEASE(client, buf, head);
        case SYNC:
            return Any_SYNC(client, buf, head);
        case EDIT:
            return Locked_EDIT(client, buf, head);
        }
        break;

    return -1;
}
