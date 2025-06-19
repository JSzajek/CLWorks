#pragma once

#include "Stats/Stats.h"

DECLARE_STATS_GROUP(TEXT("OpenCL"), STATGROUP_OpenCL, STATCAT_Advanced);

DECLARE_FLOAT_COUNTER_STAT(TEXT("Kernel Time (ms)"), STAT_OpenCL_KernelTime, STATGROUP_OpenCL);

DECLARE_DWORD_COUNTER_STAT(TEXT("Total Compute Units"), STAT_OpenCL_TotalComputeUnits, STATGROUP_OpenCL);
DECLARE_DWORD_COUNTER_STAT(TEXT("Total Work Groups"), STAT_OpenCL_TotalWorkgroups, STATGROUP_OpenCL);
DECLARE_DWORD_COUNTER_STAT(TEXT("Active Kernels"), STAT_OpenCL_ActiveKernels, STATGROUP_OpenCL);

// TODO:: Implement
//DECLARE_MEMORY_STAT(TEXT("Active Global"), STAT_OpenCL_ActiveGlobalMemory, STATGROUP_OpenCL);
//DECLARE_MEMORY_STAT(TEXT("Active Local"), STAT_OpenCL_ActiveLocalMemory, STATGROUP_OpenCL);
DECLARE_MEMORY_STAT(TEXT("Hardware Global"), STAT_OpenCL_HardwareGlobalMemory, STATGROUP_OpenCL);
DECLARE_MEMORY_STAT(TEXT("Hardware Local"), STAT_OpenCL_HardwareLocalMemory, STATGROUP_OpenCL);
