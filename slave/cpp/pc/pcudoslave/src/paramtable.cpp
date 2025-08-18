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
#include "simple_scope.h"

// at some improper definitions (like missing TClass base) the tables were moved to .data (initialized RW data)
// and thus took twice so much space.  So we force these tables to .rodata with the following macro:
//#define PARAMTABLE_DEF   __attribute__((section(".rodata"))) const TParameterDef
#define PARAMTABLE_DEF   const TParameterDef
// if you get "session type conflict", then maybe some classes with callbacks are not based on the TClass

#define PAR_TABLE(astart, atab)    {astart, astart + (sizeof(atab) / sizeof(TParameterDef)) - 1, &atab[0], nullptr, nullptr}

#define PTENTRY_EMPTY         0, nullptr, nullptr, nullptr
#define PTENTRY_CONST(aval)   PAR_INT32_CONST,  (void *)(aval), nullptr, nullptr
#define PTENTRY_CONST8(aval)  PAR_UINT8_CONST,  (void *)(aval), nullptr, nullptr


/*****************************************************************************************************************
                                             PARAMETER TABLES
******************************************************************************************************************/
// range tables must come before the main

PARAMTABLE_DEF pt_2000_app[] =  // 0x2000
{
/* +00*/{ PAR_INT32_RO,  (void *)&g_device.irq_period_ns,      nullptr, nullptr },
/* +01*/{ PAR_UINT16_RO, (void *)&g_device.irq_cycle_counter,  nullptr, nullptr },
/* +02*/{ PAR_INT32_RO,  (void *)&g_device.func_i32_1,         nullptr, nullptr },
/* +03*/{ PAR_INT32_RO,  (void *)&g_device.func_i32_2,         nullptr, nullptr },
/* +04*/{ PAR_INT16_RO,  (void *)&g_device.func_i16_1,         nullptr, nullptr },
/* +05*/{ PAR_INT16_RO,  (void *)&g_device.func_i16_2,         nullptr, nullptr },
/* +06*/{ PAR_FLOAT_RO,  (void *)&g_device.func_fl_1,          nullptr, nullptr },
/* +07*/{ PAR_FLOAT_RO,  (void *)&g_device.func_fl_2,          nullptr, nullptr },
/* +08*/{ PAR_INT32_RW,  (void *)&g_device.par_dummy_i32,      nullptr, nullptr },
/* +09*/{ PAR_INT16_RW,  (void *)&g_device.par_dummy_i16,      nullptr, nullptr },

///* +0A*/{ PTENTRY_SPECIALFUNC(0, &g_device, PWdParMethod(&TDevice::pfn_big_data)) }, //
};

PARAMTABLE_DEF pt_5000_scope[] =  // 0x5000: scope
{
/* +00*/{ PTENTRY_CONST(SCOPE_VERSION) },            // scope version
/* +01*/{ PTENTRY_CONST(SCOPE_MAX_CHANNELS) },
/* +02*/{ PAR_UINT32_RO,   (void *)&g_scope.buffer_size,     nullptr, nullptr },
/* +03*/{ PAR_UINT32_RO,   (void *)&g_device.irq_period_ns,  nullptr, nullptr },
/* +04*/{ PTENTRY_EMPTY },
/* +05*/{ PTENTRY_EMPTY },
/* +06*/{ PTENTRY_EMPTY },
/* +07*/{ PTENTRY_EMPTY },
/* +08*/{ PAR_UINT8_RW,   (void *)&g_scope.cmd,       &g_scope, PUdoParMethod(&TScope::pfn_scope_cmd) },
/* +09*/{ PAR_UINT8_RO,   (void *)&g_scope.state,     nullptr, nullptr },
/* +0A*/{ PARF_BLOB,      nullptr,                    &g_scope, PUdoParMethod(&TScope::pfn_scope_data) },
/* +0B*/{ PAR_UINT8_RO,   (void *)&g_scope.sample_width,  nullptr, nullptr },
/* +0C*/{ PAR_UINT32_RO,  (void *)&g_scope.sample_count,  nullptr, nullptr },
/* +0D*/{ PAR_UINT32_RO,  (void *)&g_scope.trigger_index, nullptr, nullptr },
/* +0E*/{ PTENTRY_EMPTY },
/* +0F*/{ PTENTRY_EMPTY },
/* +10*/{ PAR_UINT16_RW,  (void *)&g_scope.smp_cycles,           nullptr, nullptr },
/* +11*/{ PAR_UINT32_RW,  (void *)&g_scope.max_samples,          nullptr, nullptr },
/* +12*/{ PAR_UINT8_RW,   (void *)&g_scope.trigger_channel,      nullptr, nullptr },
/* +13*/{ PAR_INT32_RW,   (void *)&g_scope.trigger_level,        nullptr, nullptr },
/* +14*/{ PAR_UINT8_RW,   (void *)&g_scope.trigger_slope,        nullptr, nullptr },
/* +15*/{ PAR_UINT32_RW,  (void *)&g_scope.trigger_mask,         nullptr, nullptr },
/* +16*/{ PAR_UINT32_RW,  (void *)&g_scope.pretrigger_percent,   nullptr, nullptr },
/* +17*/{ PTENTRY_EMPTY },
/* +18*/{ PTENTRY_EMPTY },
/* +19*/{ PTENTRY_EMPTY },
/* +1A*/{ PTENTRY_EMPTY },
/* +1B*/{ PTENTRY_EMPTY },
/* +1C*/{ PTENTRY_EMPTY },
/* +1D*/{ PTENTRY_EMPTY },
/* +1E*/{ PTENTRY_EMPTY },
/* +1F*/{ PTENTRY_EMPTY },
/* +20*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[ 0].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +21*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[ 1].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +22*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[ 2].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +23*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[ 3].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +24*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[ 4].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +25*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[ 5].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +26*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[ 6].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +27*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[ 7].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +28*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[ 8].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +29*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[ 9].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +2A*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[10].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +2B*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[11].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +2C*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[12].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +2D*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[13].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +2E*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[14].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
/* +2F*/{ PAR_UINT32_RW,  (void *)&g_scope.channels[15].datadef,   &g_scope, PUdoParMethod(&TScope::pfn_scope_def) },
};


/*****************************************************************************************************************
                                       THE MAIN RANGE TABLE
******************************************************************************************************************/
extern const TParamRangeDef  param_range_table[]; // this must be here to keep the table under

const TParamRangeDef  param_range_table[] =
{
	{0x1000, 0x1018, nullptr, &g_device, PParRangeMethod(&TDevice::prfn_canobj_1008_1018) }, // some RO device identifiactions

	PAR_TABLE(0x2000, pt_2000_app),   // main application parameters
	PAR_TABLE(0x5000, pt_5000_scope), // scope parameters

	// close the list
	{0, 0, nullptr, nullptr, nullptr}
};

/*****************************************************************************************************************
******************************************************************************************************************/

