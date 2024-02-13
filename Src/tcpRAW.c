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