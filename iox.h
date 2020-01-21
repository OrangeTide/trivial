#ifndef IOX_H_
#define IOX_H_
#define SOCKET int
#define IOX_OK (0)
#define IOX_ERR (-1)
#define IOX_EV_READ (1u << 0)
#define IOX_EV_WRITE (1u << 2)
typedef struct iox_loop iox_loop_t;
struct iox_ops { void (*event_cb)(SOCKET fd, unsigned iox_event_flags, void *ptr); };
int iox_init(void);
int iox_loop_open(iox_loop_t **loop);
void iox_loop_close(iox_loop_t *loop);
int iox_poll(iox_loop_t *loop, int msec);
int iox_event_add(iox_loop_t *loop, SOCKET fd, unsigned iox_event_flags);
int iox_event_del(iox_loop_t *loop, SOCKET fd);
int iox_register(SOCKET fd, const struct iox_ops *ops, void *ptr, void (*free_cb)(void *));
int iox_retain(SOCKET fd);
int iox_release(SOCKET fd);

/* used internally by iox components */
int iox_call(SOCKET fd, unsigned iox_event_flags);
int iox_socket_close(SOCKET fd);

#endif
