/*
 * adc_sampler_f7.h
 *
 *  Created on: Aug 23, 2024
 *      Author: vitya
 */

#ifndef ADC_SAMPLER_F7_H_
#define ADC_SAMPLER_F7_H_

#include "platform.h"
#include "hwadc.h"

#define ASMP_DMABUF_SAMPLES  2048 // must be power of two !
#define ASMP_DMABUF_IDXMASK  (ASMP_DMABUF_SAMPLES - 1)

class TAdcChannelSampler
{
public:
  HWADC_REGS *          regs = nullptr;
  HWADC_COMMON_REGS *   commonregs = nullptr;
  bool                  initialized = false;
  uint8_t               dmastream = 0xFF;
  uint8_t               devnum = 0;
  uint8_t               chcnt = 0;
  uint32_t              channel_map = 0;
  uint32_t              adc_clock = 0;
  uint32_t              conv_adc_clocks = 0;  // ADC clocks required to one conversion
  uint32_t              act_conv_rate = 0;    // actual conversion rate in Hz

  THwDmaTransfer        dmaxfer;
  THwDmaChannel         dmach;

  uint16_t              dmabuf[ASMP_DMABUF_SAMPLES];

  virtual               ~TAdcChannelSampler() {}

  bool                  Init(int adevnum, uint32_t achannel_map);

public: // STM32 helper functions
  void                  SetupChannels(uint32_t achsel);
  void                  StartContConv();

  uint32_t              DmaPos() { return dmaxfer.count - dmach.Remaining(); }

};

class TAdcSampler
{
public:

  TIM_TypeDef *         timer = nullptr;

  uint32_t              prev_dma_pos = 0;
  uint32_t              sampling_period_ns = 1000;
  uint16_t              proc_samples = 0;
  uint16_t              dma_remaining = 0;

  TAdcChannelSampler    channel[ADC_SAMPLER_COUNT];

  virtual               ~TAdcSampler() {}

  void                  Init();
  void                  RunIrqTask();
};

extern TAdcSampler  g_adc_sampler;

#endif /* ADC_SAMPLER_F7_H_ */
