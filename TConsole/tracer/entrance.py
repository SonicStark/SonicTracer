import os
import sys
import typing

from .worker import TIMEOUT_KILL_CODE
from .core import TracerCoreRunner
from ..utility.log import GIVE_MY_LOGGER
from ..utility.checker import check_all_file_from_dir
from ..utility.checker import check_all_file_hierarchy
from ..utility.checker import check_file_executable
from ..utility.checker import check_dir_access

from ..config import CONFIG_FILTER
from ..config import CONFIG_PATH

CMD_FILE_PLACEHOLDER = "_@_FILE_@_"
FALLBACK_WORKER_NUM = 2
FALLBACK_WORKER_CHK = 5

def GetTruePin(PinKitDir :str) -> str:
    """ Get the path of pin binary in `PinKit`
    """
    try:
        with open(os.path.join(PinKitDir, "KIT_DIR_TREE"), 
                mode="r", encoding="utf-8") as fpointer:
            pin_path = os.path.join(PinKitDir,
                fpointer.readline().strip(), "pin")
        if check_file_executable(pin_path):
            return pin_path
        else:
            raise IOError("%s is not executable"%(pin_path))
    except BaseException as be:
        raise RuntimeError("CANNOT find available pin") from be

class CoreLauncher:
    """ Launch TracerCoreRunner

    It wraps `TracerCoreRunner` to provide centralized parameter
    passing and validity check. As the outermost interface,
    it undertakes all interactions against module `TConsole.tracer`.
    """
    def __init__(self,
        dir_src :str,
        dir_dat :str,
        dir_sym :typing.Optional[str],

        target_sca :int,
        target_bin :str,
        target_arg :str,
        read_stdin :bool =False,
        force_ia32 :bool =False,

        worker_num :int =FALLBACK_WORKER_NUM,
        worker_chk :int =FALLBACK_WORKER_CHK,
        worker_dmp :bool =False,
        worker_tim :typing.Optional[int] =None
    ) -> None:
        """ Centralized parameter passing and checking

        Parameters
        ----------
        dir_src
            A directory acts like root in which the program
            search for file recursively. EVERY searched file
            will be used as a input sent to target. So be
            careful about your directory.
        dir_dat
            A directory in which the trace file save.
            The file hierarchy in `dir_src` will be copied
            and applied to `dir_dat` and the saved trace files
            are in the same relative location with those input
            files. For example, Target with input file from
            'dir_src/h0/h1/iFile' will have its trace file
            locates at 'dir_dat/h0/h1/iFile'.
        dir_sym
            A directory in which the trace-symbol file save.
            Pass `None` means do not save the symbol files.
            Hierarchy copying and file saving acts same as `dir_dat`.
        target_sca
            Indicates `TrScaType` for TracerCore. 0 means 'cal'.
            1 means 'bbl'. 2 means 'ins'. Otherwise 0 will be used
            as fallback and the error will be logged.
        target_bin
            Path of target executable binary.
        target_arg
            Command line args when executing the target.
            Use the string which equals to `CMD_FILE_PLACEHOLDER`
            to indicate the positions where we insert the path
            string of input file. This placeholder acts like
            '___FILE___' of honggfuzz, or '@@' of AFL.
        read_stdin
            Whether to redirect input file content to stdin
            of target. This can be `True` when `target_arg` 
            still contains the placeholder. But if it `False`
            and the placeholder can't be found in `target_arg`,
            an exception will be raised.
        force_ia32
            Will load PinTool compiled for IA-32 if `True`, 
            or the one compiled for Intel-64 will be loaded
            as default. In both situation the tool will be
            loaded by arg '-t' of pin. See 
            https://software.intel.com/sites/landingpage/pintool/docs/98650/Pin/doc/html/index.html#EX
            for more details.
        worker_num
            How many workers in the pool. Executing 
            target program according to different inputs 
            will be considered as different job and then 
            submitted to the pool for allocation to the workers.
        worker_chk
            How many seconds between each start poll. After all jobs
            are submitted, the pool will close and will not accept 
            new submissions. Then a polling process will start, 
            and every few seconds, completion status of the submitted
            jobs will be queried until all done.
        worker_dmp
            Tell the workers whether to dump the stdin and stdout of
            each job. If `True`, they will be logged at `logging.DEBUG`
            level finally.
        worker_tim
            Timeout for each job. If the timeout occurs, the job will
            not continue to execute by the worker. `TIMEOUT_KILL_CODE`
            will be used as the so-called `returncode` of these jobs.
        """
        self.clog = GIVE_MY_LOGGER()
        self.OpenFileList = []

        if isinstance(worker_num, int) and (worker_num >= 1):
            self.worker_num = worker_num
        else:
            self.worker_num = FALLBACK_WORKER_NUM
        self.clog.info("Setup %d workers in the pool", self.worker_num)

        if isinstance(worker_chk, int) and (worker_chk >= 1):
            self.worker_chk = worker_chk
        else:
            self.worker_chk = FALLBACK_WORKER_CHK
        self.clog.info("Will check state of the workers"
                       " every %d sec.", self.worker_chk)
        
        if (worker_dmp is True):
            self.worker_dmp = True
            self.clog.warning("STDOUT & STDERR of each job for workers will "
            "be kept and then dumped to log. This may be noisy and "
            "cause unexpected huge memory consumption.")
        else:
            self.worker_dmp = False

        if (worker_tim is not None) and isinstance(worker_tim,int):
            self.worker_tim = worker_tim
        else:
            self.worker_tim = None
        
        if (read_stdin is True):
            self.read_stdin = True
        else:
            self.read_stdin = False

        ########## get info about pin & pintool ##########

        self.pin = GetTruePin(CONFIG_PATH["DIR_PIN_KIT"])
        if (force_ia32 is True):
            so = "build/ia32/TracerCore.so"
        else:
            so = "build/intel64/TracerCore.so"
       
        self.pintool = os.path.join(CONFIG_PATH["DIR_PIN_TOOL"], so)
        if not check_file_executable(self.pintool):
            err_s = "%s is not an executable file"%(self.pintool)
            self.clog.error(err_s)
            raise ValueError(err_s)
        
        self.pintool_cut = []
        __CF = CONFIG_FILTER["KEYWORD_BLOCKED"]
        if not isinstance(__CF, list):
            self.clog.error("No filter deployed because of "
                "invalid filter type: %s", type(__CF))
        else:
            for F in __CF:
                if isinstance(F, str) and (len(F) > 0):
                    self.pintool_cut.append(F)
                else:
                    self.clog.error("Ignore filter rule %s", repr(F))
        
        self.pintool_sca = 0
        if isinstance(target_sca, int) and (target_sca in [0,1,2]):
            self.pintool_sca = target_sca
        else:
            self.clog.error("Use default ScaType 0 "
                            "(Bad value: %s)", repr(target_sca))

        ######### get info about target #########

        self.target_bin = target_bin
        if not check_file_executable(self.target_bin):
            err_s = "%s is not an executable target"%(target_bin)
            self.clog.error(err_s)
            raise ValueError(err_s)
        
        self.target_arg_l = None
        self.target_arg_v = None
        self.target_arg_r = None
        arg_n_split = target_arg.count(CMD_FILE_PLACEHOLDER)
        if (0 == arg_n_split):
            if not self.read_stdin:
                raise ValueError("Nowhere to pass input file")
            else:
                self.target_arg_l = target_arg
        elif (1 == arg_n_split):
            llr = target_arg.split(CMD_FILE_PLACEHOLDER)
            __l, __r = llr[0].strip(), llr[1].strip()
            if (len(__l) != 0):
                self.target_arg_l = __l
            if (len(__r) != 0):
                self.target_arg_r = __r
            self.target_arg_v = lambda S: S
        else:
            self.clog.warning("Multiple `CMD_FILE_PLACEHOLDER` found. "
            "This is very rare, make sure it is correct: %s", target_arg)
            self.target_arg_v = lambda S: target_arg.replace(CMD_FILE_PLACEHOLDER, S)

        self.__init_resource(dir_src, dir_dat, dir_sym)
        self.runner = TracerCoreRunner(self.pin, self.pintool, self.target_bin,
            self.pintool_sca, self.pintool_cut, self.target_arg_l, self.target_arg_r,
            self.worker_num)

    def __init_resource(self, dsrc, ddat, dsym) -> None:
        """ Sub-init about IO resources
        """
        # about logs from pin & pintool
        self.save_logs = CONFIG_PATH["DIR_SAVELOGS"]
        if not check_dir_access(self.save_logs):
            self.save_logs = CONFIG_PATH["DIR_FALLBACK"]
            if not check_dir_access(self.save_logs):
                err_s = "Nowhere to save logs from pin & pintool. "
                "Bad directory path: %s"%(repr(self.save_logs))
                self.clog.error(err_s)
                raise IOError(err_s)
        
        # about source
        if check_dir_access(dsrc):
            self.dsrc = dsrc
            self.fsrc = check_all_file_from_dir(dsrc)
        else:
            err_s = "Bad src directory path: %s"%(repr(dsrc))
            self.clog.error(err_s)
            raise IOError(err_s)

        # about DatPath
        if check_dir_access(ddat):
            self.ddat = ddat
            self.fdat_get = lambda p: p.replace(self.dsrc, self.ddat, 1)
        else:
            err_s = "Bad TrDatPath base: %s"%(repr(ddat))
            self.clog.error(err_s)
            raise IOError(err_s)

        # about SymPath
        self.dsym = None
        if (dsym is not None) and check_dir_access(dsym):
            self.dsym = dsym
            self.fsym_get = lambda p: p.replace(self.dsrc, self.dsym, 1)
        elif (dsym is not None):
            err_s = "Bad TrSymPath base: %s"%(repr(dsym))
            self.clog.error(err_s)
            raise IOError(err_s)

        # init file hierarchy
        try:
            hierarchy = check_all_file_hierarchy(self.fsrc)
            for H in hierarchy.keys():
                os.makedirs(self.fdat_get(H), exist_ok=True)
                if (self.dsym is not None):
                    os.makedirs(self.fsym_get(H), exist_ok=True)
        except BaseException as be:
            raise RuntimeError("CANNOT init hierarchy") from be

    def launch(self) -> None:
        """ Submit all jobs to Pool
        """
        L_tool_DatPath    = []
        L_tool_SymPath    = None
        L_target_args_var = None
        L_target_stdin    = None
        if (self.dsym is not None):
            L_tool_SymPath = []
        if (self.target_arg_v is not None):
            L_target_args_var = []
        if self.read_stdin:
            L_target_stdin = self.OpenFileList #is ref, not copy!
        
        for fp in self.fsrc:
            L_tool_DatPath.append(self.fdat_get(fp))
            if (self.dsym is not None):
                L_tool_SymPath.append(self.fsym_get(fp))
            if (self.target_arg_v is not None):
                L_target_args_var.append(self.target_arg_v(fp))
            if self.read_stdin:
                L_target_stdin.append(open(fp, mode="rb"))

        vargs = self.runner.DefaultGenVarArgs(self.save_logs,
                        L_tool_DatPath, L_tool_SymPath, 
                        L_target_args_var, L_target_stdin)

        self.runner.apply(vargs, self.read_stdin, 
                        self.worker_dmp, self.worker_tim)
        self.clog.info("Friendly UAV overhead.")

    def landing(self) -> None:
        """ Wait for all jobs done and check the results
        """
        self.runner.wait(refresh_sec = self.worker_chk)
        results = self.runner.access()
        if (results is None):
            # Since `self.runner.wait` has returned, it should never be
            # `None` here. We keep this if-branch here just for fun :-)
            self.clog.warning("Unable to land. Mayday! Mayday!")
            return
        self.clog.info("Have a safe landing.")
        
        rcode = {TIMEOUT_KILL_CODE: []}
        rindx = -1
        for ret in results:
            rindx += 1
            if (ret[0] == TIMEOUT_KILL_CODE):
                rcode[TIMEOUT_KILL_CODE].append(rindx)
            else:
                if (ret[0] in rcode):
                    rcode[ret[0]].append(rindx)
                else:
                    rcode[ret[0]] = [rindx,]
            if self.worker_dmp:
                self.clog.debug("JOB%d_STDOUT >>>%s<<< JOB%d_STDOUT",
                    rindx, ret[1].decode(sys.stdout.encoding), rindx)
                self.clog.debug("JOB%d_STDERR >>>%s<<< JOB%d_STDERR",
                    rindx, ret[2].decode(sys.stderr.encoding), rindx)
        
        assert (1+rindx == len(self.fsrc)) #Input file num should equals to job num
        for rc in rcode:
            if (rc == TIMEOUT_KILL_CODE):
                __s = "timeout"
            else:
                __s = "code-%d"%rc
            self.clog.info("Input of %s:\n%s", __s, "\n".join(
                ["Job%d <= %s"%(idx, self.fsrc[idx])  for idx in rcode[rc] ]))
        
        s_attach = ""
        if (len(rcode) > 1):
            for rc in rcode:
                if (rc == TIMEOUT_KILL_CODE):
                    continue
                s_attach += ", code-%d=%d"%(rc, len(rcode[rc]))
        self.clog.info("Report: total %d [timeout=%d%s]", 
            (1+rindx), len(rcode[TIMEOUT_KILL_CODE]), s_attach)

    def __del__(self) -> None:
        """ Try to close those files in open
        """
        if hasattr(self, "OpenFileList") and \
                isinstance(self.OpenFileList, list):
            for ofile in self.OpenFileList:
                if not ofile.closed:
                    ofile.close()