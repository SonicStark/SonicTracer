#ifndef HEAD_CHECKER_H
#define HEAD_CHECKER_H

#include "pin.H"
#include <string>
#include <vector>

inline bool IsBlockedName(const std::string &SN);
bool IsBlocked(const std::string &name);
bool IsBlocked(ADDRINT            addr);
bool IsBlocked(RTN               &rtni);

inline bool IsInsideMain(ADDRINT addr);
inline bool IsInsideMain(RTN    &rtni);

// There are only limited types of parameters can be accepted by
// an analyse routine. We should allocate some memory to place the
// symbol string and pass the pointer to analyse routine. Also, we should
// carefully treat our memory space to prevent sth unexpected. So we
// use a wrapper of vector to hold those pointers.
// It's unnecessary to consider thread-safe issues for this wrapper
// because Pin calls instrumentation routines while holding an internal lock.
// Ref:
// * https://software.intel.com/sites/landingpage/pintool/docs/98650/Pin/doc/html/group__INST__ARGS.html
// * pin-3.25-98650-g8f6168173-gcc-linux/source/tools/SimpleExamples/calltrace.cpp #L47
// * pin-3.25-98650-g8f6168173-gcc-linux/source/tools/SimpleExamples/trace.cpp #L72
// * https://software.intel.com/sites/landingpage/pintool/docs/98650/Pin/doc/html/index.html#MT
class SymPtrVector
{
    protected:
        std::vector<std::string *>           PtrVec;
        std::vector<std::string *>::iterator PtrVecIt;
    public:
        std::string* GetSymPtr(const std::string &SymStr);
        SymPtrVector();
        ~SymPtrVector();
}

#endif