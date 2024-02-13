#include "http_ssi.h"
#include "tcpRAW.h"
#include "lwip/tcp.h"
#include "lwip/apps/httpd.h"
#include "stm32f4xx_hal.h"

static const char *Start_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
static const char *Close_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

const tCGI START_FORM = {"/tcpServerStart.cgi", Start_Handler};
const tCGI CLOSE_FORM = {"/tcpServerClose.cgi", Close_Handler};
tCGI tcpCGIHandlersArray[2];

// each listening port contain 1 listening pcb and 1 for connected user
struct tcp_pcb* tcpListeningPCB[2];

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
	tcpListeningPCB[0] = tcp_server_init(&localIP, localPort); // start TCP Server and save listening PCB
	return "/cgipage.html";
}

static const char* Close_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
	tcp_server_connection_close(tcpListeningPCB[1]); // stop TCP Server to accept new connections
	tcp_server_connection_close(tcpListeningPCB[0]); // stop TCP Server to accept new connections
	// FIXME close already present connections
	return "/cgipage.html";
}

void http_server_init (void)
{
	httpd_init();

	tcpCGIHandlersArray[0] = START_FORM;
	tcpCGIHandlersArray[1] = CLOSE_FORM;
	http_set_cgi_handlers (tcpCGIHandlersArray, 2);
}
