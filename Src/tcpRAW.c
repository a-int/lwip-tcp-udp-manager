#include "tcpRAW.h"

void tcp_connection_close(struct tcp_pcb *tpcb)
{
  struct tcp_server_struct *es = (struct tcp_server_struct *)tpcb->callback_arg;
  if(es != NULL){
    mem_free(es);
  }
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  tcp_close(tpcb);
}

void tcp_send(struct tcp_pcb *tpcb, struct tcp_connection_struct *es) {
  while ((es->p != NULL) && (es->p->len <= tcp_sndbuf(tpcb))) {
    struct pbuf *ptr = es->p;
    u16_t plen = ptr->len;
    tcp_write(tpcb, ptr->payload, ptr->len, TCP_WRITE_FLAG_COPY);
    es->p = ptr->next;
    if(es->p != NULL) {
      pbuf_ref(es->p);
    }
    while(pbuf_free(ptr));
    tcp_recved(tpcb, plen);
  }
}