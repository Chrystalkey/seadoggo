a = { # for comparison lateron
    "newDirMiss" : 0,
    "bakDirMiss" : 1,
    "newFileMiss": 2,
    "bakFileMiss": 3,
    "noAction"   : 4,
#    "overwriteNew": 5,
    "overwriteBackup": 6,
    "transmissionError": 7
}

base_root = "/mnt/RAID_DEV/"
storage_chroot = base_root + "files/Archiv/"
bak_chroot = base_root + "backup_diffs/"
logroot = base_root + "logs/"
