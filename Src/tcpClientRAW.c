#include "pbuf.h"
#include "tcpRAW.h"
#include "err.h"
#include "lwip/tcp.h"

extern struct tcp_pcb* tcpListeningPCB[];

enum tcp_client_states
{
  ES_NONE = 0,
  ES_CONNECTED,
  ES_CLOSING
};


static err_t tcp_client_connected(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_client_error(void *arg, err_t err);
static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_client_handle (struct tcp_pcb *tpcb, struct tcp_connection_struct *es);

struct tcp_pcb* tcp_client_init(ip_addr_t* ip, u16_t port)
{
	struct tcp_pcb* tpcb = tcp_new();
	err_t err = tcp_connect(tpcb, ip, port, tcp_client_connected);
  return tpcb;
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *newpcb, err_t err) {
  err_t ret_err;
  struct tcp_connection_struct *es = (struct tcp_connection_struct *)mem_malloc(sizeof(struct tcp_connection_struct));
  if (es != NULL)
  {
    es->state = ES_CONNECTED;
    es->pcb = newpcb;
    es->p = NULL;
    
    tcp_arg(newpcb, es);
    tcp_recv(newpcb, tcp_client_recv);
    tcp_err(newpcb, tcp_client_error);
    tcp_poll(newpcb, tcp_client_poll, 0);
    tcpListeningPCB[1] = newpcb;
    ret_err = ERR_OK;
  }
  else
  {
    tcp_close(newpcb);
    ret_err = ERR_MEM;
  }
  return ret_err;
}

static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  struct tcp_connection_struct *es = (struct tcp_connection_struct *)arg;
  err_t ret_err;

  if (p == NULL)
  {
    es->state = ES_CLOSING;
    if(es->p == NULL) {
       tcp_connection_close(tpcb);
    } else {
      tcp_sent(tpcb, tcp_client_sent);
      tcp_send(tpcb, es);
    }
    ret_err = ERR_OK;
  } else if(es->state == ES_CONNECTED) {
    if(es->p == NULL) {
      es->p = p;
      tcp_client_handle(tpcb, es);
    } else {
      pbuf_chain(es->p, p);
    } 
    ret_err = ERR_OK;
  } 
  return ret_err;
}

static void tcp_client_error(void *arg, err_t err) {
  struct tcp_connection_struct *es  = (struct tcp_connection_struct *)arg;
  if (es != NULL) {
    mem_free(es);
  }
}

static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb) {
  return ERR_OK;
}

static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
  struct tcp_connection_struct *es = (struct tcp_connection_struct *)arg;
  if(es->p != NULL) {
    tcp_sent(tpcb, tcp_client_sent);
    tcp_send(tpcb, es);
  }
  return ERR_OK;
}

static void tcp_client_handle(struct tcp_pcb *tpcb, struct tcp_connection_struct *es)
{
	char buf[es->p->len + 30];
	int len = sprintf(buf, "%s + Hello from TCP SERVER\n", (const char*)es->p->payload);
  es->p->payload = buf;
	es->p->tot_len = (es->p->tot_len - es->p->len) + len;
	es->p->len = len;
	tcp_send(tpcb, es);
}
