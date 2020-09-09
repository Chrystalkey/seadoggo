from logfuncs import *
from serverside_comparison import *
from conflict_resolve import *
from superglobals import *
from config_parse import *

log_hash = init_logfile_session()
configuration = parse_structure_conf_to_tree("../config/structure.conf")
dircodes = initialize_save_dirs(configuration, log_hash)
for user in configuration.keys():
    log_new_user({user:configuration[user]})
    for directory in configuration[user]["directories"]:
        ftp_credentials = {"host": configuration[user]["host"], "user": configuration[user]["user"], "pass": configuration[user]["pass"], "static_chroot": directory}
        difflist, bakNode, newNode = find_diffs("../files/"+dircodes[user+"/"+directory]+"/", ftp_credentials)
        print(str(difflist))
    '''
        try:
            resolve_conflicts(difflist, log_hash)
        except Exception as e:
            t_log("FATAL ERROR: "+str(type(e))+": "+str(e))
            '''
    