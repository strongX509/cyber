# Platform Security

## Table of Contents
1. [IBM TPM 2.0 Simulator](#section1)
2. [File Measurements using PCR Registers](#section2)
3. [Endorsement Keys](#section3)
4. [Attestation Keys](#section4)

## IBM TPM 2.0 Simulator<a name="section1">

We run the [IBM TPM 2.0][IBM_TPM2] simulator in a docker container and access it with the [tpm2-tools][TPM2_TOOLS] via the [tpm2-tss][TPM2_TSS] software stack . 

[IBM_TPM2]:   https://sourceforge.net/projects/ibmswtpm2/
[TPM2_TSS]:   https://github.com/tpm2-software/tpm2-tss
[TPM2_TOOLS]: https://github.com/tpm2-software/tpm2-tools

We clone the TPM `docker-compose` environment which automatically installs the `strongx509/tpm` docker image and brings the `tpm-server` container up:
```console
$ git clone https://github.com/strongX509/docker.git
$ cd docker/tpm
$ docker-compose up
Creating tpm-server ... done
Attaching to tpm-server
```
In an additional console window we open a `bash` shell to start the IBM TPM 2.0 simulator in the `tpm-server` container
```console
$ docker exec -ti tpm-server /bin/bash
# /usr/bin/tpm_server &
LIBRARY_COMPATIBILITY_CHECK is ON
Manufacturing NV state...
Size of OBJECT = 2600
Size of components in TPMT_SENSITIVE = 1096
    TPMI_ALG_PUBLIC                 2
    TPM2B_AUTH                      66
    TPM2B_DIGEST                    66
    TPMU_SENSITIVE_COMPOSITE        962
Starting ACT thread...
TPM command server listening on port 2321
Platform server listening on port 2322
```
After `tpm_server` has started in the background, its internal state must be initialized first
```console
# tpm2_startup -c 
Command IPv4 client accepted
Platform IPv4 client accepted
Platform server listening on port 2322
TPM command server listening on port 2321
```
Since the `tpm_server` daemon generates the four output lines above every time a TPM command is received it might better to suppress the debug output when starting the daemon
```console
# /usr/bin/tpm_server > /dev/null &
# tpm2_startup -c
```
Now the TPM 2.0 simulator is ready to use.

## File Measurements using PCR Registers <a name="section2">

First we list the contents of the `sha256` PCR bank
```console
# tpm2_pcrlist -g sha256
sha256 :
  0  : 0000000000000000000000000000000000000000000000000000000000000000
  1  : 0000000000000000000000000000000000000000000000000000000000000000
  2  : 0000000000000000000000000000000000000000000000000000000000000000
  3  : 0000000000000000000000000000000000000000000000000000000000000000
  4  : 0000000000000000000000000000000000000000000000000000000000000000
  5  : 0000000000000000000000000000000000000000000000000000000000000000
  6  : 0000000000000000000000000000000000000000000000000000000000000000
  7  : 0000000000000000000000000000000000000000000000000000000000000000
  8  : 0000000000000000000000000000000000000000000000000000000000000000
  9  : 0000000000000000000000000000000000000000000000000000000000000000
  10 : 0000000000000000000000000000000000000000000000000000000000000000
  11 : 0000000000000000000000000000000000000000000000000000000000000000
  12 : 0000000000000000000000000000000000000000000000000000000000000000
  13 : 0000000000000000000000000000000000000000000000000000000000000000
  14 : 0000000000000000000000000000000000000000000000000000000000000000
  15 : 0000000000000000000000000000000000000000000000000000000000000000
  16 : 0000000000000000000000000000000000000000000000000000000000000000
  17 : ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
  18 : ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
  19 : ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
  20 : ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
  21 : ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
  22 : ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
  23 : 0000000000000000000000000000000000000000000000000000000000000000
```

For legacy applications also a `sha1` PCR bank is available
```console
# tpm2_pcrlist -g sha1
sha1 :
  0  : 0000000000000000000000000000000000000000
  1  : 0000000000000000000000000000000000000000
  2  : 0000000000000000000000000000000000000000
  3  : 0000000000000000000000000000000000000000
  4  : 0000000000000000000000000000000000000000
  5  : 0000000000000000000000000000000000000000
  6  : 0000000000000000000000000000000000000000
  7  : 0000000000000000000000000000000000000000
  8  : 0000000000000000000000000000000000000000
  9  : 0000000000000000000000000000000000000000
  10 : 0000000000000000000000000000000000000000
  11 : 0000000000000000000000000000000000000000
  12 : 0000000000000000000000000000000000000000
  13 : 0000000000000000000000000000000000000000
  14 : 0000000000000000000000000000000000000000
  15 : 0000000000000000000000000000000000000000
  16 : 0000000000000000000000000000000000000000
  17 : ffffffffffffffffffffffffffffffffffffffff
  18 : ffffffffffffffffffffffffffffffffffffffff
  19 : ffffffffffffffffffffffffffffffffffffffff
  20 : ffffffffffffffffffffffffffffffffffffffff
  21 : ffffffffffffffffffffffffffffffffffffffff
  22 : ffffffffffffffffffffffffffffffffffffffff
  23 : 0000000000000000000000000000000000000000
```

The PCR banks implemented by the TPM simulator can be listed with
```console
# tpm2_pcrlist -s            
Supported Bank/Algorithm: sha1(0x0004) sha256(0x000b) sha384(0x000c) sha512(0x000d)
```
Currently available TPM devices usually support `sha1` and `sha256`,  only.

Now we determine the `sha1` and `sha256` hashes of the `bin/bash`file
```console
# sha1sum /bin/bash
59fea2c26edbbab48daaf73e7cd16ebc47475e83  /bin/bash
# sha256sum /bin/bash
da85596376bf384c14525c50ca010e9ab96952cb811b4abe188c9ef1b75bff9a  /bin/bash
```

and  extend these values into PCR 10 of both the `sha256` and `sha1` PCR banks
```console
# tpm2_pcrextend 10:sha256=da85596376bf384c14525c50ca010e9ab96952cb811b4abe188c9ef1b75bff9a,sha1=59fea2c26edbbab48daaf73e7cd16ebc47475e83
```

We are interested in the extended state of the PCR 10 registers only
```console
# tpm2_pcrlist -L sha1:10+sha256:10
sha1 :
  10 : fd6b92a42540e38fbc21c3448406002dc71b186a
sha256 :
  10 : c73667c3c7ab5c2d9882aac077fcc7149e1564ce6bcfe9df83ba2132d3cd4523
```

Now we compute the `sha256` hash of the `/bin/cp/` file
```console
# sha256sum /bin/cp  
007d90bd141b2cdbd3cbc5e5f472b3144d122440c73a46d58ee61df2dd71e56e  /bin/cp
```
and extend it into registers 10 and 16 of the `sha256` PCR bank
```console
# tpm2_pcrextend 10:sha256=007d90bd141b2cdbd3cbc5e5f472b3144d122440c73a46d58ee61df2dd71e56e 16:sha256=007d90bd141b2cdbd3cbc5e5f472b3144d122440c73a46d58ee61df2dd71e56e
```

The following registers have now been extended
```console
# tpm2_pcrlist -L sha1:10+sha256:10,16
sha1 :
  10 : fd6b92a42540e38fbc21c3448406002dc71b186a
sha256 :
  10 : 34835aed90fc6ce367b6ce9773e266e27e12e624533a929316266794c0e50745
  16 : f34b9a1346400ecd856c72d58937b13151b7fc9f3179134411ac00cd682c0de3
```

We try to reset register 10 in all PCR banks
```console
# tpm2_pcrreset 10
ERROR: Could not reset PCR index: 10
ERROR: Unable to run tpm2_pcrreset
```
Fortunately this is not possible. Otherwise malicious malware could reset the PCR registers and extend them with fake measurements. On a PC motherboard only a cold reboot resets the PCR values.

An exception is PCR 16 that is intended for test purposes and thus can be reset
```console
# tpm2_pcrreset 16
# tpm2_pcrlist -L sha1:10+sha256:10,16
sha1 :
  10 : fd6b92a42540e38fbc21c3448406002dc71b186a
sha256 :
  10 : 34835aed90fc6ce367b6ce9773e266e27e12e624533a929316266794c0e50745
  16 : 0000000000000000000000000000000000000000000000000000000000000000
```
We see that PCR 16 has been reset to all zeroes.

## Endorsement Keys<a name="section3">

Each TPM has unique non-erasable *Endorsement Seed*. From this seed an *Endorsement Key* `EK` of any chosen key type can be derived in a deterministic way.

### RSA Endorsement Key

First we derive an `RSA` `EK` and store it in non-volatile RAM under the handle `0x81010001`
```console
# tpm2_getpubek -H 0x81010001 -g rsa -f ek_rsa.pub
# tpm2_listpersistent
persistent-handle[0]:0x81010001 key-alg:rsa hash-alg:sha256 object-attr:fixedtpm|fixedparent|sensitivedataorigin|adminwithpolicy|restricted|decrypt
```
With the help of its `tpm`plugin the strongswan `pki` tool can read the unique keyid of a private key stored in a TPM based on its handle
```console
# pki --print --type priv --keyid 0x81010001
TPM 2.0 via TSS2 v2 available
signature algorithm is NULL with ERROR hash
  privkey:   RSA 2048 bits
  keyid:     78:dd:f7:00:60:0c:19:62:3d:d8:66:8e:8e:0b:e6:33:99:c3:fb:c6
  subjkey:   ce:27:dc:4d:95:f9:d7:c0:32:6c:88:f7:b5:55:89:30:e5:ed:5a:2b
```
Since no signature algorithm is defined we notice that an `EK`is a decryption key and thus cannot be used for signatures.

In order for `pki` to connect correctly to the TPM simulator the `mssim` driver must be specified explicitly in `/etc/strongswan.conf`
```console
# cat /etc/strongswan.conf
pki {
   plugins {
      tpm {
         tcti {
            name = mssim
         }
      }
   }
}
```

### Public Key Extraction

The `tpm2_getpubek` command returns the public key in a proprietary TPM 2.0 format and stores it in the file `ek_rsa.pub`.  Fortunately the `pki` tool can extract the public key in standard `PKCS#1` format
```console
 pki --pub --type priv --keyid 0x81010001 --outform pem > ek_rsa.pem
```
This extracted public key can either be analyzed with `pki`
```console
# pki --print --type pub --in ek_rsa.pem --debug 3
  file content is not binary ASN.1
  -----BEGIN PUBLIC KEY-----
  -----END PUBLIC KEY-----
L0 - subjectPublicKeyInfo:
L1 - algorithm:
L2 - algorithmIdentifier:
L3 - algorithm:
  'rsaEncryption'
L1 - subjectPublicKey:
=> 271 bytes @ 0x55bfe22c1fa7
   0: 00 30 82 01 0A 02 82 01 01 00 9B 8F EC 92 F5 5A  .0.............Z
  16: 57 98 92 9C FB 34 E5 89 2C 6A 65 50 75 E1 A5 41  W....4..,jePu..A
  32: FE 96 8C 3E 98 D9 CE 47 79 2F 25 7A ED 80 EF E6  ...>...Gy/%z....
  48: 80 7D D2 8B 54 0E 61 0C 27 19 00 FB 7F 6E EC 19  .}..T.a.'....n..
  64: FF 75 94 5B 1C 8B 6D F0 A6 C3 D9 70 25 D0 80 88  .u.[..m....p%...
  80: 19 71 F2 77 93 C7 12 90 02 31 30 51 41 88 94 AD  .q.w.....10QA...
  96: BE 41 B5 E1 A8 34 BF B9 60 C1 F7 C7 21 2A 19 94  .A...4..`...!*..
 112: 35 FC CC F4 D7 69 06 DE 4B 15 B0 54 4D B6 C3 A2  5....i..K..TM...
 128: 27 73 62 FD E2 70 27 3C DF F2 6D A7 86 24 49 2F  'sb..p'<..m..$I/
 144: EC 36 77 84 DB A2 72 AC 38 71 53 4F 41 69 C4 7C  .6w...r.8qSOAi.|
 160: F1 7A D7 16 68 08 3C DA 13 33 6B 79 FA EB B3 8A  .z..h.<..3ky....
 176: 04 CE 6A A5 FF 38 EC 03 DA 5D 36 DB AC 57 43 91  ..j..8...]6..WC.
 192: 61 1D 20 F5 F5 33 67 B1 2C 5E C4 69 9A FF 66 17  a. ..3g.,^.i..f.
 208: 5E F2 C9 4B 6A 6B DB 87 8B F2 83 D0 72 62 0C B5  ^..Kjk......rb..
 224: 4D 01 A5 5D 0C 5B 12 FE 83 B0 34 D6 B4 FD 52 9E  M..].[....4...R.
 240: 04 AF 82 A7 83 94 7E 70 1C A7 4E 0E EC A7 5D 6D  ......~p..N...]m
 256: CF A9 0F 3C 13 5B 01 4E E5 F5 02 03 01 00 01     ...<.[.N.......
-- > --
L0 - RSAPublicKey:
=> 270 bytes @ 0x55bfe22c1fa8
   0: 30 82 01 0A 02 82 01 01 00 9B 8F EC 92 F5 5A 57  0.............ZW
  16: 98 92 9C FB 34 E5 89 2C 6A 65 50 75 E1 A5 41 FE  ....4..,jePu..A.
  32: 96 8C 3E 98 D9 CE 47 79 2F 25 7A ED 80 EF E6 80  ..>...Gy/%z.....
  48: 7D D2 8B 54 0E 61 0C 27 19 00 FB 7F 6E EC 19 FF  }..T.a.'....n...
  64: 75 94 5B 1C 8B 6D F0 A6 C3 D9 70 25 D0 80 88 19  u.[..m....p%....
  80: 71 F2 77 93 C7 12 90 02 31 30 51 41 88 94 AD BE  q.w.....10QA....
  96: 41 B5 E1 A8 34 BF B9 60 C1 F7 C7 21 2A 19 94 35  A...4..`...!*..5
 112: FC CC F4 D7 69 06 DE 4B 15 B0 54 4D B6 C3 A2 27  ....i..K..TM...'
 128: 73 62 FD E2 70 27 3C DF F2 6D A7 86 24 49 2F EC  sb..p'<..m..$I/.
 144: 36 77 84 DB A2 72 AC 38 71 53 4F 41 69 C4 7C F1  6w...r.8qSOAi.|.
 160: 7A D7 16 68 08 3C DA 13 33 6B 79 FA EB B3 8A 04  z..h.<..3ky.....
 176: CE 6A A5 FF 38 EC 03 DA 5D 36 DB AC 57 43 91 61  .j..8...]6..WC.a
 192: 1D 20 F5 F5 33 67 B1 2C 5E C4 69 9A FF 66 17 5E  . ..3g.,^.i..f.^
 208: F2 C9 4B 6A 6B DB 87 8B F2 83 D0 72 62 0C B5 4D  ..Kjk......rb..M
 224: 01 A5 5D 0C 5B 12 FE 83 B0 34 D6 B4 FD 52 9E 04  ..].[....4...R..
 240: AF 82 A7 83 94 7E 70 1C A7 4E 0E EC A7 5D 6D CF  .....~p..N...]m.
 256: A9 0F 3C 13 5B 01 4E E5 F5 02 03 01 00 01        ..<.[.N.......
L1 - modulus:
=> 257 bytes @ 0x55bfe22c1fb0
   0: 00 9B 8F EC 92 F5 5A 57 98 92 9C FB 34 E5 89 2C  ......ZW....4..,
  16: 6A 65 50 75 E1 A5 41 FE 96 8C 3E 98 D9 CE 47 79  jePu..A...>...Gy
  32: 2F 25 7A ED 80 EF E6 80 7D D2 8B 54 0E 61 0C 27  /%z.....}..T.a.'
  48: 19 00 FB 7F 6E EC 19 FF 75 94 5B 1C 8B 6D F0 A6  ....n...u.[..m..
  64: C3 D9 70 25 D0 80 88 19 71 F2 77 93 C7 12 90 02  ..p%....q.w.....
  80: 31 30 51 41 88 94 AD BE 41 B5 E1 A8 34 BF B9 60  10QA....A...4..`
  96: C1 F7 C7 21 2A 19 94 35 FC CC F4 D7 69 06 DE 4B  ...!*..5....i..K
 112: 15 B0 54 4D B6 C3 A2 27 73 62 FD E2 70 27 3C DF  ..TM...'sb..p'<.
 128: F2 6D A7 86 24 49 2F EC 36 77 84 DB A2 72 AC 38  .m..$I/.6w...r.8
 144: 71 53 4F 41 69 C4 7C F1 7A D7 16 68 08 3C DA 13  qSOAi.|.z..h.<..
 160: 33 6B 79 FA EB B3 8A 04 CE 6A A5 FF 38 EC 03 DA  3ky......j..8...
 176: 5D 36 DB AC 57 43 91 61 1D 20 F5 F5 33 67 B1 2C  ]6..WC.a. ..3g.,
 192: 5E C4 69 9A FF 66 17 5E F2 C9 4B 6A 6B DB 87 8B  ^.i..f.^..Kjk...
 208: F2 83 D0 72 62 0C B5 4D 01 A5 5D 0C 5B 12 FE 83  ...rb..M..].[...
 224: B0 34 D6 B4 FD 52 9E 04 AF 82 A7 83 94 7E 70 1C  .4...R.......~p.
 240: A7 4E 0E EC A7 5D 6D CF A9 0F 3C 13 5B 01 4E E5  .N...]m...<.[.N.
 256: F5                                               .
L1 - publicExponent:
=> 3 bytes @ 0x55bfe22c20b3
   0: 01 00 01                                         ...
-- < --
  pubkey:    RSA 2048 bits
  keyid:     78:dd:f7:00:60:0c:19:62:3d:d8:66:8e:8e:0b:e6:33:99:c3:fb:c6
  subjkey:   ce:27:dc:4d:95:f9:d7:c0:32:6c:88:f7:b5:55:89:30:e5:ed:5a:2b
```
or with `openssl`
```console
# openssl rsa -in ek_rsa.pem -pubin -noout -text
RSA Public-Key: (2048 bit)
Modulus:
    00:9b:8f:ec:92:f5:5a:57:98:92:9c:fb:34:e5:89:
    2c:6a:65:50:75:e1:a5:41:fe:96:8c:3e:98:d9:ce:
    47:79:2f:25:7a:ed:80:ef:e6:80:7d:d2:8b:54:0e:
    61:0c:27:19:00:fb:7f:6e:ec:19:ff:75:94:5b:1c:
    8b:6d:f0:a6:c3:d9:70:25:d0:80:88:19:71:f2:77:
    93:c7:12:90:02:31:30:51:41:88:94:ad:be:41:b5:
    e1:a8:34:bf:b9:60:c1:f7:c7:21:2a:19:94:35:fc:
    cc:f4:d7:69:06:de:4b:15:b0:54:4d:b6:c3:a2:27:
    73:62:fd:e2:70:27:3c:df:f2:6d:a7:86:24:49:2f:
    ec:36:77:84:db:a2:72:ac:38:71:53:4f:41:69:c4:
    7c:f1:7a:d7:16:68:08:3c:da:13:33:6b:79:fa:eb:
    b3:8a:04:ce:6a:a5:ff:38:ec:03:da:5d:36:db:ac:
    57:43:91:61:1d:20:f5:f5:33:67:b1:2c:5e:c4:69:
    9a:ff:66:17:5e:f2:c9:4b:6a:6b:db:87:8b:f2:83:
    d0:72:62:0c:b5:4d:01:a5:5d:0c:5b:12:fe:83:b0:
    34:d6:b4:fd:52:9e:04:af:82:a7:83:94:7e:70:1c:
    a7:4e:0e:ec:a7:5d:6d:cf:a9:0f:3c:13:5b:01:4e:
    e5:f5
Exponent: 65537 (0x10001)
```

### ECC Endorsement Key

As an alternative we can derive an `ECC` `EK` based on *Elliptic Curve Cryptography* from the same secret *Endorsement Seed* and store it persistenly under the handle `0x81010002`
```console
# tpm2_getpubek -H 0x81010002 -g ecc -f ek_ecc.pub
# tpm2_listpersistent
persistent-handle[0]:0x81010001 key-alg:rsa hash-alg:sha256 object-attr:fixedtpm|fixedparent|sensitivedataorigin|adminwithpolicy|restricted|decrypt
persistent-handle[1]:0x81010002 key-alg:ecc hash-alg:sha256 object-attr:fixedtpm|fixedparent|sensitivedataorigin|adminwithpolicy|restricted|decrypt
```

The `pki` tool identifies the second `EK` as a 256 bit ECC decryption key
```console
# pki --print --type priv --keyid 0x81010002
TPM 2.0 via TSS2 v2 available
signature algorithm is NULL with ERROR hash
  privkey:   ECDSA 256 bits
  keyid:     0d:e5:f5:93:3a:26:d5:fc:e7:ab:09:4e:85:34:4a:44:fa:e2:5b:f6
  subjkey:   46:44:2f:0e:13:2c:ce:56:32:2c:ec:c5:06:e1:07:10:15:f4:62:b7
```
### Key Removal from Persistent Memory

If we run out of non-volatile memory we can delete an *Endorsement Key* at any time
```console
# tpm2_evictcontrol -A o -H 0x81010001
persistentHandle: 0x81010001
# tpm2_listpersistent
persistent-handle[0]:0x81010002 key-alg:ecc hash-alg:sha256 object-attr:fixedtpm|fixedparent|sensitivedataorigin|adminwithpolicy|restricted|decrypt
```
and can derive it again when the need occurs
```console
# tpm2_getpubek -H 0x81010001 -g rsa -f ek_rsa.pub
# pki --print --type priv --keyid 0x81010001
TPM 2.0 via TSS2 v2 available
signature algorithm is NULL with ERROR hash
  privkey:   RSA 2048 bits
  keyid:     78:dd:f7:00:60:0c:19:62:3d:d8:66:8e:8e:0b:e6:33:99:c3:fb:c6
  subjkey:   ce:27:dc:4d:95:f9:d7:c0:32:6c:88:f7:b5:55:89:30:e5:ed:5a:2bg
```
As can be easily verified the same `EK` as before results.

## Attestation Keys<a name="section4">

Under the *Endorsement Hierarchy* we can create as many random signature keys as we need for our applications. Microsoft calls this the  *Virtual Smartcard* functionality of a TPM.

### Built-In True Random Generator

In order to generate RSA or ECC keys the TPM needs a built-in *True Random Generator*.
```console
# tpm2_getrandom 10
0x9E 0x7C 0x42 0xD3 0x0E 0x43 0x86 0x81 0xC2 0x52
```
Up to 48 random bytes can be extracted in single call by `tpm2_getrandom`

### RSA Attestation Key

We generate a random RSA *Attestation Key* `AK` protected by the RSA *Endorsement Key* `EK` with handle `0x81010001`and store it under the handle `0x81010003`. The use of the  *Attestation Key* is protected by the password `123456`.
```console
# tpm2_getpubak -E 0x81010001 -g rsa -D sha256 -s rsassa -k 0x81010003 -P 123456 -f ak_rsa.pub -n ak_rsa.name
loaded-key:
  handle: 80000001
  name: 000bdb87fc1280e811816e539fb76bc9c0e2052d7e9e9f7c67130071268d996d4d1d
```
The `pki` command shows that a RSA signature key has been generated
```console
# pki --print --type priv --keyid 0x81010003
TPM 2.0 via TSS2 v2 available
signature algorithm is RSASSA with SHA256 hash
  privkey:   RSA 2048 bits
  keyid:     14:38:45:33:09:3b:d2:6b:40:27:36:1f:e1:36:3f:5e:9d:98:3b:24
  subjkey:   82:50:10:26:e2:05:7b:b0:ae:74:7e:0b:94:5e:9b:c7:89:45:89:a4
```
If a client wants to authenticate itself using this RSA key then it needs an X.509 certificate issued and signed by a trusted *Certification Authority* `CA`. Therefore a standard `PKCS#10` *Certificate Request* is generated by the `pki`tool signed by the private `AK` stored in the TPM.
```console
# pki --req --keyid 0x81010003 --outform pem --dn "C=CH, O=Cyber, OU=user@strongswan.org" --san user@strongswan.org > ak_rsa_Req.pem
TPM 2.0 via TSS2 v2 available
signature algorithm is RSASSA with SHA256 hash
Smartcard PIN:
123456
```
For the TPM signature the password is requested.

We visualize the `PKCS#10` *Certificate Request* using `openssl` before sending the request to the *Certification Authority*
```console
# openssl req -in ak_rsa_Req.pem -noout -text
Certificate Request:
    Data:
        Version: 1 (0x0)
        Subject: C = CH, O = Cyber, OU = user@strongswan.org
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
                RSA Public-Key: (2048 bit)
                Modulus:
                    00:d1:14:e3:c4:82:40:4f:c4:50:40:87:9e:00:e6:
                    00:be:91:6a:94:34:0a:d1:45:b1:49:0d:0b:d9:8b:
                    77:f1:e2:81:d9:67:3d:8c:92:08:a9:64:c7:56:d5:
                    c4:1e:b8:e2:bd:7f:4a:0e:2e:c9:0b:f8:f4:4c:df:
                    7a:8d:47:d1:8d:df:2b:ae:55:28:8b:5d:b2:bd:aa:
                    97:b9:82:c7:d1:a2:35:d3:de:c4:f3:be:f7:7e:c7:
                    ca:85:6d:29:d1:66:7b:ff:fe:32:82:a7:70:06:cf:
                    fa:46:cd:9b:55:0f:cf:28:bc:bd:c9:b9:3a:0c:d6:
                    eb:43:b9:cc:87:a1:95:b0:da:b8:a8:44:50:c5:72:
                    47:bd:2c:9b:b5:96:39:3b:9d:59:78:f6:e1:1c:39:
                    d6:0d:ff:4e:68:c7:f6:86:7e:f2:82:30:24:eb:d8:
                    2c:f9:52:85:bf:69:89:47:4d:55:2e:74:25:66:4d:
                    76:ed:14:39:25:a4:e5:cd:81:ee:f9:c1:36:f9:d9:
                    ab:c9:82:51:05:45:84:fc:0b:4d:5f:4d:33:24:67:
                    a2:cd:86:15:2a:34:34:bd:82:21:c6:ca:a6:60:ae:
                    4b:ce:a8:0e:05:15:1b:5a:5f:97:1a:a5:75:30:b1:
                    89:fe:06:f5:3d:58:c6:93:3c:97:0e:24:33:75:4e:
                    fd:e5
                Exponent: 65537 (0x10001)
        Attributes:
        Requested Extensions:
            X509v3 Subject Alternative Name: 
                email:user@strongswan.org
    Signature Algorithm: sha256WithRSAEncryption
         69:1d:44:be:ae:c6:b5:72:44:d1:ad:2e:05:44:0d:a7:dc:b9:
         1a:ec:e1:f8:28:07:66:3a:0f:e1:c3:30:60:a1:65:fa:3a:09:
         19:a2:fc:ff:92:70:57:1d:d5:0d:24:28:90:4a:8e:13:57:00:
         f8:27:82:77:34:b3:ab:f8:07:03:a5:87:62:5c:60:d3:a3:9e:
         19:c5:e3:b3:4e:21:16:ad:46:b1:10:e7:47:73:04:e5:08:04:
         0b:1e:b2:9e:3e:11:e3:8e:56:30:65:12:fa:04:6d:be:f5:61:
         3d:61:22:c8:78:8a:1a:5b:f0:d5:ff:9e:02:4e:d0:91:f0:99:
         8c:d6:f2:b3:5d:81:73:1b:49:73:90:aa:7e:45:38:22:a2:bd:
         be:84:9b:f3:c7:4d:13:25:10:c9:4f:84:52:90:74:94:4c:75:
         b5:ef:e3:2e:3b:f4:c6:ad:e6:9b:39:54:93:a3:cf:3c:8b:74:
         da:bd:f0:81:ad:22:f5:bf:8b:3a:8b:97:20:2d:be:56:6a:16:
         71:f4:29:1b:2b:45:4a:36:e2:8a:a4:92:fc:1f:05:ee:13:2d:
         a0:5c:89:82:ba:1a:bf:36:83:ac:6a:59:9d:33:8d:24:e6:76:
         25:1d:2e:a0:62:cc:75:d5:91:ec:2a:c6:33:06:c6:f5:3b:ed:
         fa:41:eb:ea
```

### RSA Certification Authority

The strongSwan `pki` tool can be used to generate a simple *Certification Authority* `CA`.  First a  private `CA` key is generated
```console
# pki --gen --type rsa --size 2048 --outform pem > My_RSA_CaKey.pem
```
and used to construct a self-signed `CA` certificate
```console
# pki --self --ca --type rsa --in My_RSA_CaKey.pem --dn "C=CH, O=Cyber, CN=My RSA CA" --lifetime 3652 --outform pem > My_RSA_CaCert.pem
```
that looks as follows
```console
# pki --print --type x509 --in My_RSA_CaCert.pem 
  subject:  "C=CH, O=Cyber, CN=My RSA CA"
  issuer:   "C=CH, O=Cyber, CN=My RSA CA"
  validity:  not before Aug 23 08:39:18 2020, ok
             not after  Aug 23 08:39:18 2030, ok (expires in 3651 days)
  serial:    30:cc:3e:3b:09:f6:34:41
  flags:     CA CRLSign self-signed 
  subjkeyId: 93:4f:79:df:82:e9:7a:76:5c:97:6b:68:b4:03:2d:29:fe:1e:37:26
  pubkey:    RSA 2048 bits
  keyid:     86:71:cb:af:f1:6a:4a:d7:21:9c:96:36:be:70:82:4d:27:17:1e:3d
  subjkey:   93:4f:79:df:82:e9:7a:76:5c:97:6b:68:b4:03:2d:29:fe:1e:37:26
```
Now the user certificate can be issued based on the `PKCS#10` *Certificate Request*
```console
# pki --issue --cacert My_RSA_CaCert.pem --cakey My_RSA_CaKey.pem --in ak_rsa_Req.pem --type pkcs10 --flag clientAuth --lifetime 1826 > ak_rsa_Cert.der
```
A quick look at the X.509 certificate before it is delivered to the user
```console
# pki --print --type x509 --in ak_rsa_Cert.der 
  subject:  "C=CH, O=Cyber, OU=user@strongswan.org"
  issuer:   "C=CH, O=Cyber, CN=My RSA CA"
  validity:  not before Aug 23 09:18:23 2020, ok
             not after  Aug 23 09:18:23 2025, ok (expires in 1825 days)
  serial:    2f:f1:af:26:26:5d:cb:cb
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: 93:4f:79:df:82:e9:7a:76:5c:97:6b:68:b4:03:2d:29:fe:1e:37:26
  subjkeyId: 82:50:10:26:e2:05:7b:b0:ae:74:7e:0b:94:5e:9b:c7:89:45:89:a4
  pubkey:    RSA 2048 bits
  keyid:     14:38:45:33:09:3b:d2:6b:40:27:36:1f:e1:36:3f:5e:9d:98:3b:24
  subjkey:   82:50:10:26:e2:05:7b:b0:ae:74:7e:0b:94:5e:9b:c7:89:45:89:a4
```

### RSA Attestation Certificate

In order to protect `AK` certificate received from the `CA` from accidental deletion from the system harddisk we want to store it in the *Non-Volatile* `NV` memory of the TPM. In order to allocate the required amount of  `NV` memory we determine the exact size of the binary-encoded X.509 certificate
```console
ls -l ak_rsa_Cert.der 
-rw-r--r-- 1 root root 842 Aug 23 09:18 ak_rsa_Cert.der
```
and then allocate 842 bytes of `NV` memory under  the handle `0x01800003`
```console
# tpm2_nvdefine -x 0x01800003 -a 0x40000001 -s 842 -t 0x2000A
```
Now the certificate can be written into the pre-allocated memory slot
```console
# tpm2_nvwrite -x 0x01800003 -a 0x40000001 ak_rsa_Cert.der
```
The following command shows all objects stored in `NV` memory
```console
# tpm2_nvlist 
0x1800003:
  hash algorithm:
    friendly: sha256
    value: 0xB
  attributes:
    friendly: ownerwrite|policywrite|ownerread|written
    value: 0xA000220
  size: 842
```
The `pki` tool can display X.509 certificates stored in the TPM using the object handle
```console
# pki --print --type x509 --keyid 0x01800003
TPM 2.0 via TSS2 v2 available
loaded certificate from TPM NV index 0x01800003
  subject:  "C=CH, O=Cyber, OU=user@strongswan.org"
  issuer:   "C=CH, O=Cyber, CN=My RSA CA"
  validity:  not before Aug 23 09:18:23 2020, ok
             not after  Aug 23 09:18:23 2025, ok (expires in 1825 days)
  serial:    2f:f1:af:26:26:5d:cb:cb
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: 93:4f:79:df:82:e9:7a:76:5c:97:6b:68:b4:03:2d:29:fe:1e:37:26
  subjkeyId: 82:50:10:26:e2:05:7b:b0:ae:74:7e:0b:94:5e:9b:c7:89:45:89:a4
  pubkey:    RSA 2048 bits
  keyid:     14:38:45:33:09:3b:d2:6b:40:27:36:1f:e1:36:3f:5e:9d:98:3b:24
  subjkey:   82:50:10:26:e2:05:7b:b0:ae:74:7e:0b:94:5e:9b:c7:89:45:89:a4
```
A `strongSwan` IKEv2 client could use the `AK` private key and the `AK` certificate stored in  the TPM to authenticate itself in an IPsec connection.

### ECC Attestation Key

As an alternative we generate a random ECC *Attestation Key* `AK` protected by the ECC *Endorsement Key* `EK` with handle `0x81010002`and store it under the handle `0x81010004`.
```console
# tpm2_getpubak -E 0x81010002 -g ecc -D sha256 -s ecdsa -k 0x81010004 -f ak_ecc.pub -n ak_ecc.name
loaded-key:
  handle: 80000002
  name: 000b96419568a0a12d6e330c7254290300d78af91be0680ca50cef2f3b3f86d8b79d
```
The `pki` command shows that an ECDSA signature key has been generated
```console
# pki --print --type priv --keyid 0x81010004
TPM 2.0 via TSS2 v2 available
signature algorithm is ECDSA with SHA256 hash
  privkey:   ECDSA 256 bits
  keyid:     02:78:1a:ac:a1:cb:a3:af:6c:09:61:e6:82:09:de:c8:6c:ee:60:70
  subjkey:   69:f3:27:31:46:e0:6b:77:a4:b8:f7:79:0b:87:7e:2b:84:81:eb:0c
```
In order to obtain an X.509 certificate issued and signed by a trusted *Certification Authority* `CA` we again generate a `PKCS#10` *Certificate Request* signed by the private `AK` stored in the TPM.
```console
# pki --req --keyid 0x81010004 --outform pem --dn "C=CH, O=Cyber, OU=user@strongswan.org" --san user@strongswan.org > ak_ecc_Req.pem
TPM 2.0 via TSS2 v2 available
signature algorithm is ECDSA with SHA256 hash
Smartcard PIN: 
<return>
```
This is how the `PKCS#10` *Certificate Request* looks like as listed by `openssl
```console
# openssl req -in ak_ecc_Req.pem -noout -text
Certificate Request:
    Data:
        Version: 1 (0x0)
        Subject: C = CH, O = Cyber, OU = user@strongswan.org
        Subject Public Key Info:
            Public Key Algorithm: id-ecPublicKey
                Public-Key: (256 bit)
                pub:
                    04:95:60:c7:05:72:29:b5:a8:58:0d:a9:3a:ed:40:
                    98:6c:2a:ae:93:a1:4c:89:f1:27:71:85:c3:ac:87:
                    36:2f:6d:69:92:34:de:05:32:78:ec:1d:58:18:a6:
                    c5:d9:09:9f:01:56:d1:07:8f:86:6b:cd:6a:d2:08:
                    77:e2:02:c6:a8
                ASN1 OID: prime256v1
                NIST CURVE: P-256
        Attributes:
        Requested Extensions:
            X509v3 Subject Alternative Name: 
                email:user@strongswan.org
    Signature Algorithm: ecdsa-with-SHA256
         30:45:02:20:5b:d7:55:56:7c:b4:11:a8:70:7e:4a:ac:86:41:
         65:de:0b:76:2f:7e:e8:86:90:29:a3:df:ff:9e:bd:fb:40:2f:
         02:21:00:cb:0b:52:f6:a6:4d:60:b9:af:94:21:21:20:ad:a8:
         32:75:a7:4c:bb:da:55:be:8d:f6:35:43:f6:98:16:d5:29
```

### ECC Certification Authority

In order to issue ECC end entity certificates we create a *Certification Authority* of our own. We start be generating a private ECC key
```console
# pki --gen --type ecdsa --size 256 --outform pem > My_ECC_CaKey.pem
```
and use it to construct a self-signed `CA` certiificate
```console
# pki --self --ca --type ecdsa --in My_ECC_CaKey.pem --dn "C=CH, O=Cyber, CN=My ECC CA" --lifetime 3652 --outform pem > My_ECC_CaCert.pem
```
listed below
```console
 pki --print --type x509 --in My_ECC_CaCert.pem 
  subject:  "C=CH, O=Cyber, CN=My ECC CA"
  issuer:   "C=CH, O=Cyber, CN=My ECC CA"
  validity:  not before Aug 23 09:39:36 2020, ok
             not after  Aug 23 09:39:36 2030, ok (expires in 3651 days)
  serial:    0b:9c:94:26:39:f5:27:d0
  flags:     CA CRLSign self-signed 
  subjkeyId: b0:e0:5f:3c:e8:1c:eb:ef:97:6e:61:97:56:71:c8:15:40:46:b8:3f
  pubkey:    ECDSA 256 bits
  keyid:     30:a3:eb:00:92:c9:8b:8a:32:73:d8:91:f9:7a:85:2f:16:9a:5e:a7
  subjkey:   b0:e0:5f:3c:e8:1c:eb:ef:97:6e:61:97:56:71:c8:15:40:46:b8:3f
```
Using the `CA` private key and X.509 certificate we issue an ECC user certificate based on the `PKCS#10` *Certificate Request* 
```console
# pki --issue --cacert My_ECC_CaCert.pem --cakey My_ECC_CaKey.pem --in ak_ecc_Req.pem --type pkcs10 --flag clientAuth --lifetime 1826 > ak_ecc_Cert.der
```
The certificate is ready to be shipped to the user
```console
# pki --print --type x509 --in ak_ecc_Cert.der 
  subject:  "C=CH, O=Cyber, OU=user@strongswan.org"
  issuer:   "C=CH, O=Cyber, CN=My ECC CA"
  validity:  not before Aug 23 09:41:08 2020, ok
             not after  Aug 23 09:41:08 2025, ok (expires in 1825 days)
  serial:    6c:ff:67:08:36:70:fe:53
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: b0:e0:5f:3c:e8:1c:eb:ef:97:6e:61:97:56:71:c8:15:40:46:b8:3f
  subjkeyId: 69:f3:27:31:46:e0:6b:77:a4:b8:f7:79:0b:87:7e:2b:84:81:eb:0c
  pubkey:    ECDSA 256 bits
  keyid:     02:78:1a:ac:a1:cb:a3:af:6c:09:61:e6:82:09:de:c8:6c:ee:60:70
  subjkey:   69:f3:27:31:46:e0:6b:77:a4:b8:f7:79:0b:87:7e:2b:84:81:eb:0c
```

### ECC Attestation Certificate

We again determine the exact size of the binary-encoded X.509 certificate
```console
# ls -l ak_ecc_Cert.der 
-rw-r--r-- 1 root root 446 Aug 23 09:41 ak_ecc_Cert.der
```
Next we allocate 446 bytes needed by the X.509 certificate in the NV-RAM
```console
# tpm2_nvdefine -x 0x01800004 -a 0x40000001 -s 446 -t 0x2000A
```
The certificate can now be stored in the NV memory under the handle `0x01800004`
```console
# tpm2_nvwrite -x 0x01800004 -a 0x40000001 ak_ecc_Cert.der
```
There now two objects stored in NV-RAM
```console
# tpm2_nvlist 
0x1800003:
  hash algorithm:
    friendly: sha256
    value: 0xB
  attributes:
    friendly: ownerwrite|policywrite|ownerread|written
    value: 0xA000220
  size: 842

0x1800004:
  hash algorithm:
    friendly: sha256
    value: 0xB
  attributes:
    friendly: ownerwrite|policywrite|ownerread|written
    value: 0xA000220
  size: 446
```
The `pki` tool can read the X.509 certificate stored in the TPM
```console
# pki --print --type x509 --keyid 0x01800004
TPM 2.0 via TSS2 v2 available
loaded certificate from TPM NV index 0x01800004
  subject:  "C=CH, O=Cyber, OU=user@strongswan.org"
  issuer:   "C=CH, O=Cyber, CN=My ECC CA"
  validity:  not before Aug 23 09:41:08 2020, ok
             not after  Aug 23 09:41:08 2025, ok (expires in 1825 days)
  serial:    6c:ff:67:08:36:70:fe:53
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: b0:e0:5f:3c:e8:1c:eb:ef:97:6e:61:97:56:71:c8:15:40:46:b8:3f
  subjkeyId: 69:f3:27:31:46:e0:6b:77:a4:b8:f7:79:0b:87:7e:2b:84:81:eb:0c
  pubkey:    ECDSA 256 bits
  keyid:     02:78:1a:ac:a1:cb:a3:af:6c:09:61:e6:82:09:de:c8:6c:ee:60:70
  subjkey:   69:f3:27:31:46:e0:6b:77:a4:b8:f7:79:0b:87:7e:2b:84:81:eb:0c
```

### Object Removal from Non-Volatile Memory

If an object isn't used anymore it can be removed from NV-RAM with the following command
```console
# tpm2_nvrelease -x 0x01800003 -a 0x40000001
# tpm2_nvlist 
0x1800004:
  hash algorithm:
    friendly: sha256
    value: 0xB
  attributes:
    friendly: ownerwrite|policywrite|ownerread|written
    value: 0xA000220
  size: 446
```
