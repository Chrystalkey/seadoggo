## Seadoggo

This project is an incremental backup system requiring minimal invasion on client systems.
On the server this requires a http port on which the server can listen and a corresponding free port on the client for files to be sent.
Furthermore a directory with read/write access on the server. This backup is a one-way street as of now, it is not meant for synchronizing.
As of now, there is no authentication module, only the backup functionality, meaning every user connecting to it can backup its files.

The original source was written in python, now in the process of transitioning this into a portable binary lib
for use in servers and clients.

Next steps:
- complete client library for backup functionality
- add client authentication protocol
- add backsynchronization to an empty directory
- find a way in which only standard functions of linux/windows can replace the client side. (maybe some weird script with wget and curl? idk, lets see)
