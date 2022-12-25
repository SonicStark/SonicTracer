import json

def dict2str(D :dict) -> str:
    return json.dumps(D, sort_keys=True, separators=(',', ':'))

def dict2text(D :dict) -> str:
    return json.dumps(D, sort_keys=True, indent=4, separators=(',', ':'))
