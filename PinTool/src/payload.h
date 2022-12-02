#ifndef HEAD_PAYLOAD_H
#define HEAD_PAYLOAD_H

#include "pin.H"

VOID AnalyseINS(INS   Iparam, VOID *Vparam);
VOID AnalyseBBL(TRACE Tparam, VOID *Vparam);
VOID AnalyseCAL(RTN   Rparam, VOID *Vparam);

#endif