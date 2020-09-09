from os.path import isfile
from hashlib import sha512
from random import randint
from os import mkdir, listdir
from os.path import getmtime, join
from datetime import datetime
from time import time
import json

from superglobals import *

logging_hash = ""  # generate for every backup
base_dir = base_root  # dir from where backup diffs and logs are split up
session_logfile = ""

def t_log(string):
    global session_logfile
    with open(session_logfile, "a") as tlog:
        tlog.write("["+str(time())+"] "+str(string)+"\n")
    return

def t_log_err(error):
    t_log("[ERROR]"+str(error))
    return

def t_log_filemove(filepath, new_filepath, hash):
    with open(base_dir+"logs/transitionlogs/"+logging_hash+".log", "a") as tlog:
        tlog.write("["+str(time())+"] "+"Stored file "+filepath+" as "+new_filepath+" hash: "+hash+"\n")
    return

def t_log_difflist(diff_list, username):
    global a
    with open(base_dir+"logs/"+logging_hash+".log", "a") as tlog:
        tlog.write("\n\n"+"["+str(time())+"] "+"DIFFLIST: \n")
        for cate in a.keys():
            tlog.write(cate+"("+str(a[cate])+"):\n")
            entry_listing = str()
            for entry in diff_list[a[cate]]:
                entry_listing += str(entry)+str(":")
            tlog.write(str(entry_listing[:-1]+u"\n"))
        tlog.write("DIFFLIST END\n")
        tlog.flush()
        with open("../diffs/"+logging_hash+"/"+username+".json", "w") as jsonlog:
            json.dump(a, jsonlog)
            jsonlog.flush
    return

def last_uploadtime():
    last_upload = 0
    if(isfile("../logs/backup.time")):
        with open("../logs/backup.time","r") as f:
            number = f.read()
            if(number == ""):
                pass
            else:
                last_upload = int(number)
    with open("../logs/backup.time","w") as f:
        f.write(str(int(time())))
    return last_upload

def init_logfile_session():
    global logging_hash
    global session_logfile
    alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVXYZ0123456789"
    randseq = ""
    now_datetime = str(datetime.now().isocalendar())

    for _ in range(1024):
        randseq += alphabet[randint(0, len(alphabet)-1)]
    logging_hash = sha512(randseq.encode("utf-8")).hexdigest()
    
    with open("../logs/backup.list", "a") as log:
        log.write(str(int(time()))+" " + logging_hash + " " + now_datetime + "\n")

    session_logfile = "../logs/" + logging_hash+ ".log"
    with open(session_logfile, "w") as tlog:
        tlog.write("Transition log from "+now_datetime+"; Nr.: "+logging_hash+"\n")
    return logging_hash

def log_new_user(single_user_config):
    global session_logfile
    with open(session_logfile, "a") as tlog:
        for user in single_user_config:
            tlog.write("\n\nNew files from User "+user+" credentials: "+single_user_config[user]["user"]+"@"+single_user_config[user]["host"]+"\n")

def log_new_directory(dirname):
    global session_logfile
    with open(session_logfile, "a") as tlog:
        tlog.write("\nBackup of Directory: "+ dirname)
'''
def read_difflist(log_dir):
    max_mtime = 0
    max_mtime_file = None
    for x in listdir(log_dir):
        if(getmtime(join(log_dir, x)) > max_mtime):
            max_mtime_file = join(log_dir, x)
            max_mtime = getmtime(max_mtime_file)

    string = str()
    with open(join(log_dir, max_mtime_file), "r", 1, "utf-8") as f:
        string = f.read()
    diffl_raw = string.split("DIFFLIST: \n")[1].split("DIFFLIST END")[0]
    diff_raw_split = diffl_raw.split("\n")[:-1]

    diffList = dict()
    for i in range(len(diff_raw_split)):
        if(i %2 == 0):
            diffList[int(diff_raw_split[i].split("(", 1)[1][0])] = diff_raw_split[i+1].split(":")
    return diffList
'''
if(__name__ is "__main__"):
    dl = read_difflist("/mnt/RAID_DEV/logs/transitionlogs/")