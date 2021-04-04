import textwrap


def pf():   #TODO save the result in a global variable instead of reading each time
    try :
        with open('./.engine/platform', 'r') as f:
        	return f.read(1)
    except FileNotFoundError:
        return ''

def tp():   #TODO save the result in a global variable instead of reading each time
    try:
        with open('./.engine/type', 'r') as f:
        	return f.read(1)
    except FileNotFoundError:
        return ''
def enginePath():   #TODO save the result in a global variable instead of reading each time
    with open('./.engine/enginePath', 'r') as f:
   		return f.read()