/*
 * usb_application.h
 *
 *  Created on: Jun 4, 2023
 *      Author: vitya
 */

#ifndef SRC_USB_APPLICATION_H_
#define SRC_USB_APPLICATION_H_

#include "usbdevice.h"
#include "usbif_cdc.h"
#include "udo_usb_comm.h"

class TUsbApplication : public TUsbDevice
{
private:
  typedef TUsbDevice super;

public: // mandatory functions

  virtual bool    InitDevice();

};

extern TUsbFuncUdo      usb_func_udo;
extern TUsbApplication  usb_app;

bool usb_app_init();
void usb_app_run();

#endif /* SRC_USB_APPLICATION_H_ */
