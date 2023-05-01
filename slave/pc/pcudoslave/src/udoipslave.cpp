/* -----------------------------------------------------------------------------
 * This file is a part of the UDO project: https://github.com/nvitya/udo
 * Copyright (c) 2023 Viktor Nagy, nvitya
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 * --------------------------------------------------------------------------- */
/*
 *  file:     udoipslave.cpp
 *  brief:    UDO IP (UDP) Slave implementation
 *  created:  2023-05-01
 *  authors:  nvitya
*/

#include "string.h"
#include "udoipslave.h"
#include <fcntl.h>
#include "traces.h"

uint8_t udoip_rq_buffer[UDOIP_MAX_RQ_SIZE];
uint8_t udoip_ans_cache_buffer[UDOIP_ANSCACHE_NUM * UDOIP_MAX_RQ_SIZE]; // this might be relative big!

TUdoIpSlave::TUdoIpSlave()
{


}

TUdoIpSlave::~TUdoIpSlave()
{

}

void TUdoIpSlave::ProcessUdpRequest(TUdoIpRequest * ucrq)
{
	int r;

	TUdoIpRqHeader * prqh = (TUdoIpRqHeader *)ucrq->dataptr;

#if 0
	TRACETS("UDP RQ: srcip=%s, port=%i, datalen=%i\n",
				 inet_ntoa(*(struct in_addr *)&ucrq->srcip), ucrq->srcport, ucrq->datalen);

	TRACE("  sqid=%u, len_cmd=%u, address=%04X, offset=%u\n", prqh->rqid, prqh->len_cmd, prqh->address, prqh->offset);
#endif

	// prepare the response

	// Check against the answer cache
	TUdoIpSlaveCacheRec * pansc;

	pansc = FindAnsCache(ucrq, prqh);
	if (pansc)
	{
		//TRACE(" (sending cached answer)\n");
		// send back the cached answer, avoid double execution
		r = sendto(fdsocket, pansc->dataptr, pansc->datalen, 0, (struct sockaddr*)&client_addr, client_struct_length);
		if (r <= 0)
		{
			TRACE("UdoIpSlave: error sending back the answer!\n");
		}
		return;
	}

	// allocate an answer cache record
	pansc = AllocateAnsCache(ucrq, prqh);

	TUdoIpRqHeader * pansh = (TUdoIpRqHeader *)pansc->dataptr;
	*pansh = *prqh;  // initialize the answer header with the request header
	uint8_t * pansdata = (uint8_t *)(pansh + 1); // the data comes right after the header

	// execute the SDO

	memset(&mudorq, 0, sizeof(mudorq));
	mudorq.address  = prqh->address;
	mudorq.offset = prqh->offset;
	mudorq.datalen = (prqh->len_cmd & 0x7FF);
	mudorq.iswrite = ((prqh->len_cmd >> 15) & 1);
	mudorq.metalen = ((0x8420 >> ((prqh->len_cmd >> 13) & 3) * 4) & 0xF);
	mudorq.metadata = prqh->metadata;
	uint16_t ansdatalen = 0;

	if (mudorq.iswrite)
	{
		// write
		mudorq.dataptr = (uint8_t *)(prqh + 1); // the data comes after the header
		mudorq.datalen = ucrq->datalen - sizeof(TUdoIpRqHeader);  // override the datalen from the header

		UdoReadWrite(&mudorq);
	}
	else
	{
		// read
		mudorq.dataptr = pansdata;

		UdoReadWrite(&mudorq);

		if (0 == mudorq.result)
		{
			// prepare the answer, the data is already copied there
			ansdatalen = mudorq.datalen;
		}
	}

	if (mudorq.result)
	{
		pansh->len_cmd |= 0x7FF; // abort response
		*(int32_t *)pansdata = mudorq.result;
		ansdatalen = 2;
	}

	pansc->datalen = ansdatalen + sizeof(TUdoIpRqHeader);

	// send the response
	r = sendto(fdsocket, pansc->dataptr, pansc->datalen, 0, (struct sockaddr*)&client_addr, client_struct_length);
	if (r <= 0)
	{
		TRACE("UdoIpSlave: error sending back the answer!\n");
	}
}

bool TUdoIpSlave::UdoReadWrite(TUdoRequest * udorq)  // must be overwritten
{
	udorq->result = UDOERR_NOT_IMPLEMENTED;
	return false;
}

bool TUdoIpSlave::Init()
{
	initialized = false;

	rqbuf = &udoip_rq_buffer[0];
	rqbufsize = sizeof(udoip_rq_buffer);

	// initialize the ans_cache

	uint32_t offs = 0;
	for (unsigned n = 0; n < UDOIP_ANSCACHE_NUM; ++n)
	{
		memset(&ans_cache[n], 0, sizeof(ans_cache[n]));
		ans_cache[n].dataptr = &udoip_ans_cache_buffer[offs];
		ans_cache[n].idx = n;
		offs += UDOIP_MAX_RQ_SIZE;
		ans_cache_lru_idx[n] = n;
	}

	// Create UDP socket:
	fdsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fdsocket < 0)
	{
		TRACE("UdoIpSlave: Error creating socket\n");
		return false;
	}

	// Set port and IP:
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");  // bind to all interfaces

	// set the socket non-blocking
#ifdef WIN32
  ioctlsocket(fdsocket, FIONBIO, &noBlock);
#else
  int flags = fcntl(fdsocket, F_GETFL, 0);
  fcntl(fdsocket, F_SETFL, flags | O_NONBLOCK);
#endif

	// Bind to the set port and IP:
	if (bind(fdsocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		TRACE("UdoIpSlave: bind error (is another slave already running?)\n");
		return false;
	}

	// ok.

	initialized = true;
	return true;
}

void TUdoIpSlave::Run()
{
	// Receive client's message:
	int r = recvfrom(fdsocket, rqbuf, rqbufsize, 0, (struct sockaddr*)&client_addr, &client_struct_length);
	if (r > 0)
	{
		// something was received.

		mucrq.srcip = *(uint32_t *)&client_addr.sin_addr;
		mucrq.srcport = ntohs(client_addr.sin_port);
		mucrq.datalen = r;
		mucrq.dataptr = rqbuf;

		ProcessUdpRequest(&mucrq);

		//last_request_time = nstime();
	}
}

TUdoIpSlaveCacheRec * TUdoIpSlave::FindAnsCache(TUdoIpRequest * iprq, TUdoIpRqHeader * prqh)
{
	TUdoIpSlaveCacheRec * pac = &ans_cache[0];
	TUdoIpSlaveCacheRec * last_pac = pac + UDOIP_ANSCACHE_NUM;
	while (pac < last_pac)
	{
		if ( (pac->srcip == iprq->srcip) and (pac->srcport == iprq->srcport)
				 and (pac->rqh.rqid == prqh->rqid) and (pac->rqh.len_cmd == prqh->len_cmd)
				 and (pac->rqh.address == prqh->address) and (pac->rqh.offset == prqh->offset)
				 and (pac->datalen == iprq->datalen)
			 )
		{
			// found the previous request, the response was probably lost
			return pac;
		}
		++pac;
	}

	return nullptr;
}

TUdoIpSlaveCacheRec * TUdoIpSlave::AllocateAnsCache(TUdoIpRequest * iprq, TUdoIpRqHeader * prqh)
{
	// re-use the oldest entry
	uint8_t idx = ans_cache_lru_idx[0]; // the oldest entry
	memcpy(&ans_cache_lru_idx[0], &ans_cache_lru_idx[1], UDOIP_ANSCACHE_NUM - 1); // rotate the array
	ans_cache_lru_idx[UDOIP_ANSCACHE_NUM-1] = idx; // append to the end

	TUdoIpSlaveCacheRec * pac = &ans_cache[idx];

	pac->srcip   = iprq->srcip;
	pac->srcport = iprq->srcport;
	pac->rqh     = *prqh;

	return pac;
}
