/*
 * hacking.h
 *
 *  Created on: Mar 15, 2013
 *      Author: Alejandro Alcalde <algui91@gmail.com>
 *
 *  <one line to give the program's name and a brief idea of what it does.>
 *  Copyright (C) 2013 Alejandro Alcalde
 *
 *  gnm is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gnm is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <netinet/tcp.h>
#include <asm/types.h>

typedef struct
{
	__u8 family;
	__u8 bytelen;
	__s16 bitlen;
	__u32 flags;
	__u32 data[8];
} inet_prefix;

struct filter
{
	int states;
	int families;
};

static const char *sstate_name[] = {
	"UNKNOWN",
	[TCP_ESTABLISHED] = "ESTAB",
	[TCP_SYN_SENT] = "SYN-SENT",
	[TCP_SYN_RECV] = "SYN-RECV",
	[TCP_FIN_WAIT1] = "FIN-WAIT-1",
	[TCP_FIN_WAIT2] = "FIN-WAIT-2",
	[TCP_TIME_WAIT] = "TIME-WAIT",
	[TCP_CLOSE] = "UNCONN",
	[TCP_CLOSE_WAIT] = "CLOSE-WAIT",
	[TCP_LAST_ACK] = "LAST-ACK",
	[TCP_LISTEN] = 	"LISTEN",
	[TCP_CLOSING] = "CLOSING",
};

static const char *sstate_namel[] = {
	"UNKNOWN",
	[TCP_ESTABLISHED] = "established",
	[TCP_SYN_SENT] = "syn-sent",
	[TCP_SYN_RECV] = "syn-recv",
	[TCP_FIN_WAIT1] = "fin-wait-1",
	[TCP_FIN_WAIT2] = "fin-wait-2",
	[TCP_TIME_WAIT] = "time-wait",
	[TCP_CLOSE] = "unconnected",
	[TCP_CLOSE_WAIT] = "close-wait",
	[TCP_LAST_ACK] = "last-ack",
	[TCP_LISTEN] = 	"listening",
	[TCP_CLOSING] = "closing",
};

struct tcpstat
{
	inet_prefix	local;
	inet_prefix	remote;
	int		lport;
	int		rport;
	int		state;
	int		rq, wq;
	int		timer;
	int		timeout;
	int		retrs;
	unsigned	ino;
	int		probes;
	unsigned	uid;
	int		refcnt;
	unsigned long long sk;
	int		rto, ato, qack, cwnd, ssthresh;
};

/**
 * A function to display an error message and then exit
 */
void
fatal(char* message)
{
	char error_message[100];

	strcpy(error_message, "[!!] Fatal Error ");
	strncat(error_message, message, 83);
	perror(error_message);

	exit(-1);
}

/**
 * An error-checked malloc() wrapper function
 */
void*
ec_malloc(unsigned int size)
{
	void* ptr;

	ptr = malloc(size);
	if (ptr == NULL)
		fatal("in ec_malloc() on memory allocation");

	return ptr;
}
#endif /* UTILS_H_ */
