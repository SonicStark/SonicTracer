import os
import yaml

__WHERE_CONFIG_IS = os.path.dirname(os.path.abspath(__file__))
def __GET_CONFIG(fname_no_ext :str) -> dict:
    """ Get config contents

    Give a config file name (without .yaml extension),
    then return a dict contains config contents read in.
    """
    try:
        config_fpath = os.path.join(__WHERE_CONFIG_IS, "{}.yaml".format(fname_no_ext))
        with open(config_fpath, mode="r", encoding="utf-8") as f:
            config_dict = yaml.safe_load(f)
        if not isinstance(config_dict, dict):
            raise TypeError("Invalid type: {}".format(type(config_dict)))
    except BaseException as be:
        raise RuntimeError("CANNOT read config item: {}".format(repr(fname_no_ext))) from be
    return config_dict

######################################################
CONFIG_FILTER = __GET_CONFIG("filter")
CONFIG_PATH   = __GET_CONFIG("path")
######################################################