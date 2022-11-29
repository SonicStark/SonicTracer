#include "payload.h"

/**
 * Please refer to:
 * https://software.intel.com/sites/landingpage/pintool/docs/98650/Pin/doc/html/index.html#GRAN
 * 
 * Pin also offers an instruction instrumentation mode which lets the tool 
 * inspect and instrument an executable a single instruction at a time.
 * Instruction instrumentation utilizes the INS_AddInstrumentFunction API call.
 * 
 * Trace instrumentation lets the Pintool inspect and instrument an executable 
 * one trace at a time. Pin breaks the trace into basic blocks, BBLs.
 * Trace instrumentation utilizes the TRACE_AddInstrumentFunction API call.
 * 
 * Routine instrumentation lets the Pintool inspect and instrument an entire routine 
 * when the image it is contained in is first loaded. Routine instrumentation 
 * utilizes the RTN_AddInstrumentFunction API call.
 */

///////// INSTRUCTION   LEVEL /////////
/**
 * 
 */
AFUNPTR InstrumINS_Dat(){}

/**
 * 
 */
AFUNPTR InstrumINS_DatSym(){}

/**
 * 
 */
INS_INSTRUMENT_CALLBACK AnalyseINS(INS Iparam, VOID *Vparam){}

///////// BASIC BLOCK   LEVEL /////////
/**
 * 
 */
AFUNPTR InstrumBBL_Dat(){}

/**
 * 
 */
AFUNPTR InstrumBBL_DatSym(){}

/**
 * 
 */
TRACE_INSTRUMENT_CALLBACK AnalyseBBL(TRACE Tparam, VOID *Vparam){}

///////// FUNCTION CALL LEVEL /////////
/**
 * 
 */
AFUNPTR InstrumCAL_Dat(){}

/**
 * 
 */
AFUNPTR InstrumCAL_DatSym(){}

/**
 * 
 */
RTN_INSTRUMENT_CALLBACK AnalyseCAL(RTN Rparam, VOID *Vparam){}