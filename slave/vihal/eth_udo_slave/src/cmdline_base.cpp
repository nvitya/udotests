/*
 * cmdline_base.cpp
 *
 *  Created on: Oct 29, 2022
 *      Author: vitya
 */

#include <cmdline_base.h>
#include "traces.h"

void TCmdLineBase::Init()
{
  initialized = false;

  if (!InitHw())
  {
    return;
  }

#if !MCU_NO_DMA
  rxdmapos = 0;
  uart.DmaAssign(false, &dma_rx);

  // start the DMA receive with circular DMA buffer
  dmaxfer_rx.bytewidth = 1;
  dmaxfer_rx.count = sizeof(rxdmabuf);
  dmaxfer_rx.dstaddr = &rxdmabuf[0];
  dmaxfer_rx.flags = DMATR_CIRCULAR;
  uart.DmaStartRecv(&dmaxfer_rx);
#endif

  cmdlen = 0;

  initialized = true;
}

bool TCmdLineBase::InitHw()
{
  return false;
}

bool TCmdLineBase::ParseCmd()
{
  return false;
}

bool TCmdLineBase::ExecCmd()
{

  return ParseCmd();
}

void TCmdLineBase::Run()
{
  if (!initialized)
  {
    return;
  }

  // RX processing

#if MCU_NO_DMA
  char c;
  while (uart.TryRecvChar(&c))
  {
    ProcessByte(uint8_t(c));
  }
#else
  uint16_t newrxdmapos = sizeof(rxdmabuf) - dma_rx.Remaining();
  if (newrxdmapos >= sizeof(rxdmabuf))
  {
    // fix for Remaining() == 0 (sometimes the linked list operation is processed only later)
    newrxdmapos = 0;
  }

  while (rxdmapos != newrxdmapos)
  {
    ProcessByte(rxdmabuf[rxdmapos]);

    ++rxdmapos;
    if (rxdmapos >= sizeof(rxdmabuf))  rxdmapos = 0;
  }
#endif
}

void TCmdLineBase::ProcessByte(uint8_t b)
{
  if (13 == b)
  {
    cmdbuf[cmdlen] = 0; // zero terminate the command line
    TRACE("\r\n");

    if (cmdlen > 0)
    {
      sp.Init((char *)&cmdbuf[0], cmdlen);

      // execute the command
      if (!ExecCmd())
      {
        TRACE("Unknown command \"%s\"\r\n", &cmdbuf[0]);
      }
    }

    WritePrompt();

    cmdlen = 0;
  }
  else if (8 == b) // backspace
  {
    if (cmdlen > 0)
    {
      TRACE("\b\x1b[K"); // cursor back by one, "ESC [ K" = clear the line from the cursor
      --cmdlen;
    }
  }
  else if (b >= 32) // add to the rx message length
  {
    if (cmdlen < sizeof(cmdbuf))
    {
      cmdbuf[cmdlen] = b;
      ++cmdlen;
    }
    TRACE("%c", b);  // echo back
  }
  else if (27 == b) // escape char !
  {

  }
}

void TCmdLineBase::WritePrompt()
{
  //uart.printf("%s", prompt);
  TRACE("%s", prompt);
}

