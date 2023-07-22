
using namespace std;

#include "stdio.h"
#include "stdlib.h"
#include <string>
#include "udo_comm.h"
#include "commh_udoip.h"

uint8_t databuf[65536];

void flushstdout()
{
	fflush(stdout);
}

int main(int argc, char * const * argv)
{
  printf("UDO-IP Test - v1.0\n");

  //udoip_commh.ipaddrstr = "127.0.0.1:1221";
  udoip_commh.ipaddrstr = "192.168.0.74:1221";
  if (argc > 1)
  {
  	udoip_commh.ipaddrstr = argv[1];
  }
  udocomm.SetHandler(&udoip_commh);

  udocomm.Open(); // throws an exception if it fails

	printf("UDO-IP connection \"%s\" opened.\n", udoip_commh.ipaddrstr.c_str());

	int       i;
	uint32_t  u32;
	int       rlen;

  printf("Reading test object:\n");
  u32 = udocomm.ReadU32(0, 0);
  printf("  = %08X\n", u32);

  printf("Reading test object again:\n");
  u32 = udocomm.ReadU32(0, 0);
  printf("  = %08X\n", u32);

  printf("Reading max chunk length:\n");
  u32 = udocomm.ReadU32(1, 0);
  printf("  = %u\n", u32);

  printf("Testing BLOB read:\n");
  rlen = udocomm.ReadBlob(2, 0, &databuf[0], sizeof(databuf));
  printf("rlen = %i\n", rlen);

  uint32_t * pdata = (uint32_t *)&databuf[0];
  for (i = 0; i < 512; ++i)
  {
    if ((i > 0) && ((i % 8) == 0))  printf("\n");
    printf(" %5d", pdata[i]);
  };
  printf("\n");

	// set the led blink pattern
  printf("Writing 0x1000...\n");
  try
  {
  	udocomm.WriteU32(0x1500, 0, 0x00111111);
  	printf("  OK\n");
  }
  catch (...)
  {
  	printf("  FAILED\n");
  }

	udocomm.Close();

	printf("Test finished.\n");
	//flushstdout();

  return 0;
}

