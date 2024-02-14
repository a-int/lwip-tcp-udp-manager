// #FIXME correct comments [0] and [1] not always a server
// #FIXME replace 0 and 1 index with appropriate name
// #FIXME replace all magic numbers
#include "http_ssi.h"
#include "tcpRAW.h"
#include "udpRAW.h"
#include "lwip/tcp.h"
#include "lwip/apps/httpd.h"
#include "stm32f4xx_hal.h"

static const char *Start_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
static const char *Close_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
static const char *Send_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

enum server_state
{
	DEFAULT = 0,
	TCP_SERVER,
	TCP_CLIENT,
	UDP_SERVER,
	UDP_CLIENT,
};

enum server_state st = DEFAULT;

#define numForms 9
const tCGI TCP_SERVER_START_FORM = {"/tcpServerStart.cgi", Start_Handler};
const tCGI TCP_SERVER_CLOSE_FORM = {"/tcpServerClose.cgi", Close_Handler};
const tCGI TCP_CLIENT_START_FORM = {"/tcpClientStart.cgi", Start_Handler};
const tCGI TCP_CLIENT_CLOSE_FORM = {"/tcpClientClose.cgi", Close_Handler};
const tCGI UDP_SERVER_START_FORM = {"/udpServerStart.cgi", Start_Handler};
const tCGI UDP_SERVER_CLOSE_FORM = {"/udpServerClose.cgi", Close_Handler};
const tCGI TCP_SERVER_SEND_FORM = {"/tcpSendMessageServer.cgi", Send_Handler};
const tCGI TCP_CLIENT_SEND_FORM = {"/tcpSendMessageClient.cgi", Send_Handler};
const tCGI UDP_SERVER_SEND_FORM = {"/udpSendMessageServer.cgi", Send_Handler};

tCGI tcpCGIHandlersArray[numForms];

// each listening port contain 1 listening pcb and 1 for connected user
struct tcp_pcb* tcpListeningPCB[2] = {0};

//convert str to int
int atoi(const char* str){
	int res = 0;
	for(u16_t i = 0; str[i] != '\0'; ++i){
		if((str[i] >= '0') && (str[i] <= '9')){
			res = res*10 + (str[i] - '0');
		} else {
			res = -1;
			break;
		}
	}	
	return res;
}

static const char* Start_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
	ip4_addr_t localIP;
	int localPort;

	//extract the provided IP and Port
	for (int i=0; i<iNumParams; i++) {
		if (strcmp(pcParam[i], "ip") == 0) {
			ip4addr_aton(pcValue[i], &localIP);
		} else if (strcmp(pcParam[i], "port") == 0)	{
			localPort = atoi(pcValue[i]);
		}
	}
	if(localPort == -1){
		return "/404.html";
	}
	
	if(iIndex == 0){
		tcpListeningPCB[0] = tcp_server_init(&localIP, localPort); // start TCP Server and save listening PCB
		st = TCP_SERVER;
	} else if(iIndex == 2){
		tcpListeningPCB[0] = tcp_client_init(&localIP, localPort); // start TCP Client and save listening PCB	
		st = TCP_CLIENT;
	} else if(iIndex == 4){
		tcpListeningPCB[0] = udp_server_init(&localIP, localPort); // start TCP Server and save listening PCB
		st = UDP_SERVER;
	}
	return "/cgipage.html";
}

static const char* Close_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
	if(iIndex != 5){
		if(tcpListeningPCB[1] != NULL){
			tcp_connection_close(tcpListeningPCB[1]); // stop TCP Server to accept new connections
		}
		tcp_connection_close(tcpListeningPCB[0]); // stop TCP Server to accept new connections
		// FIXME close already present connections
	} else {
		udp_connection_close(tcpListeningPCB[0]);
	}
	st = DEFAULT;
	return "/cgipage.html";
}

static const char *Send_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]){
	if ((iIndex == 7 && st == TCP_SERVER) || (iIndex == 8 && st == TCP_CLIENT)){
		// create pbuf
		// check if actual pcb is NULL or not
		// if not chain new pbuf with provided message
		// else assign newly created pbuf
		// remove pbuf
		if(tcpListeningPCB[1] != NULL){ // if the connection is established
			u16_t len = strlen(pcValue[0]);
			struct pbuf* newbuf = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
			pbuf_take(newbuf, pcValue[0], len);
			
			struct tcp_connection_struct *es = (struct tcp_connection_struct *)tcpListeningPCB[1]->callback_arg;
			if(es->p == NULL){ // set new data to be sent
				es->p = newbuf;
				tcp_send(tcpListeningPCB[1], es);
			} else {
				pbuf_chain(es->p, newbuf); // chain newly added buffer is there is data to be sent
			}
		}
	} else if (iIndex == 9 && st == UDP_SERVER){
		// create pbuf
		// connect to remote ip to port
		// send pbuf
		// disconnect
		// remove pbuf
	} else { // else the none mode is initialized yet
		return "/404.html";
	}

	return "/cgipage.html";
}

void http_server_init (void) {
	httpd_init();

	tcpCGIHandlersArray[0] = TCP_SERVER_START_FORM;
	tcpCGIHandlersArray[1] = TCP_SERVER_CLOSE_FORM;
	tcpCGIHandlersArray[2] = TCP_CLIENT_START_FORM;
	tcpCGIHandlersArray[3] = TCP_CLIENT_CLOSE_FORM;
	tcpCGIHandlersArray[4] = UDP_SERVER_START_FORM;
	tcpCGIHandlersArray[5] = UDP_SERVER_CLOSE_FORM;
	tcpCGIHandlersArray[7] = TCP_SERVER_SEND_FORM;
	tcpCGIHandlersArray[8] = TCP_CLIENT_SEND_FORM;
	tcpCGIHandlersArray[9] = UDP_SERVER_SEND_FORM;
	http_set_cgi_handlers (tcpCGIHandlersArray, numForms);
}
