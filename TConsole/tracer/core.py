import typing
import time

from .worker import ParallelWorker

class TracerCoreRunner:
    """

    |  Index |  Argument  |
    |--------|------------|
    |  10000 | Pin bin path  |
    |  1011x | Pin log path  |
    |  2000x | Tool bin path |
    |  2011x | Tool log path |
    |  2100x | Tool TrScaType |
    |  2110x | Tool TrCutName |
    |  2121x | Tool TrDatPath |
    |  2131x | Tool TrSymPath |
    |  30000 | Double dash "--" |
    |  40000 | Target bin path  |
    |  5xx0x | Target fix args before |
    |  6xx1x | Target var args        |
    |  7xx0x | Target fix args after  |
    """
    def __init__(self,
        bin_pin    :str,
        bin_tool   :str,
        bin_target :str,
        
        tool_ScaType :int,
        tool_CutName :typing.List[str],
        
        target_args_fix0 :str,
        target_args_fix1 :str,

        num_workers :int =2
    ) -> None:
        """
        """
        self.FixArgs = {
            10000 : bin_pin,
            10110 : "-logfile",
            20000 : "-t",
            20001 : bin_tool,
            20110 : "-logfile",
            21000 : "-TrScaType",
            21210 : "-TrDatPath",
            30000 : "--",
            40000 : bin_target,
            50000 : target_args_fix0,
            70000 : target_args_fix1
        }

        if   (0 == tool_ScaType):
            self.FixArgs[21001] = "cal"
        elif (1 == tool_ScaType):
            self.FixArgs[21001] = "bbl"
        else:
            self.FixArgs[21001] = "ins"

        tcutn = ";".join(tool_CutName)
        if (2 <= len(tcutn)):
            self.FixArgs[21100] = "-TrCutName"
            self.FixArgs[21101] = "%s;"%tcutn

        self.wPool = ParallelWorker(self.FixArgs, num_workers)
        self.rList = []

    def apply_jobs(self, VarArgs :typing.Generator[None,None,tuple],
        stdin  :bool =False,
        output :bool =False,
        timeout :typing.Optional[int] =None
    ) -> None:
        """
        """
        for va in VarArgs:
            if (stdin is False):
                stdin_ = None
            else:
                stdin_ = va[1]
            job_func = self.wPool.generate_job(va[0], 
                have_stdin  = stdin_,
                keep_output = output,
                timeout_sec = timeout
            )
            self.rList.append(
                self.wPool.apply_async(job_func))
    
    def wait(self, refresh_sec :int =1) -> None:
        """
        """
        self.wPool.close()

        job_num = len(self.rList)
        job_done = []
        job_wait = list(range(job_num))

        while (len(job_wait) > 0):
            job_wait_new = []
            for i in job_wait:
                if (self.rList[i].ready() is True):
                    job_done.append(i)
                else:
                    job_wait_new.append(i)
            job_wait = job_wait_new
            time.sleep(refresh_sec)