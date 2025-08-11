/*
 * usb_application.cpp
 *
 *  Created on: Jun 4, 2023
 *      Author: vitya
 */

#include "udo_usb_comm.h"
#include "usb_application.h"
#include "traces.h"

TUsbFuncUdo      usb_func_udo;
TUsbApplication  usb_app;

bool TUsbApplication::InitDevice()
{
  devdesc.vendor_id  = 0xDEAD;
  devdesc.product_id = 0xBEEF;

  devdesc.device_class = 0x02; // Communications and CDC Control
  manufacturer_name = "github.com/nvitya/udo";
  device_name = "UDO-USB Device";
  device_serial_number = "1";

  AddFunction(&usb_func_udo);

  return true;
}

bool usb_app_init()
{
  TRACE("Initializing USB Device\r\n");

  if (!usb_app.Init()) // calls InitDevice first which sets up the device
  {
    TRACE("Error initializing USB device!\r\n");
    return false;
  }

  return true;
}

void usb_app_run()
{
  usb_app.HandleIrq();
  usb_app.Run();
}
