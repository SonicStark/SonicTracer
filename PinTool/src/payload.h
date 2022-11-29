#ifndef HEAD_PAYLOAD_H
#define HEAD_PAYLOAD_H

#include "pin.H"

INS_INSTRUMENT_CALLBACK   AnalyseINS(INS   Iparam, VOID *Vparam);
TRACE_INSTRUMENT_CALLBACK AnalyseBBL(TRACE Tparam, VOID *Vparam);
RTN_INSTRUMENT_CALLBACK   AnalyseCAL(RTN   Rparam, VOID *Vparam);

#endif