import datetime
import shutil
import sys
import os, fnmatch
from sys import platform

# search file in selected folder
def find(pattern, path):
    result = []
    for root, dirs, files in os.walk(path):
        for name in files:
            if fnmatch.fnmatch(name, pattern):
                result.append(os.path.join(root, name))
    return result

#copy file at install dir and add time stamp at its name
def copy(file):
    fname = os.path.basename(file)
    ind = fname.find('.')
    fname =inst_p+fname[:ind] + '_'+date_time_str + fname[ind:]
    print("---",fname)
    shutil.copy(file, fname)
    return fname

def make_win_shell_script(fname, path):
    if platform != "win32":
        return
    fsname = path+"/../win_utils/load_"+os.path.basename(fname)+"_agen.bat"
    file = open(fsname, 'w+')
    file.write("loader_util.exe -w "+fname+" com1,230400,1")
    pass

if __name__ == "__main__":
    # need path tobe given as arguments
    path = sys.argv[1]
    print("-- Post binnary install")
    # install path
    inst_p =path+"/bin/"
    #clean install path
    if os.path.exists(inst_p):
        shutil.rmtree(inst_p)
    os.mkdir(inst_p)
    #search HEX file
    hex_files = find('*.hex', path)
    #get timestamp
    now = datetime.datetime.now()
    date_time_str = now.strftime("%Y%m%d%H%M%S")
    #copy
    for file in hex_files:
        fname = copy(file)
        # make_win_shell_script(fname, path)


