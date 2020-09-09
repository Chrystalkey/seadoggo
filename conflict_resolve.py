import threading
from hashlib import sha512
from os import rename, rmdir, remove, popen
from pathlib import Path
from os.path import join
import ftplib
import tarfile
from shutil import rmtree
import subprocess, shlex

from superglobals import *
from logfuncs import *

upload_mutex = 0
remove_mutex = 0

def compress_directory(directory):
    logstring = "Compression init\n"
    logstring += popen(str("7za a -bb3 -bt -o. -sccUTF-8 -scsUTF-8 -snh -snl -sdel -t7z -mx9 -myx9 -mtc=on -mta=on "+
                           join(directory, "archive.7z")+ " " +
                           join(directory,"*").replace("\\", "/"))
                       ).read()
    logstring += "Compression finished\n"
    t_log(logstring)

class Backup_remove_thread(threading.Thread):
    def __init__(self, files, directories, dir_to):
        global remove_mutex
        remove_mutex += 1
        t_log("Initializing Remove thread...")
        threading.Thread.__init__(self)
        self.files = [x[1:] for x in files if x[0]=="/"]+[x for x in files if x[0] != "/"]
        self.directories = [x[1:] for x in directories if x[0] == "/"]+[x for x in directories if x[0] != "/"]
        self.dir_to = dir_to
        t_log("Remove Thread init done")
    def run(self):
        global remove_mutex
        #t_log("Archiving...")
        for file in self.files:
            hash = sha512(str(join("../files/", file)).encode("utf-8")).hexdigest()
            rename(join("../files/", file), join(self.dir_to, hash))
            #compress_in_place(join(self.dir_to, hash))
            t_log_filemove(join("../files/", file), join(self.dir_to, hash), hash)
        for dir in self.directories:
            try:
                rmtree(join("../files/", dir))
            except Exception as e:
                t_log_err(str(type(e)) + str(e))
            t_log("Removed Directory: "+join("/", dir))
        #compression of all files
        compress_directory(self.dir_to, user)
        remove_mutex -= 1
        t_log("Archivar thread finished")
        return

class New_File_Upload(threading.Thread):
    def __init__(self, files, directories, storage_chroot):
        global upload_mutex
        upload_mutex += 1
        t_log("Initializing Upload thread...")
        threading.Thread.__init__(self)
        self.files = [x[1:] for x in files if x[0]=="/"]+[x for x in files if x[0] != "/"]
        self.directories = [x[1:] for x in directories if x[0] == "/"]+[x for x in directories if x[0] != "/"]
        self.storage_chroot = storage_chroot
        t_log("Upload Thread Init done")
    def run(self):
        global upload_mutex
        #t_log("Uploading")
        for dir in self.directories:
            Path(join(self.storage_chroot, dir)).mkdir(parents=True, exist_ok=True)
            t_log("Created Directory: "+ join(self.storage_chroot, dir))
        ftp = ftplib.FTP(ftp_credentials["host"])
        ftp.login(ftp_credentials["user"], ftp_credentials["pass"])

        for file in self.files:
            handle = open(join(self.storage_chroot, file), "wb")
            try:
                ftp.retrbinary("RETR %s" % join(ftp_credentials["static_chroot"], file), handle.write)
                t_log("Successfully Copied File "+ join(ftp_credentials["static_chroot"], file) + " into " + join(self.storage_chroot, file))
            except Exception as e:
                t_log_err(str(type(e)) + "; " + str(e) + "; Filename: "+file)
        ftp.close()
        upload_mutex -= 1
        t_log("Upload Thread finished")

def resolve_conflicts(diff_list, logging_hash, user):
    global storage_chroot
    global bak_chroot

    bm_thread = Backup_remove_thread( diff_list[a["newFileMiss"]], diff_list[a["newDirMiss"]], join(bak_chroot, logging_hash), storage_chroot)
    upload = New_File_Upload(diff_list[a["bakFileMiss"]] + diff_list[a["overwriteBackup"]], diff_list[a["bakDirMiss"]], storage_chroot)
    bm_thread.start()
    upload.start()
    while(upload_mutex != 0 and remove_mutex != 0):
        pass