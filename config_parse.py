# -*- coding: utf-8 -*-
"""
Created on Fri Sep  4 19:53:03 2020

@author: Chrystalkey
"""

from logfuncs import *
import re
import json
import os.path

def parse_structure_conf_to_tree(filename):
    config_tree = dict()
    current_toplevel = "default"
    with open(filename, "r") as configfile:
        for line in configfile:
            comment_position = line.find("#")
            if(comment_position != -1):
                line = line[0:comment_position]
            if(line.startswith("#") or line.startswith(" ")): # lines must align perfectly and not be comments
                continue
            if(re.match("\s*\n", line) is not None): # discard empty lines
                continue
            
            if(line.startswith("[")):
                username = line[1:line.find("]")]
                
                user_invalid = False
                for symbol in "[]/\.?<>|*\"":
                    if(symbol in username):
                        user_invalid = True
                        break
                if(user_invalid):
                    current_toplevel = "default"
                    continue
                
                config_tree[username] = dict()
                current_toplevel = username
                continue
            
            if(current_toplevel == "default"): # discard everything until a user is named
                continue
            else:
                type_n_arg = line.replace("\n", "").split(" ", 1)
                config_tree[current_toplevel][type_n_arg[0].lower()] = type_n_arg[1]
                
                if(type_n_arg[0].lower() == "directories"):
                    config_tree[current_toplevel]["directories"] = re.split("\\s*:\\s*", config_tree[current_toplevel]["directories"])
                    continue
    for user in config_tree.keys():
        if(config_tree[user]["directories"] == None): # configuration must **MUST** include directories to backup
            config_tree[user] = None
        if(config_tree[user]["logintype"] == "ftp"):
            if(config_tree[user]["host"] == None or
               config_tree[user]["user"] == None or
               config_tree[user]["pass"] == None):
                config_tree[user] = None
    return config_tree
                
            
def validate_directory_codes(config_tree): # file is a json file
    entries = dict()
    todo = dict()
    
    with open("../config/dir.match", "r") as dirmatch:
        try:
            entries = json.load(dirmatch)
        except Exception as e:
            entries = dict()
        
        largest_directory_code = 0
        for value in entries.values():
            if(int(value) > largest_directory_code):
                largest_directory_code = int(value)
    
    with open("../config/dir.match", "a") as dirmatch:
        for user in config_tree.keys():
            for directory in config_tree[user]["directories"]:
                if(not user+"/"+directory in entries.keys()):
                    largest_directory_code += 1
                    todo[user+"/"+directory] = str(largest_directory_code)
                    entries[user+"/"+directory] = str(largest_directory_code)
        json.dump(entries, dirmatch)
    return (entries, todo)
        

def initialize_save_dirs(config_tree, logging_hash):
    dircodes, todo = validate_directory_codes(config_tree)
    
    for dircode in todo.values():
        if(not os.path.isdir("../files/"+dircode)):
            os.mkdir("../files/"+dircode)
    for user in config_tree.keys():
        if(not os.path.isdir("../diffs/"+logging_hash)):
            os.mkdir("../diffs/"+logging_hash)
        if(not os.path.isdir("../diffs/"+logging_hash+"/"+user)):
            os.mkdir("../diffs/"+logging_hash+"/"+user)
    return dircodes
        # make or validate directorycodes and directories
        # make or validate userdir in files, logs und diffs
        # logging configuration

if(__name__ == "__main__"):
    parsed = parse_structure_conf_to_tree("../config/structure.conf")
    print(parsed)