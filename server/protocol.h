/* STATES */
typedef enum state {
    INIT,
    VERSION,
    CONNECTED,
    OPENING,
    OPEN,
    WAITLOCK,
    LOCKED
} state_t;

typedef struct client {
    int sockfd;
    uint16_t id;
    state_t state;
} client_t;

extern client_t[] clients;

void err_and_close(client_t *client, uint16_t error_code);

/* returns 1 if client still connected,
 *         0 if client closing,
 *         -1 if client sends invalid or malformed message */
int Init(client_t *client, textbuf *buf);
