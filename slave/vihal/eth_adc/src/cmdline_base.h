/*
 * cmdline_base.h
 *
 *  Created on: Oct 29, 2022
 *      Author: vitya
 */

#ifndef SRC_CMDLINE_BASE_H_
#define SRC_CMDLINE_BASE_H_

#include "hwuart.h"
#include "strparse.h"

#define CMDLINE_RXDMABUF_SIZE   128  // circular buffer
#define CMDLINE_MAX_RX_MSG_LEN  256  // maximal length of a parsed message

class TCmdLineBase
{
public:
  THwUart           uart;

  uint16_t          cmdlen = 0;
  bool              initialized = false;

  TStrParseObj      sp;

  const char *      prompt = "cmd> ";
  uint8_t           cmdbuf[CMDLINE_MAX_RX_MSG_LEN];  // parsed message buffer

  virtual           ~TCmdLineBase() { }

  void              Init();
  virtual bool      InitHw();

  bool              ExecCmd();
  virtual bool      ParseCmd();

  void              WritePrompt();

  void              Run();  // processes Rx
  void              ProcessByte(uint8_t b);

public:
#if MCU_NO_DMA  // VRV100

#else
  uint16_t          rxdmapos = 0;
  THwDmaChannel     dma_rx;
  THwDmaTransfer    dmaxfer_rx;
  uint8_t           rxdmabuf[CMDLINE_RXDMABUF_SIZE];   // circular buffer
#endif

};

#endif /* SRC_CMDLINE_BASE_H_ */
