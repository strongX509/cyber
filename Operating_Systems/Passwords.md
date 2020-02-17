# Passwords

## Table of Contents
1. [Unix Password Format](#section1)
2. [Password Change](#section2)

Python exercise: &nbsp; [PY1](#python1) 

## Unix Password Format <a name="section1"></a>

Unix password hashes are kept in the `/etc/shadow` file which can be read by `root`only in order to prevent mortal users from snatching a copy and trying to crack the passwords in an offline attack.
```console
 ls -l /etc/shadow
-rw-r----- 1 root shadow 1605 Dez 26  2018 /etc/shadow
```
The entries in the `shadow` file have the following form:
```console
hacker:$6$UB3QP5iUCeAEu89V$BSzAdlYcCxPyGpJcu/ce5aprxwP1XtreRLB69KCeanv00YFxaOY6Py05zWOLE6kDPGdINnMvpt.0Mzj4IWmmj.:18302:0:99999:7:::
```
which can be split into the following fields
```console
<uid>:$<algorithm>$<salt>$<hash>:<password creation and validity fields>...
```
`uid`:  user identity

`algorithm`: 

 * `1`  MD5 (deprecated)
 * `5` SHA-256 (5000 iterations)
 * `6` SHA-512 (5000 iterations)

`salt`:  Maximum 16 characters (12 bytes) resulting in  `2^96` different hashes for a given password

`hash`:

 * MD5:  22 characters  (16 bytes)
 * SHA-256:   43 characters  (32 bytes) 
 * SHA-512:   86 characters  (64 bytes)

The random binary  `salt` and `hash`values are encoded with the following 64 symbol printable alphabet: `[./],[0..9],[A..Z],[a..z]`. Unlike the  *Base64* encoding there is no `=` padding.

**Python 1**: <a name="python1"></a>The SHA-512 password hash can be computed as follows:

```python
>>> import crypt
>>> passwd='compass'
>>> salt='UB3QP5iUCeAEu89V'
>>> crypt.crypt(passwd, '$6$'+salt+'$')
'$6$UB3QP5iUCeAEu89V$BSzAdlYcCxPyGpJcu/ce5aprxwP1XtreRLB69KCeanv00YFxaOY6Py05zWOLE6kDPGdINnMvpt.0Mzj4IWmmj.'
```
Alternatively you could also compute a SHA-256 password hash
```python
>>> crypt.crypt(passwd, '$5$'+salt+'$')
'$5$UB3QP5iUCeAEu89V$enxMVecmqOFNxGUKenASsFgY7/QU7SybNsmQeh4rSK8'
```
or a deprecated MD5 password hash
```python
>>> crypt.crypt(passwd, '$1$'+salt+'$')
'$1$UB3QP5iU$VFLwRy0Uk7nvh52dkaJ061'
```
where only the first 8 characters (6 bytes) of the salt are used.

## Password Change <a name="section2"></a>

We change the password of user `hacker` from `compass` to `cyber-rs20` and then back  to `compass` again and note that due to the random `salt` value a different `hash` value results for the same password.
```console
# passwd hacker
New password: cyber-rs20
Retype new password: cyber-rs20 
passwd: password updated successfully
# tail /etc/shadow
hacker:$6$EPECXrgJeqa1izE5$iz1GH5D8/olkfZTcCe8s.dfCFxMfLM1O5UmLV1nhCTh7TuQ.zVZJ8YHBwSgAD1QA8ECcocSRzLp2Ll7CeT0r3/:18309:0:99999:7:::
```
Revert to the original password:
```console
# passwd hacker
New password: compass
Retype new password: compass
passwd: password updated successfully
# tail /etc/shadow
hacker:$6$5efJTW0jPiHe3kSs$q/c9HdTT/IT6T.HkDZbebk001yPgYCIPTvmMv6onGSDFqrXCz7p/In.mgcxwQSLRwGw31fF97IBFC3IbPu9qu/:18309:0:99999:7:::
```

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
