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
 *  file:     udoslave.cpp
 *  brief:    Generic UDO Slave Implementations
 *  created:  2023-05-01
 *  authors:  nvitya
*/

#include "udoslave.h"

#include "udo.h"
#include "string.h"

bool udo_response_error(TUdoRequest * udorq, uint16_t aresult)
{
  udorq->result = aresult;
  return false;
}

bool udo_response_ok(TUdoRequest * udorq)
{
  udorq->result = 0;
  return true;
}

bool udo_ro_int(TUdoRequest * udorq, int avalue, unsigned len)
{
	if (udorq->iswrite)
	{
		udorq->result = UDOERR_READ_ONLY;
		return false;
	}

  *(int *)(udorq->dataptr) = avalue;
  udorq->datalen = len;
  udorq->result = 0;

  return true;
}

bool udo_ro_uint(TUdoRequest * udorq, unsigned avalue, unsigned len)
{
	if (udorq->iswrite)
	{
		udorq->result = UDOERR_READ_ONLY;
		return false;
	}

  *(unsigned *)(udorq->dataptr) = avalue;
  udorq->datalen = len;
  udorq->result = 0;

  return true;
}

bool udo_rw_data(TUdoRequest * udorq, void * dataptr, unsigned datalen)
{
	if (udorq->iswrite)
	{
		// write, handling segmented write too

		//TRACE("sdo_write(%04X, %i), offs=%u, len=%u\r\n", sdorq->index, sdorq->subindex, sdorq->offset, sdorq->datalen);

		if (datalen < udorq->offset + udorq->datalen)
		{
			udorq->result = UDOERR_WRITE_BOUNDS;
			return false;
		}

		uint8_t * cp = (uint8_t *)dataptr;
		cp += udorq->offset;

		uint32_t chunksize = datalen - udorq->offset;  // the maximal possible chunksize
		if (chunksize > udorq->datalen)  chunksize = udorq->datalen;

		memcpy(cp, udorq->dataptr, chunksize);
		udorq->result = 0;
		return true;
	}
	else // read, handling segmented read too
	{
		if (datalen <= udorq->offset)
		{
			udorq->datalen = 0; // empty read: no more data
			return true;
		}

		int remaining = datalen - udorq->offset;

		uint8_t * cp = (uint8_t *)dataptr;
		cp += udorq->offset;
		if (remaining > udorq->datalen)
		{
			remaining = udorq->datalen;
		}
		else
		{
			udorq->datalen = remaining;
		}
		memcpy(udorq->dataptr, cp, remaining);
		udorq->result = 0;
		return true;
	}
}

bool udo_ro_data(TUdoRequest * udorq, void * dataptr, unsigned datalen)
{
	if (!udorq->iswrite)
	{
		return udo_rw_data(udorq, dataptr, datalen);
	}

	udorq->result = UDOERR_READ_ONLY;
	return false;
}

bool udo_wo_data(TUdoRequest * udorq, void * dataptr, unsigned datalen)
{
	if (udorq->iswrite)
	{
		return udo_rw_data(udorq, dataptr, datalen);
	}

	udorq->result = UDOERR_WRITE_ONLY;
	return false;
}

bool udo_response_cstring(TUdoRequest * udorq, char * astr)
{
	unsigned len = strlen(astr);
	return udo_ro_data(udorq, astr, len);
}

int udorq_intvalue(TUdoRequest * udorq) // get signed integer value from the udo request
{
	if (udorq->datalen >= 4)
	{
		return *(int *)udorq->dataptr;
	}
	else if (udorq->datalen == 2)
	{
		return *(int16_t *)udorq->dataptr;
	}
	else if (udorq->datalen == 3)
	{
		return (*(int *)udorq->dataptr) & 0x00FFFFFF;
	}
	else
	{
		return *(int8_t *)udorq->dataptr;
	}
}

unsigned udorq_uintvalue(TUdoRequest * udorq) // get unsigned integer value from the udo request
{
	if (udorq->datalen >= 4)
	{
		return *(unsigned *)udorq->dataptr;
	}
	else if (udorq->datalen == 2)
	{
		return *(uint16_t *)udorq->dataptr;
	}
	else if (udorq->datalen == 3)
	{
		return (*(unsigned *)udorq->dataptr) & 0x00FFFFFF;
	}
	else
	{
		return *(uint8_t *)udorq->dataptr;
	}
}

//-----------------------------------------------------------------------------

bool TUdoSlave::UdoReadWrite(TUdoRequest * udorq)
{
	udorq->result = UDOERR_NOT_IMPLEMENTED;
	return false;
}


