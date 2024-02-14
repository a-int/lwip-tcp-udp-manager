#include "err.h"
#include "ip4_addr.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include "stdio.h"
#include "udpRAW.h"

extern struct netif gnetif;
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

struct udp_pcb* udp_server_init(ip4_addr_t* remoteIP, u16_t remotePort, u16_t localPort){
	struct udp_pcb* pcb = udp_new();
	
	ip4_addr_t localIP = gnetif.ip_addr;
	err_t err = udp_bind(pcb, &localIP, localPort); // bind the IP and port for PCB
	if(ERR_OK == err){
		udp_connect(pcb, remoteIP, remotePort);
		udp_recv(pcb, udp_receive_callback, NULL); // assign callback function if received data
	} else {
		udp_remove(pcb);
	}

	return pcb;
}

void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port){
	udp_send(upcb, p);
	pbuf_free(p); // free the received data buffer
}

void udp_connection_close(struct udp_pcb *upcb){
	udp_recv(upcb, NULL, NULL); // remove recv callback
	udp_disconnect(upcb); // remove remote IP
	udp_remove(upcb);
}
