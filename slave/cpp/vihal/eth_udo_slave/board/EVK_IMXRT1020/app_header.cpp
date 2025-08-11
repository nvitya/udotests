// app_header.cpp: generic VGBOOT application header

#include "platform.h"
#include "app_header.h"

extern unsigned __app_image_end;

extern "C" void cold_entry();

extern const TAppHeader application_header;  // this is required, otherwise it will be removed by the linker

__attribute__((section(".application_header"),used))
const TAppHeader application_header =
{
  .signature = APP_HEADER_SIGNATURE,
  .length = unsigned(&__app_image_end) - unsigned(&application_header) - sizeof(TAppHeader),
  .addr_load = unsigned(&application_header),
  .addr_entry = (unsigned)cold_entry,

  .customdata = 0,
  .compid = 0,
  .csum_body = 0,  // will be calculated later
  .csum_head = 0   // will be calculated later
};


