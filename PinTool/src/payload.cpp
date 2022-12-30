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
VOID SaveDat(ADDRINT addr)
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
VOID SaveDatSym(ADDRINT addr, PIN_THREAD_UID tidv, std::string *psym)
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
VOID AnalyseINS(INS Iparam, VOID *Vparam)
{
    ADDRINT ins_addr = INS_Address(Iparam);
    if (!IsInsideMain(ins_addr)) { return; }
    else {
        if (IsBlocked(ins_addr)) { return; }
        else {
            if (!TrSym.is_open()) {
                INS_InsertCall(Iparam, IPOINT_BEFORE, AFUNPTR(SaveDat), 
                        IARG_ADDRINT, ins_addr,
                    IARG_END);
            } else {
                std::string ins_name; DumpSymInfo(ins_name, ins_addr);
                INS_InsertCall(Iparam, IPOINT_BEFORE, AFUNPTR(SaveDatSym),
                        IARG_ADDRINT, ins_addr,
                        IARG_UINT64,  PIN_ThreadUid(),
                        IARG_PTR,     SymPtrLst.GetSymPtr(ins_name),
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
VOID AnalyseBBL(TRACE Tparam, VOID *Vparam)
{
    for (BBL B__=TRACE_BblHead(Tparam); BBL_Valid(B__); B__=BBL_Next(B__)){
        ADDRINT bbl_addr = BBL_Address(B__);
        if (!IsInsideMain(bbl_addr)) { continue; }
        else {
            if (IsBlocked(bbl_addr)) { continue; }
            else {
                if (!TrSym.is_open()) {
                    BBL_InsertCall(B__, IPOINT_BEFORE, AFUNPTR(SaveDat),
                        IARG_ADDRINT, bbl_addr,
                    IARG_END);
                } else {
                    std::string bbl_name; DumpSymInfo(bbl_name, bbl_addr);
                    BBL_InsertCall(B__, IPOINT_BEFORE, AFUNPTR(SaveDatSym),
                        IARG_ADDRINT, bbl_addr,
                        IARG_UINT64,  PIN_ThreadUid(),
                        IARG_PTR,     SymPtrLst.GetSymPtr(bbl_name),
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
VOID AnalyseCAL(RTN Rparam, VOID *Vparam)
{
    if (!IsInsideMain(Rparam)) { return; }
    else {
        if (IsBlocked(Rparam)) { return; }
        else {
            if (!TrSym.is_open()) {
                RTN_InsertCall(Rparam, IPOINT_BEFORE, AFUNPTR(SaveDat),
                        IARG_ADDRINT, RTN_Address(Rparam), 
                    IARG_END);
            } else {
                std::string rname; DumpSymInfo(rname, Rparam);
                RTN_Open(Rparam);
                RTN_InsertCall(Rparam, IPOINT_BEFORE, AFUNPTR(SaveDatSym),
                        IARG_ADDRINT, RTN_Address(Rparam),
                        IARG_UINT64,  PIN_ThreadUid(),
                        IARG_PTR,     SymPtrLst.GetSymPtr(rname),
                    IARG_END);
                RTN_Close(Rparam);
            }
        }
    }
}