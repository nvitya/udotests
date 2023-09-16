/*
 * spi_self_flash_imxrt.cpp
 *
 *  Created on: Sep 3, 2023
 *      Author: vitya
 */

#include "stdlib.h"
#include "platform.h"
#include "board_pins.h"
#include "spiflash_updater.h"
#include "spi_self_flashing.h"
#include "app_header.h"
#include "vgboot_utils.h"
#include "traces.h"

extern unsigned __image_end;

#define SELFFLASH_BUFSIZE  4096

// do self flashing using the flash writer
bool spi_self_flashing(TSpiFlash * spiflash)
{
  uint8_t * tempbuf = (uint8_t *)&g_scope_buffer[0];

  uint32_t  flash_addr = NVSADDR_APPLICATION;
  uint8_t * start_addr = (uint8_t *)APP_CODE_LOAD_ADDR;

  // calculate checksums first
  TAppHeader *       pheader = (TAppHeader *)APP_CODE_LOAD_ADDR;
  uint32_t           startaddr2 = uint32_t(pheader + 1);
  uint32_t           cs = vgboot_checksum((void *)startaddr2, pheader->length);

  pheader->csum_body = cs;
  pheader->csum_head = vgboot_checksum(pheader, sizeof(TAppHeader));

  unsigned copylength = unsigned(&__image_end) - unsigned(start_addr);
  copylength = ((copylength + 255) & 0xFFF00);

  __DSB();

  // Using the flash writer to first compare the flash contents:
  TSpiFlashUpdater  flashupdater(spiflash, tempbuf, SELFFLASH_BUFSIZE);

  TRACE("Self-Flashing:\r\n");
  TRACE("  mem = %08X -> flash = %08X, len = %u ...\r\n", unsigned(start_addr), flash_addr, copylength);

  unsigned  t0, t1;
  t0 = CLOCKCNT;

  if (!flashupdater.UpdateFlash(flash_addr, start_addr, copylength))
  {
    TRACE("  ERROR!\r\n");
    return false;
  }

  t1 = CLOCKCNT;
  unsigned clocksperus = SystemCoreClock / 1000000;

  unsigned ssize_k = flashupdater.sectorsize >> 10;

  TRACE("  %u * %uk updated, %u * %uk matched, took %u us\r\n",
      flashupdater.writecnt, ssize_k, flashupdater.samecnt, ssize_k, (t1 - t0) / clocksperus);

  return true;
}
