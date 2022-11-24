#include "cli.h"
#include "amsg.h"
#include <iostream>

KNOB <std::string> KNOB_TrDatPath(
    KNOB_MODE::KNOB_MODE_WRITEONCE,
    "TraceCoreCli",
    "TrDatPath",
    "", //set default value
    "Specify the path of output trace file."
);

KNOB <std::string> KNOB_TrSymPath(
    KNOB_MODE::KNOB_MODE_WRITEONCE,
    "TraceCoreCli",
    "TrSymPath",
    "", //set default value
    "Specify the path of output trace symbol file."
);

KNOB <std::string> KNOB_TrCutName(
    KNOB_MODE::KNOB_MODE_WRITEONCE,
    "TraceCoreCli",
    "TrCutName",
    "", //set default value
    "Specify a series of function names whose "
    "all instruction addresses would be screened. "
);

KNOB <std::string> KNOB_TrScaType(
    KNOB_MODE::KNOB_MODE_WRITEONCE,
    "TraceCoreCli",
    "TrScaType",
    "bbl", //set default value
    "Specify the granularity of trace. "
    "Must be 'ins' or 'bbl' or 'cal'."
);

void disp_usage(){
    std::cerr << "TracerCore - a Pintool which produces the trace you need." << std::endl;
    std::cerr << std::endl << KNOB_BASE::StringKnobSummary() << std::endl;
}

INT32 TrSca = TL_BBL;
INT32 init_TrSca(){
    const std::string __sca = KNOB_TrScaType.ValueString();
    if      (__sca.compare("ins")) { TrSca = TL_INS; }
    else if (__sca.compare("bbl")) { TrSca = TL_BBL; }
    else if (__sca.compare("cal")) { TrSca = TL_CAL; }
    else { return EVIL_ARG; }
    return GOOD_ARG;
}

std::vector<std::string> TrCut;
INT32 init_TrCut(){

}

std::ofstream TrDat;
std::ofstream TrSym;