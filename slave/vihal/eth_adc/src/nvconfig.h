/*
 * nvconfig.h
 *
 *  Created on: Oct 29, 2022
 *      Author: vitya
 */

#ifndef SRC_NVCONFIG_H_
#define SRC_NVCONFIG_H_

#include "platform.h"
#include "net_ip4.h"

#define CFG_SIGNATURE   0xAA66CF55

typedef struct
{
  uint32_t          signature;
  uint32_t          cfg_csum;

  uint8_t           mac_address[8];  // only the first 6 bytes are used

  TIp4Addr          ip_address;
  TIp4Addr          net_mask;
  TIp4Addr          gw_address;

  uint32_t          _tail_pad[256-7];  // pad up to 1k
//
} TCfgStb;

class TNvCfgStorage
{
public:
  TCfgStb    data;

  void       Init();

  void       Load();
  void       Save();
  void       UpdateFromConfig();
};

extern TNvCfgStorage  g_config;

#endif /* SRC_NVCONFIG_H_ */
