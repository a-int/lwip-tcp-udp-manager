#include "err.h"
#include "ip4_addr.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include "stdio.h"
#include "udpRAW.h"

void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

struct tcp_pcb* udp_server_init(ip4_addr_t* ip, u16_t port){
	struct udp_pcb* pcb = udp_new();

	err_t err = udp_bind(pcb, ip, port); // bind the IP and port for PCB
	if(err == ERR_OK){
		udp_recv(pcb, udp_receive_callback, NULL); // assign callback function if received data
	} else {
		udp_remove(pcb);
	}
}

void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port){
	struct pbuf* txbuffer;
	char* buffer[100];
	u32_t len = sprintf(buffer, "TEST OF UDP %s", (char*)p->payload);
	txbuffer = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM); // allocate the neccessary memory block
	pbuf_take(txbuffer, buffer, len); // copy the data to allocated memory

	udp_connect(upcb, addr, port);
	udp_send(upcb, txbuffer);
	udp_disconnect(upcb);
	
	pbuf_free(txbuffer); // free the allocated buffer
	pbuf_free(p); // free the received data buffer
}

void udp_connection_close(struct tcp_pcb *tpcb){
	udp_recv(tpcb, NULL, NULL); // remove recv callback
	udp_disconnect(tpcb); // remove remote IP
	udp_remove(tpcb);
}
