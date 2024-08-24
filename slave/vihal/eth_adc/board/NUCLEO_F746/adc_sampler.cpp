/*
 * adc_sampler_f7.cpp
 *
 *  Created on: Aug 23, 2024
 *      Author: vitya
 */

#include "board.h"
#include <adc_sampler.h>
#include "stm32_utils.h"
#include "device.h"
#include "simple_scope.h"

TAdcSampler  g_adc_sampler;

//uint32_t    adc_smp_

void TAdcSampler::Init()
{
  // enable sampling the timer

  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  timer = TIM2;

  timer->CR1 = 0;  // stop the timer for now

  // initialize the samplers
  #if ADC_SAMPLER_COUNT > 0
    channel[0].Init(1, (1 <<  3));  // A0 = PA3 = ADC1,2,3 /  3
  #endif
  #if ADC_SAMPLER_COUNT > 1
    channel[1].Init(2, (1 << 10));  // A1 = PC0 = ADC1,2,3 / 10
  #endif
  #if ADC_SAMPLER_COUNT > 2
    channel[2].Init(3, (1 << 13));  // A2 = PC3 = ADC1,2,3 / 13
  #endif

  // start the sampling timer

  uint32_t timer_base_speed = stm32_bus_speed(1);
  if (timer_base_speed < SystemCoreClock)
  {
    timer_base_speed = (timer_base_speed << 1);  // the timer clock speed is twice of the APB speed
  }

  sampling_period_ns = 1000000000 / ADC_SAMPLER_FREQ;

  timer->PSC = 0; // count every clock
  uint32_t periodclocks = timer_base_speed / ADC_SAMPLER_FREQ;

  timer->ARR = periodclocks - 1;
  timer->CR2 = (0
    | (0  <<  7)  // TI1S
    | (2  <<  4)  // MMS(3): 2 = TRGO active on Update
    | (0  <<  3)  // CDDS: Capture / compare DMA
  );

  timer->CR1 = 1;
  timer->EGR = 1; // reinit, start the timer
}

void TAdcSampler::RunIrqTask()
{
  unsigned n;

  //uint32_t  dmapos[ADC_SAMPLER_COUNT];

  dma_remaining = channel[0].dmach.Remaining();

  // The three ADCs should run parallel, but 1 difference is possible.
  // Calculate the common greatest new sample count
  uint32_t  newsamples = 9999;
  for (n = 0; n < ADC_SAMPLER_COUNT; ++n)
  {
    uint32_t  dmapos = channel[n].DmaPos();
    uint32_t  cnewsmp = ((dmapos - prev_dma_pos) & ASMP_DMABUF_IDXMASK);
    if (cnewsmp < newsamples)  newsamples = cnewsmp;
  }

  proc_samples = newsamples;

  while (newsamples > 0)
  {
    for (n = 0; n < ADC_SAMPLER_COUNT; ++n)
    {
      g_device.adc_value[n] = (channel[n].dmabuf[prev_dma_pos] >> 1);
    }

    g_scope.RunIrqTask();  // process the scope samples

    --newsamples;
    prev_dma_pos = ((prev_dma_pos + 1) & ASMP_DMABUF_IDXMASK);
  }
}

bool TAdcChannelSampler::Init(int adevnum, uint32_t achannel_map)
{
  uint32_t tmp;

  initialized = false;

  devnum = adevnum;
  initialized = false;
  channel_map = achannel_map;

  regs = nullptr;
  if      (1 == devnum)
  {
    regs = ADC1;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    if (dmastream == 4)
    {
      dmach.Init(2, 4, 0); // alternative
    }
    else
    {
      dmach.Init(2, 0, 0); // default
    }
  }
#ifdef ADC2
  else if (2 == devnum)
  {
    regs = ADC2;
    RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
    if (dmastream == 3)
    {
      dmach.Init(2, 3, 1); // alternative
    }
    else
    {
      dmach.Init(2, 2, 1); // default
    }
  }
#endif
#ifdef ADC3
  else if (3 == devnum)
  {
    regs = ADC3;
    RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
    if (dmastream == 0)
    {
      dmach.Init(2, 0, 2); // alternative
    }
    else
    {
      dmach.Init(2, 1, 2); // default
    }
  }
#endif
  else
  {
    return false;
  }

  dmach.Prepare(false, (void *)&regs->DR, 0);

  // set ADC clock
  uint32_t baseclock = SystemCoreClock / 2;  // usually 84, 90 or 108
  uint32_t adcmaxclock = 36000000;
  uint32_t adcdiv = 2;
  while ((adcdiv < 8) && (baseclock / adcdiv > adcmaxclock))
  {
    adcdiv += 2;
  }

  adc_clock = baseclock / adcdiv;

  // ADC Common register
  ADC->CCR = 0
    | (1 << 23)  // TSVREFE: 1 = temp sensor and internal reference enable
    | (0 << 22)  // VBATEN: VBAT ch enable
    | (((adcdiv >> 1) - 1) << 16)  // ADCPRE(2): ADC prescaler
    | (0 << 14)  // DMA: Direct memory access mode for multi ADC mode
    | (0 << 13)  // DDS: DMA disable selection (for multi-ADC mode)
    | (0 <<  8)  // DELAY(4): Delay between 2 sampling phases (for dual / triple mode)
    | (0 <<  0)  // MULTI(5): Multi ADC mode selection, 0 = independent mode
  ;


  // stop adc
  regs->CR2 &= ~(ADC_CR2_ADON);

  regs->CR1 = 0
    | (0 << 26)  // OVRIE: Overrun interrupt enable
    | (0 << 24)  // RES(2): Resolution, 0 = 12 bit, 1 = 10 bit, 2 = 8 bit, 3 = 6 bit
    | (0 << 23)  // AWDEN: analog watchdog enable
    | (0 << 22)  // JAWDEN: analog watchdog enable for injected ch.
    | (0 << 13)  // DISCNUM(3): discontinuous mode channel count
    | (0 << 12)  // JDISCEN: Discontinuous mode on injected channels
    | (0 << 11)  // DISCEN: Discontinuous mode on regular channels
    | (0 << 10)  // JAUTO: Automatic Injected Group conversion
    | (0 <<  9)  // AWDSGL: Enable the watchdog on a single channel in scan mode
    | (1 <<  8)  // SCAN: Scan mode
    | (0 <<  7)  // JEOCIE: Interrupt enable for injected channels
    | (0 <<  6)  // AWDIE: Analog watchdog interrupt enable
    | (0 <<  5)  // EOCIE: Interrupt enable for EOC
    | (0 <<  0)  // AWDCH(5): Analog watchdog channel select
  ;

  regs->CR2 = 0
    | (0 << 30)  // SWSTART: Start conversion of regular channels
    | (1 << 28)  // EXTEN(2): External trigger enable for regular channels, 0 = disabled, 1 = rising edge, 2 = falling, 3 both
    | (11 << 24)  // EXTSEL(4): 11 = TIM2_TRGO, External event select for regular group (see reference)
    | (0 << 22)  // JSWSTART: Start conversion of injected channels
    | (0 << 20)  // JEXTEN(2): External trigger enable for injected channels, 0 = disabled, 1 = rising edge, 2 = falling, 3 both
    | (0 << 16)  // JEXTSEL(4): External event select for injected group (see reference)
    | (1 << 11)  // ALIGN: Data alignment, 0 = right, 1 = left
    | (0 << 10)  // EOCS: End of conversion selection, 0 = sequence end
    | (1 <<  9)  // DDS: DMA disable selection (for single ADC mode)
    | (1 <<  8)  // DMA: 1 = DMA enabled (only for ADC1, ADC2 does not have direct DMA connection)
    | (0 <<  1)  // CONT: Continuous conversion, 1 = continous conversion mode
    | (1 <<  0)  // ADON: A/D converter ON / OFF, 1 = enable ADC and start conversion
  ;

  // setup channel sampling time registers

  uint32_t stcode = 0; // sampling time code, index for the following array
  uint32_t sampling_clocks[8] = {3, 15, 28, 56, 84, 112, 144, 480};

  int i;
  tmp = 0;
  for (i = 0; i < 9; ++i)
  {
    tmp |= (stcode << (i * 3));
  }
  regs->SMPR1 = tmp;
  tmp = 0;
  for (i = 0; i < 10; ++i)
  {
    tmp |= (stcode << (i * 3));
  }
  regs->SMPR2 = tmp;

  // calculate the actual conversion rate

  // total conversion cycles:  15 ADC clocks + sampling time (= 3 ADC clocks)
  conv_adc_clocks = sampling_clocks[stcode] + 15;
  act_conv_rate = adc_clock / conv_adc_clocks;

  SetupChannels(channel_map);

  // prepare the DMA transfer

  dmaxfer.bytewidth = 2;
  dmaxfer.count = ASMP_DMABUF_SAMPLES;
  dmaxfer.dstaddr = &dmabuf[0];
  dmaxfer.flags = DMATR_CIRCULAR;
  dmach.StartTransfer(&dmaxfer);

  StartContConv();  // waits for timer trigger !

  initialized = true;
  return true;
}

#if defined(MCUSF_G4) || defined(MCUSF_H7) || defined(MCUSF_F3)
  #define HWADC_SQREG_SHIFT  6
  #define STM32_FASTADC
#else
  #define HWADC_SQREG_SHIFT  5
#endif

void TAdcChannelSampler::SetupChannels(uint32_t achsel)
{
  channel_map = achsel;

  uint32_t ch;
  uint32_t sqr[4] = {0, 0, 0, 0};
  uint32_t * psqr = &sqr[0];
  uint32_t bitshift = 0;
#ifdef STM32_FASTADC
  bitshift = HWADC_SQREG_SHIFT;  // the sequence length is on the bits 0..3 here
#endif

  chcnt = 0;
  for (ch = 0; ch < HWADC_MAX_CHANNELS; ++ch)
  {
    if (channel_map & (1 << ch))
    {
      // add this channel
      *psqr |= (ch << bitshift);
      ++chcnt;

      bitshift += HWADC_SQREG_SHIFT;
      if (bitshift > 25)
      {
        // go to the next register
        bitshift = 0;
        ++psqr;
      }
    }
  }

#if defined(MCUSF_F0) || defined(MCUSF_G0)
  regs->CHSELR = channel_map;

#elif defined(STM32_FASTADC)
  regs->SQR1 = sqr[0] | (dmadatacnt-1);  // this contains the sequence length too
  regs->SQR2 = sqr[1];
  regs->SQR3 = sqr[2];
  regs->SQR4 = sqr[3];

  #if defined(MCUSF_H7_V2)
    regs->PCSEL_RES0 = channel_map;
  #elif defined(ADC_PCSEL_PCSEL_0)
    regs->PCSEL = channel_map;
  #endif

#else
  regs->SQR3 = sqr[0];
  regs->SQR2 = sqr[1];
  regs->SQR1 = (sqr[2] & 0x000FFFFF) | ((chcnt-1) << 20);  // this contains the sequence length too
#endif
}

void TAdcChannelSampler::StartContConv()
{
  // and start the conversion
  //regs->CR2 |= ADC_CR2_CONT;  // enable the continuous mode
  //regs->CR2 |= (ADC_CR2_SWSTART | ADC_CR2_ADON);
  regs->CR2 |= ADC_CR2_ADON;
}


