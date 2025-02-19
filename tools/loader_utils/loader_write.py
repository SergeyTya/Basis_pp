import tkinter as tk
from tkinter import Tk     # from tkinter import Tk for Python 3.x
from tkinter.filedialog import askopenfilename
from subprocess import run
import platform

com_name = 'ttyUSB0'
com_bdr  = '460800'

if __name__ == "__main__":
    filetypes = (
        ('Hex files', '*.hex'),
        ('Bin files', '*.bin'),
    )

    Tk().withdraw() # we don't want a full GUI, so keep the root window from appearing
    filename = askopenfilename(filetypes=filetypes, initialdir="../build/") # show an "Open" dialog box and return the path to the selected file
    if filename is not None:
       
        if platform.system() == 'Linux':
            chk_cmd = ['./tools/loader_utils/loader_util', '-v']
            wrt_cmd = [ './tools/loader_utils/loader_util', '-w', f'{filename}', f'{com_name},{com_bdr},1' ]
        
        else:
            chk_cmd = ['./tools/loader_utils/loader_util.exe', '-v']
            wrt_cmd = [ './tools/loader_utils/loader_util.exe', '-w', f'{filename}', f'{com_name},{com_bdr},1' ]


        run(chk_cmd)
        run(wrt_cmd)

