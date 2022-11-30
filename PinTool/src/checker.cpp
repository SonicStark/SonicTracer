#include "checker.h"
#include "cli.h"
#include <algorithm>

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

/**
 * Interface for operating `PtrVec`
 * @param SymStr A string waiting to be processed
 * @return Pointer to a copy of input string that can be used continuously.
 */
std::string* SymPtrVector::GetSymPtr(const std::string &SymStr){
    //try the last available one
    if (PtrVecIt != PtrVec.end() && (*PtrVecIt)->compare(Symstr))
        { return (*PtrVecIt); }
    //try to find a available pointer
    PtrVecIt = std::find_if(PtrVec.begin(), PtrVec.end(),
        [](auto i__){
            if ((*i__)->compare(SymStr)) { return true; }
            else { return false; }
        }
    );
    //return an existed one or a new one
    if (PtrVecIt != PtrVec.end()) { return (*PtrVecIt); }
    else {
        std::string *pSymStr = new std::string(SymStr);
        PtrVec.push_back(pSymStr);
        return pSymStr;
    }
}

/**
 * Constructor
 */
SymPtrVector::SymPtrVector(){
    PtrVecIt = PtrVec.end();
}

/**
 * Destructor to prevent memory leak
 */
SymPtrVector::~SymPtrVector(){
    //free the memory
    std::for_each(PtrVec.begin(), PtrVec.end(),
        [](auto j__){ delete (*j__); });
    //erase the vector
    PtrVecIt = PtrVec.begin();
    for (; PtrVecIt != PtrVecIt.end();){
        { PtrVecIt = PtrVec.erase(PtrVecIt); }
}