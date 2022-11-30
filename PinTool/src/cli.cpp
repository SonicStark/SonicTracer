#include "cli.h"
#include "amsg.h"
#include <iostream>
#include <sstream>

/**
 * Command line option '-TrDatPath'
 * Must be explicitly specified as a writable file path.
 * WARNNING: 
 * If nothing is specified or contains non-existent folder, 
 * `TraceCore` will exit immediately.
 */
KNOB<std::string> KNOB_TrDatPath(
    KNOB_MODE_WRITEONCE,
    "TraceCoreCli",
    "TrDatPath",
    "", //set default value
    "Specify the path of output trace file."
);

/**
 * Command line option '-TrSymPath'
 * If not specified, no trace symbol file will be generated.
 * Otherwise a non-writable file path will make
 * `TraceCore` exit immediately.
 */
KNOB<std::string> KNOB_TrSymPath(
    KNOB_MODE_WRITEONCE,
    "TraceCoreCli",
    "TrSymPath",
    "", //set default value
    "Specify the path of output trace symbol file."
);

/**
 * Command line option '-TrCutName'
 * If not specified, the filter feature will be disabled.
 */
KNOB<std::string> KNOB_TrCutName(
    KNOB_MODE_WRITEONCE,
    "TraceCoreCli",
    "TrCutName",
    "", //set default value
    "Specify a series of function names whose "
    "all instruction addresses would be screened. "
    "Please append ';' at the end of each name for separation. "
    "Both reading names and finding matches are case insensitive."
);

/**
 * Command line option '-TrScaType'
 * 'Sca' means 'Scale' and
 * 'ins' => instruction   level
 * 'bbl' => basic block   level
 * 'cal' => function call level
 */
KNOB<std::string> KNOB_TrScaType(
    KNOB_MODE_WRITEONCE,
    "TraceCoreCli",
    "TrScaType",
    "bbl", //set default value
    "Specify the granularity of trace. "
    "Must be 'ins' or 'bbl' or 'cal'."
);

/**
 * Print out a summary of all command line options
 * WARNNING: They will be in `stderr` rather than `stdout`
 */
void disp_usage(){
    std::cout << "[+] TracerCore - a Pintool which produces the trace you need." << std::endl;
    std::cout << KNOB_BASE::StringKnobSummary() << std::endl;
}

/**
 * Convert each single byte character in the string to uppercase
 * @return A new string with all single byte characters in uppercase.
 */
std::string GetUpper(const std::string &stri){
    std::string aupper;
    std::transform(stri.begin(), stri.end(), std::back_inserter(aupper), ::toupper);
    return aupper;
}

// Global Variable
// Store the granularity level of execution trace.
// TL_INS => instruction   level
// TL_BBL => basic block   level
// TL_CAL => function call level
INT32 TrSca = TL_BBL;
/**
 * Initialize the value of `TrSca`
 * @return Whether the specified value is applied successfully
 */
INT32 init_TrSca(){
    const std::string sca = KNOB_TrScaType.Value();
    if      (sca.compare("ins")) { TrSca = TL_INS; }
    else if (sca.compare("bbl")) { TrSca = TL_BBL; }
    else if (sca.compare("cal")) { TrSca = TL_CAL; }
    else { return EVIL_ARG; }
    return GOOD_ARG;
}

// Global Variable
// Store the names from KNOB_TrCutName.
std::vector<std::string> TrCut;
/**
 * Initialize the value of `TrCut`
 * @return Always `GOOD_ARG`
 */
INT32 init_TrCut(){
    TrCut.clear();
    const std::string tcn = KNOB_TrCutName.Value();
    if (0 == tcn.size()) { return GOOD_ARG; }
    else {
        std::istringstream iss(tcn);
        std::string tmps;
        while (std::getline(iss, tmps, ';'))
            { TrCut.push_back(GetUpper(tmps)); }
        return GOOD_ARG;
    }
}

// Global Variable
// iostream against trace file
std::ofstream TrDat;
/**
 * Initialize the iostream against trace file
 * @return `GOOD_ARG` for success or `EVIL_ARG` for failed `open`
 */
INT32 init_TrDat(){
    TrDat.open(KNOB_TrDatPath.Value(), std::ios::out|std::ios::trunc);
    if (TrDat.is_open()) { return GOOD_ARG; }
    else { return EVIL_ARG; }
}

// Global Variable
// iostream against trace symbol file
std::ofstream TrSym;
/**
 * Initialize the iostream against trace symbol file
 * @return `GOOD_ARG` for no trace symbol file output or successful `open`.
 *         `EVIL_ARG` for a failed `open` call.
 */
INT32 init_TrSym(){
    const std::string tsp = KNOB_TrSymPath.Value();
    if (0 == tsp.size()) { return GOOD_ARG; }
    else {
        TrSym.open(tsp, std::ios::out|std::ios::trunc);
        if (TrSym.is_open()) { return GOOD_ARG; }
        else { return EVIL_ARG; }
    }
}

/**
 * Close those file streams if they are not closed
 * @param C from default signature & unused
 * @param V from default signature & unused
 */
FINI_CALLBACK fini_files(INT32 C, VOID *V){
    if (TrDat.is_open()) { TrDat.close(); }
    if (TrSym.is_open()) { TrSym.close(); }
}

/**
 * Print the ASCII Text Banner
 */
void disp_ascii_text_banner()
{
    using namespace std; cout << "[+] Welcome to Igor's powerful" << endl;
    cout << "'########:'########:::::'###:::::'######::'########:'########::" << endl;
    cout << "... ##..:: ##.... ##:::'## ##:::'##... ##: ##.....:: ##.... ##:" << endl;
    cout << "::: ##:::: ##:::: ##::'##:. ##:: ##:::..:: ##::::::: ##:::: ##:" << endl;
    cout << "::: ##:::: ########::'##:::. ##: ##::::::: ######::: ########::" << endl;
    cout << "::: ##:::: ##.. ##::: #########: ##::::::: ##...:::: ##.. ##:::" << endl;
    cout << "::: ##:::: ##::. ##:: ##.... ##: ##::: ##: ##::::::: ##::. ##::" << endl;
    cout << "::: ##:::: ##:::. ##: ##:::: ##:. ######:: ########: ##:::. ##:" << endl;
    cout << ":::..:::::..:::::..::..:::::..:::......:::........::..:::::..::" << endl;
}