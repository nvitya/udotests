/* -----------------------------------------------------------------------------
 * This file is a part of the UDOTESTS project: https://github.com/nvitya/udotests
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
 *  file:     cmdline_base.cpp
 *  brief:    base class for interactive command line (Arduino version)
 *  created:  2023-05-31
 *  authors:  nvitya
*/

#include <cmdline_base.h>

#define TRACE(...)  Serial.printf( __VA_ARGS__ )

void TCmdLineBase::Init()
{
  cmdlen = 0;

  initialized = true;
}

bool TCmdLineBase::InitHw()
{
  return false;
}

bool TCmdLineBase::ParseCmd()
{
  return false;
}

bool TCmdLineBase::ExecCmd()
{

  return ParseCmd();
}

void TCmdLineBase::Run()
{
  if (!initialized)
  {
    return;
  }

  while (Serial.available() > 0)
  {
    uint8_t b = Serial.read();
    //TRACE("%02X\r\n", b);
    if (13 == b)
    {
      cmdbuf[cmdlen] = 0; // zero terminate the command line
      TRACE("\r\n");

      if (cmdlen > 0)
      {
#if 0
        TRACE("cmd = ");
        for (unsigned n = 0; n < cmdlen; ++n)
        {
          TRACE(" %02X", cmdbuf[n]);
        }
        TRACE("\r\n");
#endif

        sp.Init((char *)&cmdbuf[0], cmdlen);

        // execute the command
        if (!ExecCmd())
        {
          TRACE("Unknown command \"%s\"\r\n", &cmdbuf[0]);
        }
      }

      WritePrompt();

      cmdlen = 0;
    }
    else if ((8 == b) || (127 == b)) // backspace
    {
      if (cmdlen > 0)
      {
        TRACE("\b\x1b[K"); // cursor back by one, "ESC [ K" = clear the line from the cursor
        --cmdlen;
      }
    }
    else if ((b >= 32) && (b < 0x7F)) // add to the rx message length
    {
      if (cmdlen < sizeof(cmdbuf))
      {
        cmdbuf[cmdlen] = b;
        ++cmdlen;
      }
      TRACE("%c", b);  // echo back
    }
    else if (27 == b) // escape char !
    {

    }
  }
}

void TCmdLineBase::WritePrompt()
{
  //uart.printf("%s", prompt);
  TRACE("%s", prompt);
}
