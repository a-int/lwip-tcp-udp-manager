/*
  ***************************************************************************************************************
  ***************************************************************************************************************
  ***************************************************************************************************************

  File:		  tcpServerRAW.h
  Author:     ControllersTech.com
  Updated:    26-Jul-2021

  ***************************************************************************************************************
  Copyright (C) 2017 ControllersTech.com

  This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
  of the GNU General Public License version 3 as published by the Free Software Foundation.
  This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
  or indirectly by this software, read more about this on the GNU General Public License.

  ***************************************************************************************************************
*/
#include "lwip/tcp.h"

#ifndef INC_TCPSERVERRAW_H_
#define INC_TCPSERVERRAW_H_

struct tcp_pcb*  tcp_server_init(ip4_addr_t* ip, u16_t port);
void tcp_client_init(void);

void tcp_server_connection_close(struct tcp_pcb *tpcb);


#endif /* INC_TCPSERVERRAW_H_ */
