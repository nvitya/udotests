/* -----------------------------------------------------------------------------
 * This file is a part of the UDO-Tests projects: https://github.com/nvitya/udotests
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
 * file:     cmdline_app.cpp
 * brief:    Application specific command line processing
 * created:  2023-05-21
 * authors:  nvitya
*/

#include "string.h"
#include <cmdline_app.h>
#include "nvconfig.h"
#include "traces.h"
#include "board_pins.h"

TCmdLineApp g_cmdline;

void cpu_soft_reset()
{
  __disable_irq();

  uint32_t tmp = SCB->AIRCR;
  tmp &= 0x0000FF00;
  tmp |= 0x05FA0004; // write key + reset (SYSRESETREQ)
  SCB->AIRCR = tmp;

  while (1)
  {
    // wait for the reset...
  }
}

bool TCmdLineApp::ParseCmd() // sp is already prepared
{
  sp.SkipSpaces();
  if (!sp.ReadAlphaNum())
  {
    TRACE("The command must begin with an identifier!\r\n");
    return true;
  }

  if (sp.UCComparePrev("H") or sp.UCComparePrev("HELP"))
  {
    ShowHelp();
    return true;
  }

  if (sp.UCComparePrev("SAVE"))
  {
    g_config.Save();
    return true;
  }

  if (sp.UCComparePrev("NET"))
  {
    ParseCmdNet();
    return true;
  }

  if (sp.UCComparePrev("REBOOT") || sp.UCComparePrev("RESTART"))
  {
    cpu_soft_reset();
    return true;
  }


  return false;
}

void TCmdLineApp::ShowHelp()
{
  TRACE("Available commands:\r\n");
  TRACE("  NET [var] [value]  (displays, sets network parameters)\r\n");
  TRACE("  SAVE  (saves configuration permanently)\r\n");
  TRACE("  REBOOT  (restarts the device)\r\n");
}

bool TCmdLineApp::ParseCmdNet()
{
  sp.SkipSpaces();
  if (!sp.ReadAlphaNum())
  {
    ShowNetInfo(nullptr, 0);
    return true;
  }

  idptr = sp.prevptr;
  idlen = sp.prevlen;
  char * ridp = idptr;

  sp.SkipSpaces();

  if (sp.readptr >= sp.bufend)
  {
    // no value, just print the info
    ShowNetInfo(idptr, idlen);
    return true;
  }

  if (sp.CheckSymbol("=")) // it is optional
  {
    sp.SkipSpaces();
  }

  // there is a value

  if (PCharUCCompare(&ridp, idlen, "IP"))
  {
    if (!ParseIpAddr())
    {
      return true;
    }
    g_config.data.ip_address = ipaddr;
    ShowNetInfo(idptr, idlen);
    return true;
  }

  if (PCharUCCompare(&ridp, idlen, "PORT") || PCharUCCompare(&ridp, idlen, "UDPCAN_PORT"))
  {
    int ival;
    if (!sp.ReadDecimalNumbers())
    {
      TRACE("Invalid port value.\r\n");
      return true;
    }
    g_config.data.udoip_port = sp.PrevToInt();
    ShowNetInfo(idptr, idlen);
    return true;
  }

  if (PCharUCCompare(&ridp, idlen, "NETMASK"))
  {
    if (!ParseIpAddr())
    {
      return true;
    }
    g_config.data.net_mask = ipaddr;
    ShowNetInfo(idptr, idlen);
    return true;
  }

  if (PCharUCCompare(&ridp, idlen, "GW"))
  {
    if (!ParseIpAddr())
    {
      return true;
    }
    g_config.data.gw_address = ipaddr;
    ShowNetInfo(idptr, idlen);
    return true;
  }

  if (PCharUCCompare(&ridp, idlen, "DNS"))
  {
    if (!ParseIpAddr())
    {
      return true;
    }

    g_config.data.dns = ipaddr;
    ShowNetInfo(idptr, idlen);
    return true;
  }

  if (PCharUCCompare(&ridp, idlen, "WLSSID"))
  {
    if (!ParseString(g_config.data.wifi_ssid, sizeof(g_config.data.wifi_ssid)))
    {
      return true;
    }

    ShowNetInfo(idptr, idlen);
    return true;
  }

  if (PCharUCCompare(&ridp, idlen, "WLPW"))
  {
    if (!ParseString(g_config.data.wifi_password, sizeof(g_config.data.wifi_password)))
    {
      return true;
    }

    ShowNetInfo(idptr, idlen);
    return true;
  }


  TRACE("Unknown NET section.\r\n");

  return true;
}

void TCmdLineApp::ShowNetAdapterInfo()
{
  TRACE("Network Adapter Info:\r\n");
  TRACE("  UART Speed:    %u bytes\r\n",  wifi.uart_speed);
}

void TCmdLineApp::ShowNetInfo(char * idptr, unsigned idlen)
{
  TCfgStb * pcfg = &g_config.data;

  TIp4Addr ip;

  if (!idptr || PCharUCCompare(&idptr, idlen, "IP"))
  {
    ip = pcfg->ip_address;
    TRACE("  IP       = %d.%d.%d.%d\r\n", ip.u8[0], ip.u8[1], ip.u8[2], ip.u8[3]);
  }
  if (!idptr || PCharUCCompare(&idptr, idlen, "PORT") || PCharUCCompare(&idptr, idlen, "UDPCAN_PORT"))
  {
    TRACE("  PORT     = %d\r\n", pcfg->udoip_port);
  }
  if (!idptr || PCharUCCompare(&idptr, idlen, "NETMASK"))
  {
    ip = pcfg->net_mask;
    TRACE("  NETMASK  = %d.%d.%d.%d\r\n", ip.u8[0], ip.u8[1], ip.u8[2], ip.u8[3]);
  }
  if (!idptr || PCharUCCompare(&idptr, idlen, "GW"))
  {
    ip = pcfg->gw_address;
    TRACE("  GW       = %d.%d.%d.%d\r\n", ip.u8[0], ip.u8[1], ip.u8[2], ip.u8[3]);
  }
  if (!idptr || PCharUCCompare(&idptr, idlen, "DNS"))
  {
    ip = pcfg->dns;
    TRACE("  DNS      = %d.%d.%d.%d\r\n", ip.u8[0], ip.u8[1], ip.u8[2], ip.u8[3]);
  }

  if (!idptr || PCharUCCompare(&idptr, idlen, "WLSSID"))
  {
    TRACE("  WLSSID   = \"%s\"\r\n", pcfg->wifi_ssid);
  }

  if (!idptr || PCharUCCompare(&idptr, idlen, "WLPW"))
  {
    TRACE("  WLPW     = ***\r\n");
  }
}


bool TCmdLineApp::ParseIpAddr()
{
  for (int n = 0; n < 4; ++n)
  {
    sp.SkipSpaces();
    if (n > 0)
    {
      if (!sp.CheckSymbol("."))
      {
        TRACE("Invalid IP\r\n");
        return false;
      }
      sp.SkipSpaces();
    }

    if (!sp.ReadDecimalNumbers())
    {
      TRACE("Invalid IP\r\n");
      return false;
    }
    ipaddr.u8[n] = sp.PrevToInt();
  }

  return true;
}

bool TCmdLineApp::ParseMacAddr()
{
  for (int n = 0; n < 6; ++n)
  {
    sp.SkipSpaces();
    if (n > 0)
    {
      if (!sp.CheckSymbol("-") && !sp.CheckSymbol(":"))
      {
        TRACE("Invalid MAC\r\n");
        return false;
      }
      sp.SkipSpaces();
    }

    if (!sp.ReadHexNumbers())
    {
      TRACE("Invalid MAC\r\n");
      return false;
    }
    macaddr[n] = sp.PrevHexToInt();
  }

  return true;
}

bool TCmdLineApp::ParseString(char * adst, unsigned adstlen)
{
  sp.SkipSpaces();
  if (*sp.readptr == '\"')
  {
    if (!sp.ReadQuotedString())
    {
      TRACE("Invalid String.\r\n");
      return false;
    }
  }
  else
  {
    sp.ReadTo(" \r\n\0");
    if (sp.prevlen <= 0)
    {
      TRACE("Invalid String.\r\n");
      return false;
    }
  }

  vstr = sp.prevptr;
  vstrlen = sp.prevlen;

  if (adst)
  {
    if (adstlen <= sp.prevlen)  sp.prevlen = adstlen - 1;
    memcpy(adst, sp.prevptr, sp.prevlen);
    adst[sp.prevlen] = 0;
  }

  return true;
}
