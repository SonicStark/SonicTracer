import os
import typing
import time

from .worker import ParallelWorker
from ..utility.log import GIVE_MY_LOGGER
from ..utility.text import dict2str

class TracerCoreRunner:
    """ A wrapper for running TracerCore

    Current implementation allows multiple tasks to be executed in parallel.
    The command line parameters passed to the subprocess will be managed by dict
    and indexed with an integer between 10000 and 99999. (`FixArgs` and `VarArgs`)
    
    As shown in the following table, the first 3 digits indicate the category of parameters. 
    The 4th digit indicates whether the parameters of this category are fixed (when equal to 0) 
    or variable (when equal to 1) for different tasks. 
    The 5th digit is used to number each parameter under the same category (which means that 
    the number of parameter strings in 1 category cannot exceed 10). For positional args, it is
    usually equal to 0. For keyword args, usually 0 is used to number the keyword itself, 
    while 1~9 are used to number the corresponding values.

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
        
        target_args_fix0 :typing.Optional[str],
        target_args_fix1 :typing.Optional[str],

        num_workers :int =2
    ) -> None:
        """ Constructor which receives fix args

        Please be careful, in this class we do NOT check the 
        validity of those parameters, but only saved, combined
        and passed them. Abnormal parameters may lead to 
        unexpected consequences.

        Parameters
        ----------
        num_workers:
            Number of workers in the pool. Default is 2.
        bin_pin:
            Argument category 10000
        bin_tool:
            Argument category 20001
        bin_target:
            Argument category 40000
        tool_ScaType:
            Argument category 21001. 0 means 'cal'. 1 means 'bbl'.
            2 means 'ins'. Otherwise 'bbl' as fallback.
        tool_CutName:
            Argument category 21101. Pass `[]` or `[""]`
            means shutting off the corresponding feature.
        target_args_fix0:
            Argument category 50000. In order to simplify the operation,
            parameters of target program are divided into three parts:
            fixed(before), variable and fixed(after). For each part, 
            parameters need to be merged into one string. For example,
            We need to run this against a `var_file`: 
            `/bin/a.out --debug -l /tmp/var_file -h -o /dev/null`.
            So `bin_target` is `/bin/a.out`, `target_args_fix0` is `--debug -l`,
            and `target_args_fix0` is `-h -o /dev/null`.
            Pass `None` to it when nothing is needed.
        target_args_fix1:
            Argument category 70000. Just like `target_args_fix0`
            except it will be placed after var args of target.
        """
        self.rlog = GIVE_MY_LOGGER()

        self.FixArgs = {
            10000 : bin_pin,
            10110 : "-logfile",
            20000 : "-t",
            20001 : bin_tool,
            20110 : "-logfile",
            21000 : "-TrScaType",
            21210 : "-TrDatPath",
            30000 : "--",
            40000 : bin_target
        }

        if (target_args_fix0 is not None):
            self.FixArgs[50000] = target_args_fix0
        if (target_args_fix1 is not None):
            self.FixArgs[70000] = target_args_fix1

        if   (0 == tool_ScaType):
            self.FixArgs[21001] = "cal"
        elif (1 == tool_ScaType):
            self.FixArgs[21001] = "bbl"
        elif (2 == tool_ScaType):
            self.FixArgs[21001] = "ins"
        else:
            self.FixArgs[21001] = "bbl"

        tcutn = ";".join(tool_CutName)
        if (2 <= len(tcutn)):
            self.FixArgs[21100] = "-TrCutName"
            self.FixArgs[21101] = "%s;"%tcutn

        self._n_workers = num_workers
        self.wPool = ParallelWorker(self.FixArgs, num_workers)
        self.rList = []
        self.wDone = False

    def apply(self, GenVarArgs :typing.Generator[None,None,tuple],
        stdin  :bool,
        output :bool =False,
        timeout :typing.Optional[int] =None
    ) -> None:
        """ Apply jobs to workers in the pool

        Parameters
        ----------
        GenVarArgs:
            A generator which produces the variable args info.
            It yield a tuple in once. The first item in the tuple
            should be a dict which contains variable args like
            those in `self.FixArgs`. Variable args may various
            between different jobs.
        stdin:
            If `False`, the tuple yielded by `GenVarArgs` should only
            contains one item. If `True`, the tuple should contains two.
            And the second item should be a file object like 
            `open("sth", mode="rb")`. The file content will be applied
            into stdin of target.
        output:
            If `False`, the job function returns (`Popen.returncode`,).
            Otherwise it returns (`Popen.returncode`, `Popen.stdout`, `Popen.stderr`).
        timeout:
            If the process which executes the job funtion does not terminate
            after timeout seconds, it will be killed. `None` shuts off this feature.
        """
        for va in GenVarArgs:
            VarArgs = va[0]
            if (stdin is False):
                stdin_ = None
            else:
                stdin_ = va[1]
            job_func = self.wPool.generate_job(VarArgs, 
                have_stdin  = stdin_,
                keep_output = output,
                timeout_sec = timeout
            )
            self.rList.append(
                self.wPool.apply_async(job_func))
            self.rlog.debug("Apply => %s", dict2str(self.wPool.args_common))
    
    def wait(self, refresh_sec :int =5) -> None:
        """ Wait for all jobs to complete

        `refresh_sec` is the time interval (in seconds)
        for polling the progress. Default is 5.
        """
        self.wPool.close()
        self.wDone = False

        job_num = len(self.rList)
        job_done = []
        job_wait = list(range(job_num))

        while (len(job_wait) > 0):
            self.rlog.info("Running: %d, Progress: %d/%d", 
                len(job_wait), len(job_done), job_num)

            job_wait_new = []
            for i in job_wait:
                if (self.rList[i].ready() is True):
                    job_done.append(i)
                else:
                    job_wait_new.append(i)
            job_wait = job_wait_new
            time.sleep(refresh_sec)

        self.wPool.join()
        self.wDone = True
        self.rlog.info("%d jobs have been done", job_num)

    def access(self) -> typing.Optional[typing.Generator[None,None,tuple]]:
        """ Access returned things from job functions

        One must call `self.wait` before using `self.access`.
        Otherwise it will return `None`. 
        """
        if (self.wDone is False):
            return None
        else:
            for i in range(len(self.rList)):
                self.rlog.debug("Access job %d", i)
                yield self.rList[i].get()

    def DefaultGenVarArgs(self,
        log_dir_path :str,

        tool_DatPath :typing.List[str],
        tool_SymPath :typing.Optional[typing.List[str]],

        target_args_var :typing.Optional[typing.List[str]],
        target_stdin    :typing.Optional[typing.List[typing.BinaryIO]]
    ) -> typing.Generator[None,None,tuple]:
        """ Default `GenVarArgs` which can be used for `self.apply`

        Calling `self.apply` requires a generator. Here we provide a
        default implementation if you don't know how to make one.
        It accepts several lists and same index means in same yield.
        Different workers rather than jobs will be assiged with
        different log files, so we can avoid resource conflicts
        with fewer file creation.

        To simplify, we DO NOT check the validity of those parameters
        here but your own implementation should consider this issue.

        Parameters
        ----------
        log_dir_path:
            Path of the directory used to store logs from Pin and PinTool.
        tool_DatPath:
            Argument category 21211. Expects a non-empty list.
        tool_SymPath:
            Argument category 21311. Pass `None` if not need this arg.
            Or it expects a list whose length equals to `tool_DatPath`.
        target_args_var:
            Argument category 60000. Pass `None` if not need this arg.
            Or it expects a list whose length equals to `tool_DatPath`.
        target_stdin:
            A list of file objects that need to be assigned
            to the yields. Pass `None` if nothing to assign.
            Or it must have same length with `tool_DatPath`.
        """
        lst_info = []
        lst_info.append(tool_DatPath)
        __len_or_non = lambda L: len(L) if (L is not None) else None
        lst_info.append(__len_or_non(tool_SymPath))
        lst_info.append(__len_or_non(target_args_var))
        lst_info.append(__len_or_non(target_stdin))
        for i in lst_info:
            if (i is not None) and (i != lst_info[0]):
                err_s = "Unmatched length: " \
                    "DatPath has %d but it has %d"%(lst_info[0],i)
                self.rlog.error(err_s)
                raise IndexError(err_s)

        log_res_pin  = []
        log_res_tool = []
        time_s = time.strftime("%y-%m-%d-%H-%M-%S.log", time.localtime())
        mlen = len(str(self._n_workers))
        for i in range(self._n_workers):
            log_res_pin.append(os.path.join(log_dir_path, 
                "TConsole-pin{}-{}".format(str(i+1).zfill(mlen), time_s)))
            log_res_tool.append(os.path.join(log_dir_path, 
                "TConsole-tool{}-{}".format(str(i+1).zfill(mlen), time_s)))

        for i in range(lst_info[0]):
            VarArgs = {
                10111 : log_res_pin [i%self._n_workers],
                20111 : log_res_tool[i%self._n_workers],
                21211 : tool_DatPath[i]
            }
            if (tool_SymPath is not None):
                VarArgs[21310] = "-TrSymPath"
                VarArgs[21311] = tool_SymPath[i]
            if (target_args_var is not None):
                VarArgs[60000] = target_args_var[i]
            if (target_stdin is not None):
                yield (VarArgs, target_stdin[i])
            else:
                yield (VarArgs,)