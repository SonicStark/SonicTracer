#include "checker.h"
#include "cli.h"

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