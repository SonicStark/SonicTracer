import os
import sys
import time
import logging

from ..config import CONFIG_PATH
from ..config import CONFIG_BASIC

DEFAULT_LOG_LEVEL = logging.INFO

def GIVE_MY_LOGGER(name :str ="TConsole_root") -> logging.Logger:
    """
    https://docs.python.org/3.8/library/logging.html#logging.getLogger

    https://docs.python.org/3/library/time.html#time.strftime

    https://docs.python.org/3/library/logging.html#logrecord-attributes
    """
    LGR = logging.getLogger(name)
    if not (LGR.hasHandlers()):
        FMT_DATE = "%y.%m.%d %H:%M:%S"
        FMT_MAIN = "%(asctime)s,%(msecs)d %(filename)s:%(lineno)d %(levelname)s: %(message)s"
        FMT = logging.Formatter(FMT_MAIN, datefmt=FMT_DATE)

        SCN = logging.StreamHandler(sys.stderr)
        SCN.setFormatter(FMT)
        LGR.addHandler(SCN)

        if (0 != len(CONFIG_PATH["DIR_SAVELOGS"])):
            lname = time.strftime("TConsole-%y-%m-%d-%H-%M-%S.log", time.localtime())
            FIL = logging.FileHandler(os.path.join(CONFIG_PATH["DIR_SAVELOGS"], lname))
            FIL.setFormatter(FMT)
            LGR.addHandler(FIL)

        if isinstance(CONFIG_BASIC["LOG_LEVEL"], int):
            LGR.setLevel(CONFIG_BASIC["LOG_LEVEL"])
        else:
            LGR.setLevel(DEFAULT_LOG_LEVEL)
    return LGR

if __name__ == "__main__":
    a_logger = GIVE_MY_LOGGER()
    a_logger.debug   ("This is debug   ")
    a_logger.info    ("This is info    ")
    a_logger.warning ("This is warning ")
    a_logger.error   ("This is error   ")
    a_logger.critical("This is critical")
