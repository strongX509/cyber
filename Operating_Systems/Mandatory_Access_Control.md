# Mandatory Access Control

## Table of Contents
1. [Introduction](#section1)
2. [Security Enhanced Linux](#section2)
3. [Restricted Users](#section3)
4. [Security Context of Files](#section4)

## Introduction <a name="section1"></a>

[Security Enhanced Linux][SELINUX]  (SELinux) is an implementation of *Mandatory Access Control* (MAC). This is done mainly through *Type Enforcement* (TE) but it contains also aspects of *Role Based Access Control* (RBAC). An additional feature is *Multi Layer Security* (MLS) with an extension towards *Multi Category Security* (MCS).

The Fedora 31 VM image that we are going to work with comes with SELinux enabled
and running. Import the `Fedora-31.ova` image file into VirtualBox by double-clicking on it, start up and log in as user `hacker` with password `compass`.

[SELINUX]: https://en.wikipedia.org/wiki/Security-Enhanced_Linux

## Security Enhanced Linux <a name="section2"></a>

We execute the `sestatus` command and verify that the `targeted` SELinux policy is
enforced by *Fedora* out-of-the-box.
```console
$ sestatus
SELinux status:                 enabled
SELinuxfs mount:                /sys/fs/selinux
SELinux root directory:         /etc/selinux
Loaded policy name:             targeted
Current mode:                   enforcing
Mode from config file:          enforcing
Policy MLS status:              enabled
Policy deny_unknown status:     allowed
Memory protection checking:     actual (secure)
Max kernel policy version:      31
```
The `id` command shows the *security context* of subject `hacker` who is acting as user `unconfined_u` with role `unconfined_r`.
```console
$ id
uid=1000(hacker) gid=1000(hacker) groups=1000(hacker),10(wheel)
context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023
```
Executing the `ls -l -Z` command in the home directory of user `hacker` shows the
SELinux *security context* of the file objects
```console
$ ls -l -Z
total 32
drwxr-xr-x. 2 hacker hacker unconfined_u:object_r:user_home_t:s0  4096 Feb 10 17:53 Desktop
drwxr-xr-x. 2 hacker hacker unconfined_u:object_r:user_home_t:s0  4096 Feb 10 17:53 Documents
drwxr-xr-x. 2 hacker hacker unconfined_u:object_r:user_home_t:s0  4096 Feb 10 17:53 Downloads
drwxr-xr-x. 2 hacker hacker unconfined_u:object_r:audio_home_t:s0 4096 Feb 10 17:53 Music
drwxr-xr-x. 2 hacker hacker unconfined_u:object_r:user_home_t:s0  4096 Feb 10 17:53 Pictures
drwxr-xr-x. 2 hacker hacker unconfined_u:object_r:user_home_t:s0  4096 Feb 10 17:53 Public
drwxr-xr-x. 2 hacker hacker unconfined_u:object_r:user_home_t:s0  4096 Feb 10 17:53 Templates
drwxr-xr-x. 2 hacker hacker unconfined_u:object_r:user_home_t:s0  4096 Feb 10 17:53 Videos
```
We notice that the whole Linux file system has been labeled.

The user `hacker`becomes `root` by executing `sudo -s` . We see that the `uid` and `gid` have changed but the SELinux *security context* has remained the same.
```console
$ sudo -s
[sudo] password for hacker: compass
# id
uid=0(root) gid=0(root) groups=0(root) context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023
```
The command `ps -dZ` shows the SELinux *security context* of the process subjects. User processes are running as user `undefined_u` with role `unconfined_r`
under the domain `unconfined_t` whereas system processes run as `system_u`
and have role `system_r` and various special domains.
```console
$ ps -dZ
LABEL                               PID TTY          TIME CMD
system_u:system_r:kernel_t:s0         2 ?        00:00:00 kthreadd
system_u:system_r:kernel_t:s0         3 ?        00:00:00 rcu_gp
...
system_u:system_r:chronyd_t:s0      776 ?        00:00:00 chronyd
system_u:system_r:system_dbusd_t:s0-s0:c0.c1023 785 ? 00:00:00 dbus-broker
system_u:system_r:avahi_t:s0        789 ?        00:00:00 avahi-daemon
system_u:system_r:sssd_t:s0         794 ?        00:00:00 sssd_be
system_u:system_r:sssd_t:s0         799 ?        00:00:00 sssd_nss
system_u:system_r:unconfined_service_t:s0 1060 ? 00:00:00 VBoxClient
system_u:system_r:xdm_t:s0-s0:c0.c1023 1350 ?    00:00:00 gdm-session-wor
system_u:system_r:init_t:s0        1373 ?        00:00:00 (sd-pam)
unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023 1384 ? 00:00:00 gnome-keyring-d
...
unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023 3888 pts/0 00:00:00 sudo
unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023 3894 pts/0 00:00:00 bash
system_u:system_r:kernel_t:s0      3925 ?        00:00:00 kworker/0:0-ata_sff
system_u:system_r:kernel_t:s0      3928 ?        00:00:00 kworker/0:2-events
unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023 3933 pts/0 00:00:00 ps
```
The command `semanage user -l` shows which standard SELinux users are defined. 
```console
# semanage user -l
                Labeling   MLS/       MLS/                          
SELinux User    Prefix     MCS Level  MCS Range                      SELinux Roles
guest_u         user       s0         s0                             guest_r
root            user       s0         s0-s0:c0.c1023                 staff_r sysadm_r system_r unconfined_r
staff_u         user       s0         s0-s0:c0.c1023                 staff_r sysadm_r system_r unconfined_r
sysadm_u        user       s0         s0-s0:c0.c1023                 sysadm_r
system_u        user       s0         s0-s0:c0.c1023                 system_r unconfined_r
unconfined_u    user       s0         s0-s0:c0.c1023                 system_r unconfined_r
user_u          user       s0         s0                             user_r
xguest_u        user       s0         s0                             xguest_r
```
We notice that the default `unconfined_u` user profile has the same roles `unconfined_r`and `system_r` as the system user `system_u` running the operating system. With this pragmatic `unconfined_r` approach, Fedora appears to behave quite like a traditional  Linux system with *Discretionary Access Control* but even as `root` a user can never become `system_u`.

## Restricted Users <a name="section3"></a>

We now create a new user `cshannon` with the command
```console
# useradd -G wheel -c "Claude Shannon" cshannon
```
and set a password
```console
# passwd cshannon
Changing password for user cshannon.
New password: more-entropy
Retype new password: more-entropy
passwd: all authentication tokens updated successfully.
```
By adding `cshannon`  to the `wheel` group this user obtains `sudo` administrator rights. We open a new terminal window, log in with `ssh cshannon@localhost` and verify that the newly created user currently possesses the default `unconfined_u` security context
```console
[cshannon@localhost ~]$ id
uid=1001(cshannon) gid=1001(cshannon) groups=1001(cshannon),10(wheel)
context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023
```
The user `cshannon`is able to read the protected `etc/shadow` password file as `root`. 
```console
[cshannon@localhost ~]$ sudo tail -3 /etc/shadow
[sudo] password for cshannon: more-entropy 
tcpdump:!!:18192::::::
hacker:$6$oZ6Nkf96HI8OtogG$NkxTGML/Sg.kHv7nd1kWZbDcmsenMZRrcP9gqEIeurG2Yl.7Hno.b563k5lRLH2JATL1McgNv32.3dF0n9Z6S0:18307:0:99999:7:::
cshannon:$6$ojaeMefZtVmOR/2E$AKN2LDvm.nH9vwSvT/vqtKWwOSxerv6CLbSGCHowRMgXRcrpBRtd6RwHovuH2TtjZ0zsIubnRw0qc0Rt2qNA2.:18307:0:99999:7:::
```
Then we leave the secure shell with the `exit` command.
```console
[cshannon@localhost ~]$ exit
logout
Connection to localhost closed.
```
We switch back to the root terminal and associate the new `cshannon` account with the
restricted SELinux user profile `user_u`

```console
# semanage login -a -s user_u cshannon
```
Then we list the security context of all users with the command
```console
# semanage login -l
Login Name           SELinux User         MLS/MCS Range        Service
__default__          unconfined_u         s0-s0:c0.c1023       *
cshannon             user_u               s0                   *
root                 unconfined_u         s0-s0:c0.c1023       *
```
In the user terminal window we log in  with `ssh` again and verify with `id` that `cshannon` now possesses the `user_u` security context
```console
$ ssh cshannon@localhost
cshannon@localhost's password: more-entropy
Last login: Sat Feb 15 11:53:04 2020
[cshannon@localhost ~]$ id
uid=1001(cshannon) gid=1001(cshannon) groups=1001(cshannon),10(wheel) context=user_u:user_r:user_t:s0
```
Even though `cshannon` is still in the wheel group the `sudo` command doesn’t work anymore 
```console
[cshannon@localhost ~]$ sudo tail -3 /etc/shadow
sudo: unable to open /run/sudo/ts/cshannon: Permission denied
[sudo] password for cshannon: 
sudo: unable to stat /var/db/sudo: Permission denied
tail: cannot open '/etc/shadow' for reading: Permission denied
```
and SELinux generates a couple of `denied` entries in `/var/log/audit/audit.log`
```console
...
type=AVC msg=audit(1581764093.587:417): avc:  denied  { sys_resource } for  pid=4710 comm="sudo" capability=24  scontext=user_u:user_r:user_t:s0 tcontext=user_u:user_r:user_t:s0 tclass=capability permissive=0
...
```

## Security Context of Files <a name="section4"></a>

In the root window we start the Apache web server with
```console
# systemctl start httpd
```
and determine the security context the `httpd`daemon is running in
```console
# ps -efZ | grep httpd
system_u:system_r:httpd_t:s0  root    4805     1  0 12:08 ? 00:00:00 /usr/sbin/httpd -DFOREGROUND
system_u:system_r:httpd_t:s0  apache  4806  4805  0 12:08 ? 00:00:00 /usr/sbin/httpd -DFOREGROUND
...
```
With a text editor we create a `html`file `/var/www/html/index.html` with the content
```html
<html>
  <title>This is a test web page</title>
  <body>
    <h1>This is a test web page</h1>
  </body>
</html>
```
The security context of this newly created file is inherited from  the parent directory it was created in
```console
ls -lZ /var/www/html/index.html
-rw-r--r--. 1 root root unconfined_u:object_r:httpd_sys_content_t:s0 136 Feb 15 12:20 index.html
```
```console
# ls -lZ /var/www
drwxr-xr-x. 2 root root system_u:object_r:httpd_sys_script_exec_t:s0 4096 Oct  3 10:44 cgi-bin
drwxr-xr-x. 2 root root system_u:object_r:httpd_sys_content_t:s0     4096 Feb 15 12:20 html
```
Using the Firefox browser it should be possible to access the test page we created via the
URL `localhost`.

![Test Web Page][TEST_IMG]

Now we change the SELinux file type of `index.html` to `var_t` with the `chcon` command and then verify the type with `stat`
```console
# chcon --type var_t /var/www/html/index.html
# stat -c "%a %A %U %G %C" /var/www/html/index.html
644 -rw-r--r-- root root unconfined_u:object_r:var_t:s0
```
With these modified file properties  the browser is not able to access the test page anymore and a `denied` entry is added to `/var/log/audit/audit.log`
```console
type=AVC msg=audit(1581768103.178:480): avc:  denied  { getattr } for pid=4810 comm="httpd" path="/var/www/html/index.html" dev="dm-0" ino=167714 scontext=system_u:system_r:httpd_t:s0 tcontext=unconfined_u:object_r:var_t:s0 tclass=file permissive=0
```
With the command
```console
# restorecon -v /var/www/html/index.html
```
the original SELinux security context is restored and the test page should become available again.

[TEST_IMG]: Test_Web_Page_600.png

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
