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

  TIp4Addr          ip_address;    // IP
  TIp4Addr          net_mask;      // NETMASK
  TIp4Addr          gw_address;    // GW
  TIp4Addr          dns;           // DNS
  TIp4Addr          dns2;          // DNS2

  uint16_t          udoip_port;
  uint16_t          _reserved;

  char              wifi_ssid[64];     // WLSSID
  char              wifi_password[64]; // WLPW

  uint32_t          _tail_pad[256-8-32];  // pad up to 1k
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
