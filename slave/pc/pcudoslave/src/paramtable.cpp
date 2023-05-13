/*
 *  file:     paramtable.cpp
 *  brief:    UDO Slave Example Parameter Table
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#include "string.h"
#include "paramtable.h"
#include "device.h"

// at some improper definitions (like missing TClass base) the tables were moved to .data (initialized RW data)
// and thus took twice so much space.  So we force these tables to .rodata with the following macro:
//#define PARAMTABLE_DEF   __attribute__((section(".rodata"))) const TParameterDef
#define PARAMTABLE_DEF   const TParameterDef
// if you get "session type conflict", then maybe some classes with callbacks are not based on the TClass

#define PAR_TABLE(astart, atab)    {astart, astart + (sizeof(atab) / sizeof(TParameterDef)) - 1, &atab[0], nullptr, nullptr}

/*****************************************************************************************************************
                                             RANGE TABLES
******************************************************************************************************************/
// range tables must come before the main

PARAMTABLE_DEF paramtable_2000[] =  // 0x2000
{
/*2000*/{ PAR_INT32_RO,  (void *)&g_device.irq_period_ns,      nullptr, nullptr },
/*2001*/{ PAR_UINT16_RO, (void *)&g_device.irq_cycle_counter,  nullptr, nullptr },
/*2002*/{ PAR_INT32_RO,  (void *)&g_device.func_i32_1,         nullptr, nullptr },
/*2003*/{ PAR_INT32_RO,  (void *)&g_device.func_i32_2,         nullptr, nullptr },
/*2004*/{ PAR_INT16_RO,  (void *)&g_device.func_i16_1,         nullptr, nullptr },
/*2005*/{ PAR_INT16_RO,  (void *)&g_device.func_i16_2,         nullptr, nullptr },
/*2006*/{ PAR_FLOAT_RO,  (void *)&g_device.func_fl_1,          nullptr, nullptr },
/*2007*/{ PAR_FLOAT_RO,  (void *)&g_device.func_fl_2,          nullptr, nullptr },
/*2008*/{ PAR_INT32_RW,  (void *)&g_device.par_dummy_i32,      nullptr, nullptr },
/*2009*/{ PAR_INT16_RW,  (void *)&g_device.par_dummy_i16,      nullptr, nullptr },

///*200A*/{ PTENTRY_SPECIALFUNC(0, &g_device, PWdParMethod(&TDevice::pfn_big_data)) }, //
};

/*****************************************************************************************************************
                                       THE MAIN PARAMETER TABLE
******************************************************************************************************************/

const TParamRangeDef  param_range_table[] =
{
	{0x1000, 0x1018, nullptr, &g_device, PParRangeMethod(&TDevice::prfn_canobj_1008_1018) }, // some RO device identifiactions

	PAR_TABLE(0x2000, paramtable_2000), // special device parameters

	// close the list
	{0, 0, nullptr}
};

/*****************************************************************************************************************
******************************************************************************************************************/

