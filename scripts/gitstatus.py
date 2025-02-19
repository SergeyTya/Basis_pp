import datetime
import subprocess
from sys import argv


# check git for uncommited changes
def isGitRepoFileChanged():
    process = subprocess.Popen("git status -s -uno", shell=True, stdout=subprocess.PIPE)
    out = process.stdout.read().strip()
    if(len(out) == 0):
        return False
    return True

# check git get version
def isGitGetHash():
    process = subprocess.Popen("git rev-parse --short HEAD", shell=True, stdout=subprocess.PIPE)
    out_hash = process.stdout.read().strip()
    process = subprocess.Popen("git rev-parse --abbrev-ref HEAD", shell=True, stdout=subprocess.PIPE)
    out_branch = process.stdout.read().strip()
    out = out_branch.decode("utf-8")+" "+out_hash.decode("utf-8")
    return out

# check git get version
def isGitGetHashNum():
    process = subprocess.Popen("git rev-parse --short HEAD", shell=True, stdout=subprocess.PIPE)
    out_hash = process.stdout.read().strip()
    out = out_hash.decode("utf-8")
    return out



if __name__ == "__main__":
    path = argv[1]
    file_name = path+'/gitversion_autogen.h'
    # get timestamp
    now = datetime.datetime.now()
    date_time_str = now.strftime("%Y-%m-%d %H:%M:%S")
    #check git version
    hash = isGitGetHash()
    hash_num = isGitGetHashNum()
    #create file 
    file = open(file_name, 'w+')
    try:
        file.writelines(['#define BUILD_INFO " ' ,hash+" "+date_time_str,'"\n'])
        file.writelines(['#define HASH_INFO "',hash_num+" ",'"\n'])
    finally:
        file.close()
    