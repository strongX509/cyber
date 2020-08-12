# Discretionary Access Control

## Table of Contents
1. [Introduction](#section1)
2. [Linux Access Control Lists](#section2)
3. [Default Permissions](#section3)
4. [Set User and Group ID Access Control Flags](#section4)
5. [The Sticky Bit](#section5)

## Introduction <a name="section1"></a>

The authorization scheme of the *Linux* and *Unix* operating systems based on *Access Control Lists*  is a typical example of *Discretionary Access Control* where a *user* has total control over the access rights of his or her files. Whereas this allows for flexibility it also poses a grave security risk, e.g. if a file owner gives other users access to sensitive information or secret keys by intentionally or unwittingly changing the file permissions.

## Linux Access Control Lists <a name="section2"></a>

If we have a default `root` shell
```console
# id
uid=0(root) gid=0(root) groups=0(root)
```
we can switch to any other user with the `su` command
```console
# su -l hacker
$ id
uid=1000(hacker) gid=1000(hacker) groups=1000(hacker),...
$ pwd
/home/hacker
```
We create an empty file with the `touch` command
```console
$ touch testfile1
$ ls -l testfile1 
-rw-r--r-- 1 hacker hacker 0 Feb 13 10:11 testfile1
```
The *user* or *owner* of `testfile1` is `hacker` and the *group* is `hacker` as well. The Linux *Access Control List* (ACL) is of the form  `-rw-r--r--` and can be split into the following four fields:

| file type | user `u` | group `g` | others `o` |
| :-------: | :------: | :-------: | :--------: |
|   `-`     |  `rw-`   |   `r--`   |   `r--`    |

An alternative way to display the file properties is the `stat` command
```console
$ stat -c "%a %A %U %G" testfile1
644 -rw-r--r-- hacker hacker
```
and we see that the ACL can also be represented in the octal format `644`.

Next we want to create an *executable* `/bin/bash` script that prints out the current date
```console
$ echo -e '#!/bin/bash\ndate' > testfile2.sh
$ cat testfile2.sh                          
#!/bin/bash
date
```
The `stat` command shows that contrary to true binaries, scripts are not set to *executable* by default:
```console
$ stat -c "%a %A %U %G" testfile2.sh
644 -rw-r--r-- hacker hacker
```
With the `chmod a+x` command we make `testfile2.sh` *executable* for all, i.e. for user `u`, group  `g` and others `o`:
```console
$ chmod a+x testfile2.sh
$ stat -c "%a %A %U %G" testfile2.sh
755 -rwxr-xr-x hacker hacker
$ ./testfile2.sh 
Thu 13 Feb 2020 10:55:51 AM EST
```
With the information gathered from the `stat` commands above, we can derive the following general structure for the Linux *Access Control List* format:

| Flag (octal) | Flag (binary) | Flag (string) | Capability     |
| :----------: | :-----------: | :-----------: | :------------- |
|      1       |      001      |     `--x`     | Execute/Browse |
|      2       |      010      |     `-w-`     | Write          |
|      4       |      100      |     `r--`     | Read           |

## Default Permissions <a name="section3"></a>

The default permissions for newly created files or directories can be viewed with the `umask` command
```console
$ umask
022
```
Not the *granted permissions* are defined but rather the *forbidden capabilites* which is the complement of the permissions. Thus an `022` umask translates into `755` default permissions and they add up to `777`.

So if we don't want to give *read* permission to *others* and want to add *write* permission to *group* then we can do this manually change for existing files
```console
$ chmod o-r testfile*
$ chmod g+w testfile*
$ ls -l testfile*
-rw-rw---- 1 hacker hacker  0 Feb 13 10:11 testfile1
-rwxrwx--x 1 hacker hacker 17 Feb 13 10:50 testfile2.sh
```
and for future files change the default policy to
```console
$ umask 006
```
With these new defaults we create a `www` subdirectory
```console
$ mkdir www
$ stat -c "%a %A %U %G" www
771 drwxrwx--x hacker hacker
```
We note with satisfaction that the *read* permission for *others* is gone and *group* `hacker`has *write* capability. The problem now is that the `httpd` daemon running as `www-data` cannot read and modify any `html` files in the local `www` directory. We can fix this by changing the *group* ownership to *www-data* 
```console
$ sudo chgrp www-data www                                                 1 ↵
$ stat -c "%a %A %U %G" www
771 drwxrwx--x hacker www-data
```
Now we create an empty `html` file in the `www` subdirectory
```console
$ touch www/testfile3.html
$ stat -c "%a %A %U %G" www/testfile3.html
660 -rw-rw---- hacker hacker
```
Unfortunately  the created file inherits its group membership from user `hacker` and thus cannot be read by the `httpd` daemon. We could change the group manually using `chgrp` but this is quite tiresome for a large and dynamic website.

## Set User and Group ID Access Control Flags <a name="section4"></a>

A much smarter solution is to set the `setgid` flag in the ACL of the  `www` subdirectory
```console
sudo chmod g+s www
$ stat -c "%a %A %U %G" www
2771 drwxrws--x hacker www-data
```
If we now create a new subdirectory `htdocs` in `www` and add  some `html` files there, both subdirectories and files inherit the `www-data` group ownership.
```console
$ mkdir www/htdocs
$ stat -c "%a %A %U %G" www/htdocs
2771 drwxrws--x hacker www-data
$ touch www/htdocs/testfile4.html
$ stat -c "%a %A %U %G" www/htdocs/testfile4.html
660 -rw-rw---- hacker www-data
```
The `setgid` flag and its companion the `setuid` flag can be applied to *executable binaries* in order to run them with the access control rights of the file *owner* instead of the *user* executing the file. A typical example is the `sudo` command
```console
$ stat -c "%a %A %U %G" /usr/bin/sudo
4755 -rwsr-xr-x root root
```
that due to the `setuid` flag can be called by any *user* in order to execute commands as `root`.

## The Sticky Bit <a name="section5"></a>

The third special flag besides `setuid` and `setgid` is the so-called `sticky bit` that can be set on directories with *write* permissions for *all*, so that only the *owner* of a file located in this directory can delete or rename the file. A typical example is the `/tmp` directory where any user can create temporary files but would not want others to tamper with these files
```console
$ stat -c "%a %A %U %G" /tmp         
1777 drwxrwxrwt root root
```
Thus we can summarize the use of the special flags in the following table

| Flag (octal) | Flag (binary) |  ACL (u g o)  | Capability   |
| :----------: | :-----------: | :-----------: | :----------- |
|      1       |      001      | `--- --- --t` | Sticky Bit   |
|      2       |      010      | `--- --s ---` | Set Group ID |
|      4       |      100      | `--s --- ---` | Set User ID  |

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
