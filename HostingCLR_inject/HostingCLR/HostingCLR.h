#pragma once
#include <io.h>
#include <stdio.h>
#include <tchar.h>
#include <metahost.h>
#pragma comment(lib, "MSCorEE.lib")

#import "C:\Windows\Microsoft.NET\Framework64\v4.0.30319\mscorlib.tlb" raw_interfaces_only high_property_prefixes("_get","_put","_putref") rename("ReportEvent", "InteropServices_ReportEvent")
#import "C:\Windows\Microsoft.NET\Framework64\v2.0.50727\mscorlib.tlb" raw_interfaces_only high_property_prefixes("_get","_put","_putref") rename("ReportEvent", "InteropServices_ReportEvent")

using namespace mscorlib;

VOID Execute(LPVOID lpPayload);
BOOL FindVersion(void * assembly);
