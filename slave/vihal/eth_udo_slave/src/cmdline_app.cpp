/*
 * cmdline_app.cpp
 *
 *  Created on: Oct 29, 2022
 *      Author: vitya
 */

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

  // there is a value

  if (PCharUCCompare(&ridp, idlen, "IP"))
  {
    if (!ParseIpAddr())
    {
      return true;
    }
    ip4_handler.ipaddress = ipaddr;
    ShowNetInfo(idptr, idlen);
    return true;
  }

  if (PCharUCCompare(&ridp, idlen, "NETMASK"))
  {
    if (!ParseIpAddr())
    {
      return true;
    }
    ip4_handler.netmask = ipaddr;
    ShowNetInfo(idptr, idlen);
    return true;
  }

  if (PCharUCCompare(&ridp, idlen, "GW"))
  {
    if (!ParseIpAddr())
    {
      return true;
    }
    ip4_handler.gwaddress = ipaddr;
    ShowNetInfo(idptr, idlen);
    return true;
  }

  if (PCharUCCompare(&ridp, idlen, "MAC"))
  {
    if (!ParseMacAddr())
    {
      return true;
    }
    eth.SetMacAddress(&macaddr[0]);
    ShowNetInfo(idptr, idlen);
    return true;
  }

  TRACE("Unknown NET section.\r\n");

  return true;
}

void TCmdLineApp::ShowNetAdapterInfo()
{
  TRACE("Network Adapter Info:\r\n");
  //TRACE("  Tx Packets  : %6u\r\n",        net_adapter.max_tx_packets);
  //TRACE("  Rx Packets  : %6u\r\n",        net_adapter.max_rx_packets);
  TRACE("  Total Memory:   %u bytes\r\n",  net_adapter.NetMemSize());
  TRACE("  Free Memory:    %u bytes\r\n",  net_adapter.NetMemFree());
}

void TCmdLineApp::ShowNetInfo(char * idptr, unsigned idlen)
{
  TIp4Addr ip;

  if (!idptr || PCharUCCompare(&idptr, idlen, "MAC"))
  {
    uint8_t * pmac = &eth.mac_address[0];
    TRACE("  MAC      = %02X-%02X-%02X-%02X-%02X-%02X\r\n",
            pmac[0], pmac[1], pmac[2], pmac[3], pmac[4], pmac[5]);
  }

  if (!idptr || PCharUCCompare(&idptr, idlen, "IP"))
  {
    ip = ip4_handler.ipaddress;
    TRACE("  IP       = %d.%d.%d.%d\r\n", ip.u8[0], ip.u8[1], ip.u8[2], ip.u8[3]);
  }
  if (!idptr || PCharUCCompare(&idptr, idlen, "NETMASK"))
  {
    ip = ip4_handler.netmask;
    TRACE("  NETMASK  = %d.%d.%d.%d\r\n", ip.u8[0], ip.u8[1], ip.u8[2], ip.u8[3]);
  }
  if (!idptr || PCharUCCompare(&idptr, idlen, "GW"))
  {
    ip = ip4_handler.gwaddress;
    TRACE("  GW       = %d.%d.%d.%d\r\n", ip.u8[0], ip.u8[1], ip.u8[2], ip.u8[3]);
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
