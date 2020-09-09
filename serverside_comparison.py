from os import listdir
from os.path import isfile, join
from os.path import getmtime
import ftplib
import time
import datetime as dt

from superglobals import *
from logfuncs import *

class Node:
    def __init__(self, name, parent):
        self.name = name
        self.parent = parent
        self.found = False
class Dir_Node(Node):
    def __init__(self, name, parent):
        Node.__init__(self, name, parent)
        self.file_children = dict()
        self.dir_children = dict()
class File_Node(Node):
    def __init__(self, name, parent, mtime):
        Node.__init__(self, name, parent)
        self.mtime = mtime

def listdir_rec(static_chroot, node = None, root = "/"): # dont change the root
    local_files = dict()
    local_dirs = dict()
    access_root = static_chroot[:-1]+root
    for f in listdir(access_root):
        access_path = join(access_root, f)
        path = join(root, f)
        mtime = int(getmtime(access_path))
        try:
            if( isfile(access_path) ):
                local_files.update({path: [mtime, 0]})

                node.file_children.update({path: File_Node(path, node, mtime)})

            else:
                local_dirs.update({path: [mtime, 0]})

                dirnode = Dir_Node(path, node)
                node.dir_children.update({dirnode.name: dirnode})

                (x, y) = listdir_rec(static_chroot, dirnode, path)
                local_files.update(x)
                local_dirs.update(y)
        except Exception as e:
            t_log_err(e)
            exit(2) # 2 == transmission error or something else
    return (local_files, local_dirs)

def listdir_rec_ftp(ftp, node = None, root = "/"): # dont change the root
    #print(root)
    local_files = dict()
    local_dirs = dict()
    prop_list = None
    try:
        prop_list = list(ftp.mlsd("", facts=["modify", "type"]))
    except ftplib.all_errors as err:
        t_log_err(str(type(err)) + " : " + str(err))

    for response in prop_list:
            name = str(response[0]).encode("latin1").decode()
            path = join(root, name)
            mtime = int(dt.datetime(
                int(response[1]["modify"][0:4]),
                int(response[1]["modify"][4:6]),
                int(response[1]["modify"][6:8]),
                int(response[1]["modify"][8:10]),
                int(response[1]["modify"][10:12]),
                int(response[1]["modify"][12:14])).timestamp())
            if(response[1]["type"] == "dir" and name not in [".", ".."]):
                local_dirs.update({path: [mtime, 0]})

                dirnode = Dir_Node(path, node)
                node.dir_children.update({dirnode.name: dirnode})

                try:
                    ftp.cwd(response[0])
                    (x, y) = listdir_rec_ftp(ftp, dirnode, path)
                    ftp.cwd("..")
                except Exception as e:
                    t_log_err(e)
                    exit(3)  # 3 == ftp error

                local_files.update(x)
                local_dirs.update(y)
            elif(response[1]["type"] == "file"):
                node.file_children.update({path: File_Node(path, node, mtime)})

                local_files.update({path: [mtime, 0]})
            else:
                pass
    return (local_files, local_dirs)

def compare_trees(bNode, nNode):
    begin = time()
    diffList = dict()
    for b in a.keys():
        diffList[a[b]] = []

    last_upload = last_uploadtime()

    diffList = compare_trees_rec(bNode, nNode, diffList, last_upload)
    print("compare Trees: " + str(time()-begin))
    return diffList

def compare_trees_rec(bNode, nNode, dl, last_upload):
    if(bNode is None and nNode is None):
        return dl

    if(bNode is None):
        nNode.found = True
        for file in nNode.file_children.keys():
            dl[a["bakFileMiss"]].append(file)
            nNode.file_children[file].found = True
        for dir in nNode.dir_children.keys():
            dl[a["bakDirMiss"]].append(dir)
            compare_trees_rec(None, nNode.dir_children[dir],dl, last_upload)
        return
    if(nNode is None):
        bNode.found = True
        for file in bNode.file_children.keys():
            dl[a["newFileMiss"]].append(file)
            bNode.file_children[file].found = True
        for dir in bNode.dir_children.keys():
            dl[a["newDirMiss"]].append(dir)
            compare_trees_rec(bNode.dir_children[dir], None, dl, last_upload)
        return

    for bak in bNode.file_children.keys():
        equal = False
        for new in nNode.file_children.keys():
            if(bak == new):
                equal = True
                bNode.file_children[bak].found = True
                nNode.file_children[new].found = True
                if(nNode.file_children[new].mtime > last_upload):
                    dl[a["overwriteBackup"]].append(new)
                else:
                    dl[a["noAction"]].append(new)
                break
        if(not equal):
            dl[a["newFileMiss"]].append(bak)
            bNode.file_children[bak].found = True
    for file in [n for n in nNode.file_children.keys() if nNode.file_children[n].found == False]:
        dl[a["bakFileMiss"]].append(file)
        nNode.file_children[file].found = True

    for bak in bNode.dir_children.keys():
        equal = False
        for new in nNode.dir_children.keys():
            if(bak == new):
                equal = True
                dl[a["noAction"]].append(new)
                bNode.dir_children[bak].found = True
                nNode.dir_children[new].found = True
                compare_trees_rec(bNode.dir_children[bak], nNode.dir_children[new], dl, last_upload)
                break
        if(not equal):
            dl[a["newDirMiss"]].append(bak)
            bNode.dir_children[bak].found = True
            compare_trees_rec(bNode.dir_children[bak], None, dl, last_upload)
    for dir in [n for n in nNode.dir_children.keys() if nNode.dir_children[n].found == False]:
        dl[a["bakDirMiss"]].append(dir)
        nNode.dir_children[dir].found = True
        compare_trees_rec(None, nNode.dir_children[dir],dl,last_upload)

    return dl

def find_diffs(backupchroot, ftp_credentials):
    #print(backupchroot)
    t_log("Backup Chroot: " + backupchroot)
    bNode = Dir_Node("/", None)
    begin = time()
    (bfiles, bdirs) = listdir_rec(backupchroot, bNode)
    duration = time()-begin
    print("listdir_rec: " + str(duration))
    t_log("listdir_rec: "+ str(duration))
    #print("ben\n")
    #print(len(bfiles))
    #print(len(bdirs))
    t_log("Located "+str(len(bfiles)) + " files and "+str(len(bdirs)) + " directories in active backup")
    t_log("Connecting to remote file server")
    try:
        ftp = ftplib.FTP(ftp_credentials["host"])
        ftp.login(ftp_credentials["user"], ftp_credentials["pass"])
    except ftplib.error_temp as e:
        t_log_err(str(type(e)) + " : " + str(e))
    except ftplib.error_proto as e:
        t_log_err(str(type(e)) + " : " + str(e))
    except ftplib.error_reply as e:
        t_log_err(str(type(e)) + " : " + str(e))
    except ftplib.error_perm as e:
        t_log_err("FATAL "+str(type(e)) + " : " + str(e))
        exit(1)
    t_log("Done")

    for i in ftp_credentials["static_chroot"].split("/"):
        ftp.cwd(i)
    nNode = Dir_Node("/", None)
    begin = time()
    (nfiles, ndirs) = listdir_rec_ftp(ftp, nNode)
    duration = time()-begin
    print("listdir_rec_ftp: " + str(duration))
    t_log("listdir_rec_ftp: " + str(duration))
    #print("new\n")
    #print(len(nfiles))
    #print(len(ndirs))
    t_log("Located "+str(len(nfiles)) + " files and "+str(len(ndirs)) + " directories remotely")

    diffList = compare_trees(bNode, nNode)

    t_log_difflist(diffList)
    t_log("DiffList creation finished...\n")
    return diffList, bNode, nNode

if __name__ is "__main__":
    init_logfile_session()
    difflist, x, y = find_diffs(storage_chroot, ftp_credentials)
    print(":::::::::::::::::::::::::::::")
    print(len(difflist[a["newFileMiss"]]))
    print(len(difflist[a["bakFileMiss"]]))
    print(len(difflist[a["newDirMiss"]]))
    print(len(difflist[a["bakDirMiss"]]))
    print(len(difflist[a["overwriteBackup"]]))
    print(len(difflist[a["noAction"]]))
    print("ENDE")