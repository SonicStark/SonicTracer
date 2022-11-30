#include "payload.h"
#include "checker.h"
#include "cli.h"

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

SymPtrVector SymPtrLst; //manage string pointers
PIN_LOCK WriteFile; //the lock used in writing files

/**
 * Analyse Routine for saving address
 * @param addr memory address
 */
AFUNPTR SaveDat(ADDRINT addr)
{
    PIN_GetLock(&WriteFile, WriteFile._owner);
    TrDat << hexstr(addr) << std::endl;
    PIN_ReleaseLock(&WriteFile);
}

/**
 * Analyse Routine for saving address, thread-ID and symbol string
 * @param addr memory address
 * @param tidv thread ID
 * @param psym pointer of a symbol string
 */
AFUNPTR SaveDatSym(ADDRINT addr, PIN_THREAD_UID tidv, std::string *psym)
{
    PIN_GetLock(&WriteFile, WriteFile._owner);
    TrDat << hexstr(addr) << std::endl;
    TrSym << hexstr(tidv) << "," << *psym << std::endl;
    PIN_ReleaseLock(&WriteFile);
}

/**
 * Instrumentation Routine at instruction-level
 * @param Iparam Instruction Object
 * @param Vparam from default signature & unused
 */
INS_INSTRUMENT_CALLBACK AnalyseINS(INS Iparam, VOID *Vparam)
{
    ADDRINT ins_addr = INS_Address(Iparam);
    if (!IsInsideMain(ins_addr)) { return; }
    else {
        if (IsBlocked(ins_addr)) { return; }
        else {
            if (!TrSym.is_open()) {
                INS_InsertCall(Iparam, IPOINT_BEFORE, SaveDat, 
                        ins_addr, 
                    IARG_END);
            } else {
                std::string ins_name = RTN_FindNameByAddress(ins_addr);
                INS_InsertCall(Iparam, IPOINT_BEFORE, SaveDatSym,
                        ins_addr, PIN_ThreadUid(), SymPtrLst.GetSymPtr(ins_name), 
                    IARG_END);
            }
        }
    }
}

/**
 * Instrumentation Routine at basic-block-level
 * @param Tparam TRACE Object
 * @param Vparam from default signature & unused
 */
TRACE_INSTRUMENT_CALLBACK AnalyseBBL(TRACE Tparam, VOID *Vparam)
{
    for (BBL B__=TRACE_BblHead(Tparam); BBL_Valid(B__); B__=BBL_Next(B__)){
        ADDRINT bbl_addr = BBL_Address(B__);
        if (!IsInsideMain(bbl_addr)) { continue; }
        else {
            if (IsBlocked(bbl_addr)) { continue; }
            else {
                if (!TrSym.is_open()) {
                    BBL_InsertCall(B__, IPOINT_BEFORE, SaveDat,
                        bbl_addr,
                    IARG_END);
                } else {
                    std::string bbl_name = RTN_FindNameByAddress(bbl_addr);
                    BBL_InsertCall(B__, IPOINT_BEFORE, SaveDat,
                        bbl_addr, PIN_ThreadUid(), SymPtrLst.GetSymPtr(bbl_name),
                    IARG_END);
                }
            }
        }
    }
}

/**
 * Instrumentation Routine at function-call-level
 * @param Rparam Routine Object
 * @param Vparam from default signature & unused
 */
RTN_INSTRUMENT_CALLBACK AnalyseCAL(RTN Rparam, VOID *Vparam)
{
    if (!IsInsideMain(Rparam)) { return; }
    else {
        if (IsBlocked(Rparam)) { return; }
        else {
            if (!TrSym.is_open()) {
                RTN_InsertCall(Rparam, IPOINT_BEFORE, SaveDat,
                        RTN_Address(Rparam), 
                    IARG_END);
            } else {
                std::string rname = RTN_Name(Rparam);
                RTN_InsertCall(Rparam, IPOINT_BEFORE, SaveDatSym,
                        RTN_Address(Rparam), PIN_ThreadUid(), SymPtrLst.GetSymPtr(rname),
                    IARG_END);
            }
        }
    }
}