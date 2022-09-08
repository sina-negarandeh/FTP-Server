# FTP-Server

A simplified implementation of an FTP server. Created for the Computer Networks course at the University of Tehran.

## How To Install

This project consists of two parts, the server, and the client. To Use the server, run the following:

 cd Server
 make
 ./Server

And to use the client use:

 cd Client
 make
 ./Client

## How To Use

To use this project, first, you have to update the **config.json** files included in each of the Server and Client folders. Provide the ports from which the server is going to operate and the files the server can access.

You can use the following commands from the client side to manage your files on the FTP Server:

* USER [name], Its argument is used to specify the user’s name. Authenticates users.
* PASS [password], Its argument is used to specify the user's password. Authenticates users.
* PWD, Returns working directory name.
* MKD [directory path], Its argument is used to specify the directory path. Used for making directories.
* DELE [-f / -d] [filename / directory path], Its argument is used to specify attempt to remove directories or files. Removes directory entries.
* LS, Lists directory contents.
* CWD [path], Its argument is used to specify the desired path. Changes working directory.
* RENAME [previos_name] [new_name], The first argument specifies the previous name of the file, and the second argument specifies its new name. Changes names of files.
* RETR [name], Its argument is used to specify which file to download. Downloads files.
* QUIT, Used for logging out.

