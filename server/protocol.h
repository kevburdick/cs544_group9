/* STATES */
typedef enum state {
    Init,
    Version,
    Connected,
    Opening,
    Open,
    WaitLock,
    Locked
} state_t;

typedef struct client {
    int sockfd;
    uint16_t id;
    state_t state;
    bool active;
} client_t;

/* ??? */
extern client_t[] clients;
extern client_t *max_active_client, *locking_client;

void err_and_close(client_t *client, uint16_t error_code);

/* returns 1 if client still connected,
 *         0 if client closing,
 *         -1 if client sends invalid or malformed message */
int handle_client_mesg(client_t *client, textbuf *buf);
