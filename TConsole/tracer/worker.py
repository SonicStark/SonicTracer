import typing
import subprocess
import multiprocessing.pool

class ParallelWorker(multiprocessing.pool.ThreadPool):
    """ Parallel Worker Interface

    Use `multiprocessing.pool.ThreadPool` to manage multiple
    subprocesses (i.e. the workers) which are created by
    `subprocess.Popen`.

    We use a dict to express the program to execute. Its
    key is int and its value is str. According to the size of
    the integer, append corresponding value to an empty list 
    from small to large, and this list will be passed to `Popen`.

    For example:
    The dict is `{-1:"/bin/ls", 100:"/tmp", 0:"-l", 20:"-h"}`
    So the list will be `["/bin/ls", "-l", "-h", "/tmp"]`

    Based on this mechanism, it is more flexible to assign 
    different jobs to different workers, especially when 
    there are many shared long args accross all jobs.
    """
    def __init__(self,
        args_common :typing.Dict[int,str], 
        n_workers   :int =2
    ) -> None:
        """ Global settings

        `args_common` stores shared args accross all jobs.
        `n_workers` is the number of worker processes to use.
        """
        self.args_common = args_common
        super().__init__(n_workers)

    def generate_job(self, 
        args_append :typing.Dict[int,str], 
        have_stdin  :typing.Optional[typing.BinaryIO] =None,
        keep_output :bool =False,
        timeout_sec :typing.Optional[int] =None
    ) -> typing.Callable:
        """ Return a function without args as job for a worker

        Parameters
        ----------
        args_append:
            Will be merged into `args_common` as a supplement.
            The merged one will be used for building the final list.
        have_stdin:
            `None` means nothing will be write to stdin. Otherwise
            it is usually a file object like `open("sth", mode="rb")`.
        keep_output:
            If `False`, the returned function returns `Popen.returncode`.
            Otherwise it returns `Popen.returncode`, `Popen.stdout` and `Popen.stderr`,
            and the last two are byte streams.
        timeout_sec:
            If the process does not terminate after timeout seconds, `Popen.kill()`
            will be called.
        """
        self.args_common.update(args_append)
        arg_idx = sorted(self.args_common.keys())
        arg_lst = []
        for i in arg_idx:
            arg_lst.append(self.args_common[i])

        if (keep_output is False):
            if (have_stdin is None):
                ######## keep_output: 0  have_stdin: 0 ########
                def job_function():
                    p_ = subprocess.Popen(arg_lst, shell=False)
                    try:
                        p_.wait(timeout=timeout_sec)
                    except subprocess.TimeoutExpired:
                        p_.kill()
                    finally:
                        return p_.returncode
                ###############################################
            else:
                ######## keep_output: 0  have_stdin: 1 ########
                def job_function():
                    p_ = subprocess.Popen(arg_lst, shell=False,
                                          stdin=have_stdin)
                    try:
                        p_.wait(timeout=timeout_sec)
                    except subprocess.TimeoutExpired:
                        p_.kill()
                    finally:
                        return p_.returncode
                ###############################################
        else:
            if (have_stdin is None):
                ######## keep_output: 1  have_stdin: 0 ########
                def job_function():
                    p_ = subprocess.Popen(arg_lst, shell=False,
                        stdout = subprocess.PIPE,
                        stderr = subprocess.PIPE)
                    try:
                        bout , berr = \
                            p_.communicate(timeout=timeout_sec)
                    except subprocess.TimeoutExpired:
                        p_.kill()
                    finally:
                        return p_.returncode, bout, berr
                ###############################################
            else:
                ######## keep_output: 1  have_stdin: 1 ########
                def job_function():
                    p_ = subprocess.Popen(arg_lst, shell=False,
                        stdin  = have_stdin,
                        stdout = subprocess.PIPE,
                        stderr = subprocess.PIPE)
                    try:
                        bout , berr = \
                            p_.communicate(timeout=timeout_sec)
                    except subprocess.TimeoutExpired:
                        p_.kill()
                    finally:
                        return p_.returncode, bout, berr
                ###############################################
        
        return job_function