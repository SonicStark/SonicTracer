#include "checker.h"
#include "cli.h"

/**
 * Dump symbol info of the input routine.
 * When macro `DISABLE_DUMP_SECTON_INFO` is 
 * not defined, the info string is in a format like
 * "<section name>+<offset>:<routine name>".
 * Otherwise the string equals to name of
 * the routine. 
 * If a `RTN_Invalid()` is given, the string is "".
 * @param s_recv recieves the info string
 * @param rtni Routine Object
 */
void DumpSymInfo(std::string &s_recv, RTN &rtni)
{
    if (!RTN_Valid(rtni)) { s_recv = ""; return; }
#ifndef DISABLE_DUMP_SECTON_INFO
    SEC rtn_inside = RTN_Sec(rtni);
    s_recv =  SEC_Name(rtn_inside);
    s_recv += "+";
    s_recv += hexstr(RTN_Address(rtni) - SEC_Address(rtn_inside));
    s_recv += ":";
    s_recv += RTN_Name(rtni);
#else
    s_recv = RTN_Name(rtni);
#endif
}

/**
 * Dump symbol info of the routine which the 
 * input address belongs to. 
 * When macro `DISABLE_DUMP_SECTON_INFO` is 
 * not defined, the info string is in a format like
 * "<section name>+<offset>:<routine name>".
 * Otherwise the string equals to name of
 * the routine. 
 * Will be "" if no valid routine is found.
 * @param s_recv Recieves the info string
 * @param addr memory address
 */
void DumpSymInfo(std::string &s_recv, ADDRINT addr)
{
#ifndef DISABLE_DUMP_SECTON_INFO
    PIN_LockClient();
    RTN rtn_found = RTN_FindByAddress(addr);
    if (!RTN_Valid(rtn_found)) { PIN_UnlockClient(); s_recv = ""; return; }
    SEC rtn_inside = RTN_Sec(rtn_found);
    s_recv =  SEC_Name(rtn_inside);
    s_recv += "+";
    s_recv += hexstr(RTN_Address(rtn_found) - SEC_Address(rtn_inside));
    s_recv += ":";
    s_recv += RTN_Name(rtn_found);
    PIN_UnlockClient();
#else
    s_recv = RTN_FindNameByAddress(addr);
#endif
}

/**
 * Whether the name needs to be screened.
 * If sth unexpected occurs, false is returned by default.
 * @param SN name string
 * @return true or false
 */
bool IsBlockedName(const std::string &SN)
{
    if (SN.size() == 0) { return false; }
    else {
        std::string to_match = GetUpper(SN);
        for (auto cName : TrCut) {
            if (to_match.find(cName) != std::string::npos)
                { return true; }
        }
        return false;
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
bool IsInsideMain(ADDRINT addr){
    IMG imgi = IMG_FindByAddress(addr);

    if (!IMG_Valid(imgi)) { return false; }
    else { return IMG_IsMainExecutable(imgi); }
}

/**
 * Whether the routine is inside the image Pin was applied on in the command line.
 * @param rtni Routine Object
 * @return true or false
 */
bool IsInsideMain(RTN &rtni){
    if (!RTN_Valid(rtni)) { return false; }
    IMG imgi = SEC_Img(RTN_Sec(rtni));

    if (!IMG_Valid(imgi)) { return false; }
    else { return IMG_IsMainExecutable(imgi); }
}

/**
 * Interface for operating `PtrVec`
 * @param SymStr A string waiting to be processed
 * @return Pointer to a copy of input string that can be used continuously.
 */
std::string* SymPtrVector::GetSymPtr(const std::string &SymStr){
    //try the last available one
    if (pSymCache && 0==pSymCache->compare(SymStr))
        { return pSymCache; }

    //try to find a available pointer
    std::vector<std::string *>::iterator it_;
    for (it_ = PtrVec.begin(); it_ != PtrVec.end(); ++it_) {
        if (0 == (*it_)->compare(SymStr)) {
            pSymCache = (*it_);
            return pSymCache;
        }
    }

    //for-loop ends without return, so create a new one
    pSymCache = new std::string(SymStr);
    PtrVec.push_back(pSymCache);
    return pSymCache;
}

/**
 * Constructor
 */
SymPtrVector::SymPtrVector(){
    pSymCache = 0;
}

/**
 * Destructor to prevent memory leak
 */
SymPtrVector::~SymPtrVector(){
    std::vector<std::string *>::iterator it_;
    //reset pSymCache
    pSymCache = 0;
    //free the memory
    for (it_ = PtrVec.begin(); it_ != PtrVec.end(); ++it_)
        { delete (*it_); }
    //erase the vector
    for (it_ = PtrVec.begin(); it_ != PtrVec.end();)
        { it_ = PtrVec.erase(it_); }
}