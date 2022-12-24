import os
import re
import typing
from .log import GIVE_MY_LOGGER

def check_where_this_script_is(pass__file__here) -> str:
    """ Return absolute path of the directory where the current file is located
    """
    return os.path.dirname(os.path.abspath(pass__file__here))

def check_all_file_from_dir(
    root_dir_path   :str, 
    match_and_seize :typing.Optional[str] =None,
    preserve_filter :typing.Optional[str] =None,
    sort_path_str   :bool =True
) -> typing.Union[list , typing.Tuple[typing.List[str],typing.List[str]]]:
    """ Check and record all files in directory `root_dir_path` recursively

    If `match_and_seize` is `None`, only a list of file paths will be returned.

    Otherwise a regex string is supposed to be passed which will be used to
    perform a match on each of those file paths and 2 lists will be returned.
    The first list contains all file paths. The second list contains all matched
    strings (also called tags).

    If `preserve_filter` is `None`, every file found by 
    `os.walk(root_dir_path, followlinks=True)`
    will be preserved. Otherwise a regex string is supposed to be passed. Only if there 
    is 1 or more matches in the path string of a file, the file will be preserved.

    It is recommended to set `sort_path_str` to `True` which can avoid potential 
    adverse effect of path list in arbitrary order returned by `os.walk`.
    """
    if not os.path.isdir(root_dir_path):
        raise ValueError("Invalid directory path: {}".format(repr(root_dir_path)))
    try:
        if (match_and_seize is not None):
            pattern_mas = re.compile(match_and_seize)
    except BaseException as be:
        raise ValueError("Bad regex string {}".format(repr(match_and_seize))) from be
    try:
        if (preserve_filter is not None):
            pattern_pft = re.compile(preserve_filter)
    except BaseException as be:
        raise ValueError("Bad regex string {}".format(repr(preserve_filter))) from be

    MY_LOGGER = GIVE_MY_LOGGER()

    all_file_path = []
    for root, dirs, files in os.walk(root_dir_path, followlinks=True):
        for name in files:
            full_path = os.path.join(root, name)
            if (preserve_filter is None) or ((preserve_filter is not None) and (0 < len(pattern_pft.findall(full_path)))):
                all_file_path.append(full_path)
    
    all_file_cnt = len(all_file_path)
    if (0 == all_file_cnt):
        raise RuntimeError("Could not find any files in {}".format(repr(root_dir_path)))

    if (True == sort_path_str):
        all_file_path.sort()
    
    if (match_and_seize is None):
        MY_LOGGER.info("Get {} files in {}".format(all_file_cnt, repr(root_dir_path)))
        return all_file_path
    else:
        all_file_tag = []
        for F in all_file_path:
            matches = pattern_mas.findall(F)
            if (0 == len(matches)):
                raise RuntimeError("No target matched in {}".format(repr(F)))
            if (2 <= len(matches)):
                MY_LOGGER.warning("First one is used since multiple matches in {}".format(repr(F)))
            all_file_tag.append(matches[0])
        MY_LOGGER.info("Get {} tagged files in {}".format(all_file_cnt, repr(root_dir_path)))
        return all_file_path , all_file_tag