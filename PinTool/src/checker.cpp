#include "checker.h"
#include "cli.h"
#include <string>

/**
 * Whether the name needs to be screened.
 * If sth unexpected occurs, false is returned by default.
 * @param SN name string
 * @return true or false
 */
inline bool IsBlockedName(const std::string &SN)
{
    if (SN.size() == 0) { return false; }
    else {
        std::string to_match = GetUpper(SN);
        for (auto cName : TrCut) {
            if (to_match.find(cName) != std::string::npos)
                { return true; }
        }
    }
}

/**
 * Whether the name needs to be screened.
 * If sth unexpected occurs, false is returned by default.
 * @param name Usually the name of a routine.
 * @return true or false
 */
bool IsBlocked(const std::string &name)
{
    if (0 == TrCut.size()) { return false; }
    return IsBlockedName(name);
}

/**
 * Whether the name which the address belongs to needs to be screened.
 * If sth unexpected occurs, false is returned by default.
 * @param addr memory address
 * @return true or false
 */
bool IsBlocked(ADDRINT addr)
{
    if (0 == TrCut.size()) { return false; }
    std::string name = RTN_FindNameByAddress(addr);
    return IsBlockedName(name);
}

/**
 * Whether the name of input routine needs to be screened.
 * If sth unexpected occurs, false is returned by default.
 * @param rtni Routine Object
 * @return true or false
 */
bool IsBlocked(RTN &rtni)
{
    if (0 == TrCut.size()) { return false; }
    std::string name = RTN_Name(rtni);
    return IsBlockedName(name);
}

/**
 * Whether the address is inside the image Pin was applied on in the command line.
 * @param addr memory address
 * @return true or false
 */
inline bool IsInsideMain(ADDRINT addr){
    return IMG_IsMainExecutable(
        IMG_FindByAddress(addr)
    );
}

/**
 * Whether the routine is inside the image Pin was applied on in the command line.
 * @param rtni Routine Object
 * @return true or false
 */
inline bool IsInsideMain(RTN &rtni){
    return IMG_IsMainExecutable(
        SEC_Img(RTN_Sec(rtni))
    );
}

// There are only limited types of parameters can be accepted by
// an analyse routine. We should allocate some memory to place the
// symbol string and pass the pointer to analyse routine. Also, we should
// carefully treat our memory space to prevent sth unexpected. So we
// use a vector to hold those pointers.
// It's unnecessary to consider thread-safe issues for this vector
// because Pin calls instrumentation routines while holding an internal lock.
// Ref:
// * https://software.intel.com/sites/landingpage/pintool/docs/98650/Pin/doc/html/group__INST__ARGS.html
// * pin-3.25-98650-g8f6168173-gcc-linux/source/tools/SimpleExamples/calltrace.cpp #L47
// * pin-3.25-98650-g8f6168173-gcc-linux/source/tools/SimpleExamples/trace.cpp #L72
// * https://software.intel.com/sites/landingpage/pintool/docs/98650/Pin/doc/html/index.html#MT
std::vector<std::string *> SymPtrLst;