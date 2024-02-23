// app_header.cpp: generic VGBOOT application header

#include "platform.h"
#include "app_header.h"

extern unsigned __app_image_end;

extern "C" void _cold_entry(void);

extern const bootblock_header_t application_header;  // this is required, otherwise it will be removed by the linker

__attribute__((section(".application_header"),used))
const bootblock_header_t application_header =
{
  .signature = BOOTBLOCK_SIGNATURE,
  .length = unsigned(&__app_image_end) - unsigned(&application_header) - sizeof(bootblock_header_t),
  .addr_load = unsigned(&application_header),
  .addr_entry = (unsigned)_cold_entry,
  ._reserved10 = 0,
  .compid = 0xEEEEEEEE,  // will be updated later
  .csum_body = 0,
  .csum_head = 0
};

