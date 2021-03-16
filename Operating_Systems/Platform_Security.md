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
# kill %1
# /usr/bin/tpm_server > /dev/null &
# tpm2_startup -c
```
Now the TPM 2.0 simulator is ready to use.

## File Measurements using PCR Registers <a name="section2">

First we list the contents of the `sha256` PCR bank
```console
# tpm2_pcrread sha256
sha256:
  0 : 0x0000000000000000000000000000000000000000000000000000000000000000
  1 : 0x0000000000000000000000000000000000000000000000000000000000000000
  2 : 0x0000000000000000000000000000000000000000000000000000000000000000
  3 : 0x0000000000000000000000000000000000000000000000000000000000000000
  4 : 0x0000000000000000000000000000000000000000000000000000000000000000
  5 : 0x0000000000000000000000000000000000000000000000000000000000000000
  6 : 0x0000000000000000000000000000000000000000000000000000000000000000
  7 : 0x0000000000000000000000000000000000000000000000000000000000000000
  8 : 0x0000000000000000000000000000000000000000000000000000000000000000
  9 : 0x0000000000000000000000000000000000000000000000000000000000000000
  10: 0x0000000000000000000000000000000000000000000000000000000000000000
  11: 0x0000000000000000000000000000000000000000000000000000000000000000
  12: 0x0000000000000000000000000000000000000000000000000000000000000000
  13: 0x0000000000000000000000000000000000000000000000000000000000000000
  14: 0x0000000000000000000000000000000000000000000000000000000000000000
  15: 0x0000000000000000000000000000000000000000000000000000000000000000
  16: 0x0000000000000000000000000000000000000000000000000000000000000000
  17: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  18: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  19: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  20: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  21: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  22: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  23: 0x0000000000000000000000000000000000000000000000000000000000000000
```
For legacy applications also a `sha1` PCR bank is available
```console
tpm2_pcrread sha1
sha1:
  0 : 0x0000000000000000000000000000000000000000
  1 : 0x0000000000000000000000000000000000000000
  2 : 0x0000000000000000000000000000000000000000
  3 : 0x0000000000000000000000000000000000000000
  4 : 0x0000000000000000000000000000000000000000
  5 : 0x0000000000000000000000000000000000000000
  6 : 0x0000000000000000000000000000000000000000
  7 : 0x0000000000000000000000000000000000000000
  8 : 0x0000000000000000000000000000000000000000
  9 : 0x0000000000000000000000000000000000000000
  10: 0x0000000000000000000000000000000000000000
  11: 0x0000000000000000000000000000000000000000
  12: 0x0000000000000000000000000000000000000000
  13: 0x0000000000000000000000000000000000000000
  14: 0x0000000000000000000000000000000000000000
  15: 0x0000000000000000000000000000000000000000
  16: 0x0000000000000000000000000000000000000000
  17: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  18: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  19: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  20: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  21: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  22: 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  23: 0x0000000000000000000000000000000000000000
```

The PCR banks implemented by the TPM simulator can be listed with
```console
# tpm2_getcap pcrs
selected-pcrs:
  - sha1: [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 ]
  - sha256: [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 ]
  - sha384: [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 ]
  - sha512: [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 ]
```
Currently available TPM devices usually support `sha1` and `sha256`,  only.

Now we determine the `sha1` and `sha256` hashes of the `bin/bash`file
```console
# sha1sum /bin/bash
41ba1bd49cb22466e422098d184bd4267ef9529e  /bin/bash
# sha256sum /bin/bash
04a484f27a4b485b28451923605d9b528453d6c098a5a5112bec859fb5f2eea9  /bin/bash
```

and  extend these values into PCR 10 of both the `sha256` and `sha1` PCR banks
```console
# tpm2_pcrextend 10:sha256=04a484f27a4b485b28451923605d9b528453d6c098a5a5112bec859fb5f2eea9,sha1=41ba1bd49cb22466e422098d184bd4267ef9529e
```

We are interested in the extended state of the PCR 10 registers only
```console
# tpm2_pcrread sha1:10+sha256:10
sha1:
  10: 0x0BDC2A96CA42A685DA6D75A7E551A8645209212D
sha256:
  10: 0x88D7CA34DB8D986D4BF69267128B007417D7DB5E52F04CA23065A0E41AB0C3B5
```

Now we compute the `sha256` hash of the `/bin/cp/` file
```console
# sha256sum /bin/cp
40ea53f38efe555c09a2c1c860379190e2af94427daaa68756874154c3fa1188  /bin/cp
```
and extend it into registers 10 and 16 of the `sha256` PCR bank
```console
tpm2_pcrextend 10:sha256=40ea53f38efe555c09a2c1c860379190e2af94427daaa68756874154c3fa1188 16:sha256=40ea53f38efe555c09a2c1c860379190e2af94427daaa68756874154c3fa1188
```

The following registers have now been extended
```console
# tpm2_pcrread sha1:10+sha256:10,16
sha1:
  10: 0x0BDC2A96CA42A685DA6D75A7E551A8645209212D
sha256:
  10: 0x57106023C8AE2B75F21FDD861A4A90CEBF42B774D58666A4E00762348463C393
  16: 0x84FBC2C45B05AB5891F96706C55B0F1CA42228CC09C7FE64459F711AB83C4256
```

We try to reset register 10 in all PCR banks
```console
# tpm2_pcrreset 10
WARNING:esys:src/tss2-esys/api/Esys_PCR_Reset.c:287:Esys_PCR_Reset_Finish() Received TPM Error 
ERROR:esys:src/tss2-esys/api/Esys_PCR_Reset.c:98:Esys_PCR_Reset() Esys Finish ErrorCode (0x00000907) 
ERROR: Esys_PCR_Reset(0x907) - tpm:warn(2.0): bad locality
ERROR: Could not reset PCR index: 10
ERROR: Unable to run tpm2_pcrreset
```
Fortunately this is not possible. Otherwise malicious malware could reset the PCR registers and extend them with fake measurements. On a PC motherboard only a cold reboot resets the PCR values.

An exception is PCR 16 that is intended for test purposes and thus can be reset
```console
# tpm2_pcrreset 16
# tpm2_pcrread sha1:10+sha256:10,16
sha1:
  10: 0x0BDC2A96CA42A685DA6D75A7E551A8645209212D
sha256:
  10: 0x57106023C8AE2B75F21FDD861A4A90CEBF42B774D58666A4E00762348463C393
  16: 0x0000000000000000000000000000000000000000000000000000000000000000
```
We see that PCR 16 has been reset to all zeroes.

## Endorsement Keys<a name="section3">

Each TPM has unique non-erasable *Endorsement Seed*. From this seed an *Endorsement Key* `EK` of any chosen key type can be derived in a deterministic way.

### RSA Endorsement Key

First we derive an `RSA` `EK` and store it in non-volatile RAM under the handle `0x81010001`
```console
# tpm2_createek -G rsa -c 0x81010001
# tpm2_getcap handles-persistent
- 0x81010001
```
With the help of its `tpm`plugin the strongSwan `pki` tool can read the unique keyid of a private key stored in a TPM based on its handle
```console
# pki --print --type priv --keyid 0x81010001
TPM 2.0 via TSS2 v2 available
signature algorithm is NULL with ERROR hash
  privkey:   RSA 2048 bits
  keyid:     ab:49:74:05:79:d8:fd:67:cc:fc:69:33:fa:67:a8:15:75:f7:17:00
  subjkey:   ee:8d:e6:b5:d9:47:e0:ad:3a:69:26:cd:7f:8f:01:48:1c:f0:f6:32
```
Since no signature algorithm is defined we notice that an `EK` is a decryption key and thus cannot be used for signatures.

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

The `pki` tool can extract the public key in standard `PKCS#1` format. During the extraction we increase the debug level to `3` thus showing the encoding of the public key.
```console
# pki --pub --type priv --keyid 0x81010001 --outform pem --debug 3 > ek_rsa.pem
```
The debug output first displays the device manufacturer and then lists some capabilities of the TPM
```console
TPM 2.0 - manufacturer: IBM  (SW   TPM) rev: 01.62 2020 FIPS 140-2
TPM 2.0 - algorithms: RSA SHA1 HMAC AES MGF1 KEYEDHASH XOR SHA256 SHA384 SHA512 RSASSA RSAES RSAPSS OAEP ECDSA ECDH ECDAA ECSCHNORR KDF1_SP800_56A KDF2 KDF1_SP800_108 ECC SYMCIPHER CAMELLIA CMAC CTR OFB CBC CFB ECB
TPM 2.0 - ECC curves: NIST_P256 NIST_P384 BN_P256
TPM 2.0 - PCR banks: SHA1 SHA256 SHA384 SHA512
TPM 2.0 via TSS2 v2 available
```
Then the `PKCS#1` encoding of the public key follows:
```console
L0 - subjectPublicKeyInfo:
L1 - algorithm:
L2 - algorithmIdentifier:
L3 - algorithm:
  'rsaEncryption'
L1 - subjectPublicKey:
=> 271 bytes @ 0x562fad1530a7
   0: 00 30 82 01 0A 02 82 01 01 00 C0 EE 7C A9 00 15  .0..........|...
  16: C2 2A 76 A2 E3 D1 BE F9 85 19 A2 45 E4 CE C6 6D  .*v........E...m
  32: 15 1C 30 89 3C 9E 43 25 EB 80 8C 04 B7 D3 D8 AB  ..0.<.C%........
  48: 15 AB 7D 49 E9 15 FE BF A2 4E F0 99 93 E7 97 D9  ..}I.....N......
  64: 30 55 E6 E8 18 AA 7F 04 FE 64 14 D8 8B E0 E9 54  0U.......d.....T
  80: 11 7B 93 1A 4F 26 43 32 0B C8 5C DF 76 85 0E B2  .{..O&C2..\.v...
  96: 94 B5 AD 9E 7B 21 7C 99 E5 30 9C E5 B3 AC CD 41  ....{!|..0.....A
 112: 93 0C 2A 0E 86 12 FA 1C 61 E6 9E 24 7E 73 0F F1  ..*.....a..$~s..
 128: F7 AA 7D FC C2 CE AC CF D7 D0 97 B6 FC FE 1F 3F  ..}............?
 144: 8B 62 71 F6 A8 17 78 2F A2 F9 AC 1E 8E 72 AE 35  .bq...x/.....r.5
 160: 58 E3 5A F4 B0 EA 78 E7 6C A6 6B 45 CA 99 E9 7F  X.Z...x.l.kE....
 176: 08 90 12 6B E4 25 AF 0A 27 B1 13 BF 95 EA 34 4A  ...k.%..'.....4J
 192: F9 8E 34 6B 38 63 DD FC 7C 61 53 63 86 7E 72 E8  ..4k8c..|aSc.~r.
 208: AE 04 56 EF EC 64 F8 65 F5 C1 BF 54 56 8B 4A 14  ..V..d.e...TV.J.
 224: A2 51 AC E0 AB CC 34 19 3C C8 BD 55 B8 64 BC 98  .Q....4.<..U.d..
 240: 39 0D 9F E2 B8 50 E4 E8 69 46 3C 13 3C 20 AE B5  9....P..iF<.< ..
 256: 7B 7E 50 62 D7 DE 2E 50 0C DD 02 03 01 00 01     {~Pb...P.......
-- > --
L0 - RSAPublicKey:
=> 270 bytes @ 0x562fad1530a8
   0: 30 82 01 0A 02 82 01 01 00 C0 EE 7C A9 00 15 C2  0..........|....
  16: 2A 76 A2 E3 D1 BE F9 85 19 A2 45 E4 CE C6 6D 15  *v........E...m.
  32: 1C 30 89 3C 9E 43 25 EB 80 8C 04 B7 D3 D8 AB 15  .0.<.C%.........
  48: AB 7D 49 E9 15 FE BF A2 4E F0 99 93 E7 97 D9 30  .}I.....N......0
  64: 55 E6 E8 18 AA 7F 04 FE 64 14 D8 8B E0 E9 54 11  U.......d.....T.
  80: 7B 93 1A 4F 26 43 32 0B C8 5C DF 76 85 0E B2 94  {..O&C2..\.v....
  96: B5 AD 9E 7B 21 7C 99 E5 30 9C E5 B3 AC CD 41 93  ...{!|..0.....A.
 112: 0C 2A 0E 86 12 FA 1C 61 E6 9E 24 7E 73 0F F1 F7  .*.....a..$~s...
 128: AA 7D FC C2 CE AC CF D7 D0 97 B6 FC FE 1F 3F 8B  .}............?.
 144: 62 71 F6 A8 17 78 2F A2 F9 AC 1E 8E 72 AE 35 58  bq...x/.....r.5X
 160: E3 5A F4 B0 EA 78 E7 6C A6 6B 45 CA 99 E9 7F 08  .Z...x.l.kE.....
 176: 90 12 6B E4 25 AF 0A 27 B1 13 BF 95 EA 34 4A F9  ..k.%..'.....4J.
 192: 8E 34 6B 38 63 DD FC 7C 61 53 63 86 7E 72 E8 AE  .4k8c..|aSc.~r..
 208: 04 56 EF EC 64 F8 65 F5 C1 BF 54 56 8B 4A 14 A2  .V..d.e...TV.J..
 224: 51 AC E0 AB CC 34 19 3C C8 BD 55 B8 64 BC 98 39  Q....4.<..U.d..9
 240: 0D 9F E2 B8 50 E4 E8 69 46 3C 13 3C 20 AE B5 7B  ....P..iF<.< ..{
 256: 7E 50 62 D7 DE 2E 50 0C DD 02 03 01 00 01        ~Pb...P.......
L1 - modulus:
=> 257 bytes @ 0x562fad1530b0
   0: 00 C0 EE 7C A9 00 15 C2 2A 76 A2 E3 D1 BE F9 85  ...|....*v......
  16: 19 A2 45 E4 CE C6 6D 15 1C 30 89 3C 9E 43 25 EB  ..E...m..0.<.C%.
  32: 80 8C 04 B7 D3 D8 AB 15 AB 7D 49 E9 15 FE BF A2  .........}I.....
  48: 4E F0 99 93 E7 97 D9 30 55 E6 E8 18 AA 7F 04 FE  N......0U.......
  64: 64 14 D8 8B E0 E9 54 11 7B 93 1A 4F 26 43 32 0B  d.....T.{..O&C2.
  80: C8 5C DF 76 85 0E B2 94 B5 AD 9E 7B 21 7C 99 E5  .\.v.......{!|..
  96: 30 9C E5 B3 AC CD 41 93 0C 2A 0E 86 12 FA 1C 61  0.....A..*.....a
 112: E6 9E 24 7E 73 0F F1 F7 AA 7D FC C2 CE AC CF D7  ..$~s....}......
 128: D0 97 B6 FC FE 1F 3F 8B 62 71 F6 A8 17 78 2F A2  ......?.bq...x/.
 144: F9 AC 1E 8E 72 AE 35 58 E3 5A F4 B0 EA 78 E7 6C  ....r.5X.Z...x.l
 160: A6 6B 45 CA 99 E9 7F 08 90 12 6B E4 25 AF 0A 27  .kE.......k.%..'
 176: B1 13 BF 95 EA 34 4A F9 8E 34 6B 38 63 DD FC 7C  .....4J..4k8c..|
 192: 61 53 63 86 7E 72 E8 AE 04 56 EF EC 64 F8 65 F5  aSc.~r...V..d.e.
 208: C1 BF 54 56 8B 4A 14 A2 51 AC E0 AB CC 34 19 3C  ..TV.J..Q....4.<
 224: C8 BD 55 B8 64 BC 98 39 0D 9F E2 B8 50 E4 E8 69  ..U.d..9....P..i
 240: 46 3C 13 3C 20 AE B5 7B 7E 50 62 D7 DE 2E 50 0C  F<.< ..{~Pb...P.
 256: DD                                               .
L1 - publicExponent:
=> 3 bytes @ 0x562fad1531b3
   0: 01 00 01                                         ...
-- < --
```
Similar information can be obtained with `openssl`
```console
# openssl rsa -in ek_rsa.pem -pubin -noout -text
RSA Public-Key: (2048 bit)
Modulus:
    00:c0:ee:7c:a9:00:15:c2:2a:76:a2:e3:d1:be:f9:
    85:19:a2:45:e4:ce:c6:6d:15:1c:30:89:3c:9e:43:
    25:eb:80:8c:04:b7:d3:d8:ab:15:ab:7d:49:e9:15:
    fe:bf:a2:4e:f0:99:93:e7:97:d9:30:55:e6:e8:18:
    aa:7f:04:fe:64:14:d8:8b:e0:e9:54:11:7b:93:1a:
    4f:26:43:32:0b:c8:5c:df:76:85:0e:b2:94:b5:ad:
    9e:7b:21:7c:99:e5:30:9c:e5:b3:ac:cd:41:93:0c:
    2a:0e:86:12:fa:1c:61:e6:9e:24:7e:73:0f:f1:f7:
    aa:7d:fc:c2:ce:ac:cf:d7:d0:97:b6:fc:fe:1f:3f:
    8b:62:71:f6:a8:17:78:2f:a2:f9:ac:1e:8e:72:ae:
    35:58:e3:5a:f4:b0:ea:78:e7:6c:a6:6b:45:ca:99:
    e9:7f:08:90:12:6b:e4:25:af:0a:27:b1:13:bf:95:
    ea:34:4a:f9:8e:34:6b:38:63:dd:fc:7c:61:53:63:
    86:7e:72:e8:ae:04:56:ef:ec:64:f8:65:f5:c1:bf:
    54:56:8b:4a:14:a2:51:ac:e0:ab:cc:34:19:3c:c8:
    bd:55:b8:64:bc:98:39:0d:9f:e2:b8:50:e4:e8:69:
    46:3c:13:3c:20:ae:b5:7b:7e:50:62:d7:de:2e:50:
    0c:dd
Exponent: 65537 (0x10001)
```

### ECC Endorsement Key

As an alternative we can derive an `ECC` `EK` based on *Elliptic Curve Cryptography* from the same secret *Endorsement Seed* and store it persistenly under the handle `0x81010002`
```console
# tpm2_createek -G ecc -c 0x81010002
# tpm2_getcap handles-persistent
- 0x81010001
- 0x81010002
```
The `pki` tool identifies the second `EK` as a 256 bit ECDSA decryption key
```console
# pki --print --type priv --keyid 0x81010002
TPM 2.0 via TSS2 v2 available
signature algorithm is NULL with ERROR hash
  privkey:   ECDSA 256 bits
  keyid:     c6:be:7a:6e:8b:5f:2b:90:fc:61:72:d4:8d:7e:84:57:40:66:75:4b
  subjkey:   b1:21:d7:07:80:04:00:75:0f:50:5d:70:1a:6c:da:6d:18:8b:70:35
```

### Key Removal from Persistent Memory

If we run out of non-volatile memory we can delete an *Endorsement Key* at any time
```console
# tpm2_evictcontrol -c 0x81010001
persistent-handle: 0x81010001
action: evicted
# tpm2_getcap handles-persistent
- 0x81010002
```
and can derive it again when the need occurs
```console
# tpm2_createek -G rsa -c 0x81010001
# pki --print --type priv --keyid 0x81010001
TPM 2.0 via TSS2 v2 available
signature algorithm is NULL with ERROR hash
  privkey:   RSA 2048 bits
  keyid:     ab:49:74:05:79:d8:fd:67:cc:fc:69:33:fa:67:a8:15:75:f7:17:00
  subjkey:   ee:8d:e6:b5:d9:47:e0:ad:3a:69:26:cd:7f:8f:01:48:1c:f0:f6:32
```
As can be easily verified the same `EK` as before results.

## Attestation Keys<a name="section4">

Under the *Endorsement Hierarchy* we can create as many random signature keys as we need for our applications. Microsoft calls this the  *Virtual Smartcard* functionality of a TPM.

### Built-In True Random Generator

In order to generate RSA or ECC keys the TPM needs a built-in *True Random Generator*.
```console
# tpm2_getrandom --hex 10
54be6d8a4c041177ccc7
```
Up to 64 random bytes can be extracted in single call by `tpm2_getrandom`

### RSA Attestation Key

We generate a random RSA *Attestation Key* `AK` protected by the RSA *Endorsement Key* `EK` with handle `0x81010001`and store it in the context file  `ak_rsa.ctx`. The use of the  *Attestation Key* is protected by the password `123456`.
```console
# tpm2_createak -C 0x81010001 -G rsa -g sha256 -s rsassa -p 123456 -c ak_rsa.ctx -u ak_rsa.pub -n ak_rsa.name
loaded-key:
  name: 000b2d94f5f7d5b488d26311baeddbf220c57ef54c803089b61fec459425ab5d855a
  qualified name: 000bdb1a99740862997b37e10f57fbab3cda984dbaec0ccd1022597c00e1d51af000
```
We then persist the key under the handle `0x81010003`
```console
# tpm2_evictcontrol -C o -c ak_rsa.ctx 0x81010003
persistent-handle: 0x81010003
action: persisted
```
The `pki` command shows that a RSA signature key has been generated
```console
# pki --print --type priv --keyid 0x81010003
TPM 2.0 via TSS2 v2 available
signature algorithm is RSASSA with SHA256 hash
  privkey:   RSA 2048 bits
  keyid:     08:d8:e6:3a:2f:03:c2:64:47:1a:1f:58:b1:24:56:23:6b:59:91:e0
  subjkey:   50:01:79:bd:c2:b3:07:ba:93:1c:fb:a7:2a:11:f3:78:8f:bf:45:2d
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
                    00:a6:16:78:6e:7b:dd:3b:a5:9b:11:a8:ec:99:ab:
                    bd:de:15:34:21:93:65:f4:c8:18:eb:d4:85:4c:1f:
                    59:a7:32:35:e6:8c:63:3d:b7:27:c4:04:e1:10:f3:
                    ec:37:b4:77:2b:9a:bc:dc:eb:c3:23:ab:6a:c1:82:
                    e3:68:27:b8:cc:85:15:c4:6c:5d:3f:81:d5:a1:f9:
                    c0:6e:c3:5a:ad:f9:3f:d5:ca:6c:83:c1:39:28:bf:
                    a7:ee:4f:69:7c:64:9a:57:00:6a:3c:16:ac:75:28:
                    be:c0:e0:f5:89:8a:99:a6:7c:25:25:bd:b4:33:7e:
                    cb:d9:1a:22:46:03:87:e1:e5:97:b4:02:2b:ae:be:
                    fc:90:75:9e:21:13:39:6b:37:5a:bd:fe:66:31:cd:
                    9e:97:26:c8:b3:21:2a:62:8f:d0:1c:d4:62:fc:20:
                    ea:31:45:2f:37:8b:cf:83:e8:ba:37:0a:be:40:5f:
                    bc:8e:30:88:5f:36:b2:4c:1d:b0:93:0e:4d:fc:86:
                    a9:c4:a7:50:4d:aa:57:ab:3f:a6:07:5a:e3:50:a9:
                    9b:31:e6:bc:36:da:38:ca:3b:65:42:37:02:ff:f0:
                    35:96:a8:40:4c:f4:48:90:64:05:93:53:f0:a9:a9:
                    80:ff:d4:a9:c3:90:d9:3c:8c:cb:42:79:2b:a1:b4:
                    b8:15
                Exponent: 65537 (0x10001)
        Attributes:
        Requested Extensions:
            X509v3 Subject Alternative Name: 
                email:user@strongswan.org
    Signature Algorithm: sha256WithRSAEncryption
         08:bf:c6:23:c8:e2:af:c4:9e:df:4d:7e:ba:16:90:db:85:e8:
         12:15:38:6e:18:db:d0:c0:14:a0:7c:fb:9f:7b:6b:02:b9:c8:
         65:c8:0d:c5:bd:83:b8:d7:9d:eb:50:7e:a3:69:7d:00:e8:25:
         f9:b2:34:26:3e:e6:69:0e:d5:e4:b8:e3:45:64:90:99:17:63:
         ec:4b:a9:c4:39:fc:81:ed:4a:4c:20:13:32:27:c0:b8:6a:22:
         b9:2c:aa:85:4e:dd:99:c6:76:da:e0:64:ac:3f:79:a9:5f:bc:
         0c:2c:25:16:d4:53:e2:e1:54:a0:36:80:51:cb:40:2b:4d:4c:
         f0:3f:bd:f4:b2:cb:65:7a:60:f0:8d:44:9e:51:2a:89:0c:58:
         39:d0:00:db:11:de:b8:26:e8:61:05:a8:d1:d9:d9:dd:6e:c1:
         26:54:65:80:fe:07:f3:61:53:9f:a8:23:54:0b:f9:a9:95:88:
         19:b3:4e:04:30:19:0d:cc:2d:2a:e0:bf:c0:5e:85:d5:5a:04:
         df:b7:38:21:cc:76:86:20:a8:08:e6:91:58:b2:17:f1:e0:bd:
         65:6b:81:02:93:7a:63:30:0b:37:e6:fb:e1:0a:aa:f0:16:af:
         fa:25:47:3d:89:5f:c3:86:34:27:97:3e:27:6f:10:01:0a:5d:
         0d:f0:01:41
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
  validity:  not before Mar 15 19:18:26 2021, ok
             not after  Mar 15 19:18:26 2031, ok (expires in 3651 days)
  serial:    36:15:f1:2b:b1:30:b2:05
  flags:     CA CRLSign self-signed 
  subjkeyId: 67:30:84:87:c9:e1:b3:30:51:a2:73:01:ec:c7:f5:76:e2:48:f7:5b
  pubkey:    RSA 2048 bits
  keyid:     3c:6d:e8:43:00:25:ad:24:56:06:e0:e7:68:7b:85:38:cc:d1:e0:30
  subjkey:   67:30:84:87:c9:e1:b3:30:51:a2:73:01:ec:c7:f5:76:e2:48:f7:5b
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
  validity:  not before Mar 15 19:19:26 2021, ok
             not after  Mar 15 19:19:26 2026, ok (expires in 1825 days)
  serial:    50:6c:fa:ac:65:2c:9c:a0
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: 67:30:84:87:c9:e1:b3:30:51:a2:73:01:ec:c7:f5:76:e2:48:f7:5b
  subjkeyId: 50:01:79:bd:c2:b3:07:ba:93:1c:fb:a7:2a:11:f3:78:8f:bf:45:2d
  pubkey:    RSA 2048 bits
  keyid:     08:d8:e6:3a:2f:03:c2:64:47:1a:1f:58:b1:24:56:23:6b:59:91:e0
  subjkey:   50:01:79:bd:c2:b3:07:ba:93:1c:fb:a7:2a:11:f3:78:8f:bf:45:2d
```

### RSA Attestation Certificate

In order to protect the `AK` certificate received from the `CA` from accidental deletion from the system harddisk we want to store it in the *Non-Volatile* `NV` memory of the TPM. To allocate the required amount of  `NV` memory we determine the exact size of the binary-encoded X.509 certificate
```console
ls -l ak_rsa_Cert.der 
-rw-r--r-- 1 root root 842 Mar 15 19:19 ak_rsa_Cert.der
```
and then allocate 842 bytes of `NV` memory under  the handle `0x01800003`
```console
# tpm2_nvdefine 0x01800003 -C o -s 842 -a 0x2000A
nv-index: 0x1800003
```
Now the certificate can be written into the pre-allocated memory slot
```console
# tpm2_nvwrite 0x01800003 -C o -i ak_rsa_Cert.der
```
The following command shows all objects stored in `NV` memory
```console
# tpm2_getcap handles-nv-index
- 0x1800003
```
The `pki` tool can display X.509 certificates stored in the TPM using the object handle
```console
# pki --print --type x509 --keyid 0x01800003
TPM 2.0 via TSS2 v2 available
loaded certificate from TPM NV index 0x01800003
  subject:  "C=CH, O=Cyber, OU=user@strongswan.org"
  issuer:   "C=CH, O=Cyber, CN=My RSA CA"
  validity:  not before Mar 15 19:19:26 2021, ok
             not after  Mar 15 19:19:26 2026, ok (expires in 1825 days)
  serial:    50:6c:fa:ac:65:2c:9c:a0
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: 67:30:84:87:c9:e1:b3:30:51:a2:73:01:ec:c7:f5:76:e2:48:f7:5b
  subjkeyId: 50:01:79:bd:c2:b3:07:ba:93:1c:fb:a7:2a:11:f3:78:8f:bf:45:2d
  pubkey:    RSA 2048 bits
  keyid:     08:d8:e6:3a:2f:03:c2:64:47:1a:1f:58:b1:24:56:23:6b:59:91:e0
  subjkey:   50:01:79:bd:c2:b3:07:ba:93:1c:fb:a7:2a:11:f3:78:8f:bf:45:2d
```
A `strongSwan` IKEv2 client could use the `AK` private key and the `AK` certificate stored in  the TPM to authenticate itself in an IPsec connection.

### Flushing the Context

From time to time it is recommended to flush the context so that the TPM doesn't run out of transient memory:
```console
# tpm2_flushcontext -t
```

### ECC Attestation Key

As an alternative we generate a random ECC *Attestation Key* `AK` protected by the ECC *Endorsement Key* `EK` with handle `0x81010002`and persist it under the handle `0x81010004`.
```console
# tpm2_createak -C 0x81010002 -G ecc -g sha256 -s ecdsa -c ak_ecc.ctx -u ak_ecc.pub -n ak_ecc.name
loaded-key:
  name: 000b49a626facf456be0381f8b6d1121db3512f50f0ff22257c3f94e535dcff8a8a5
  qualified name: 000b292a7407d40a715dfd24c3f44c80513a3757a337ce374946984e0cd56de1f4e8
# tpm2_evictcontrol -C o -c ak_ecc.ctx 0x81010004
persistent-handle: 0x81010004
action: persisted
```
The `pki` command shows that an ECDSA signature key has been generated
```console
# pki --print --type priv --keyid 0x81010004
TPM 2.0 via TSS2 v2 available
signature algorithm is ECDSA with SHA256 hash
  privkey:   ECDSA 256 bits
  keyid:     2d:cb:20:6a:70:6a:82:0c:37:54:f7:2b:f8:38:6e:29:e5:19:27:72
  subjkey:   4a:3c:47:e6:4d:1a:cd:0e:de:27:18:6e:76:12:64:68:53:e1:ee:ae
```
In order to obtain an X.509 certificate issued and signed by a trusted *Certification Authority* `CA` we again generate a `PKCS#10` *Certificate Request* signed by the private `AK` stored in the TPM.
```console
# pki --req --keyid 0x81010004 --outform pem --dn "C=CH, O=Cyber, OU=user@strongswan.org" --san user@strongswan.org > ak_ecc_Req.pem
TPM 2.0 via TSS2 v2 available
signature algorithm is ECDSA with SHA256 hash
Smartcard PIN: 
<return>
```
Since no password has been configured just press the `<return>` key

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
                    04:bb:f2:e3:c9:1e:ca:a9:b2:6b:ab:86:97:f6:e8:
                    47:89:3f:78:c8:58:d2:0f:24:01:e3:5c:df:57:42:
                    63:11:41:1c:54:a1:a1:78:d8:b2:ea:e3:0c:17:a5:
                    43:6c:3c:30:a4:3f:24:b2:65:4b:aa:f6:0d:20:b0:
                    a2:16:35:04:d5
                ASN1 OID: prime256v1
                NIST CURVE: P-256
        Attributes:
        Requested Extensions:
            X509v3 Subject Alternative Name: 
                email:user@strongswan.org
    Signature Algorithm: ecdsa-with-SHA256
         30:46:02:21:00:93:59:69:78:0c:9c:b4:ba:fc:7d:3c:6a:95:
         a4:56:c8:bf:97:04:f9:e0:15:7c:37:95:29:8f:f5:66:ed:d5:
         2d:02:21:00:bb:9b:ab:20:7e:ff:74:b7:21:57:0c:ae:cb:70:
         c3:bb:3d:9f:4d:fc:00:39:d6:8c:61:00:c6:7d:49:28:14:a2
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
  validity:  not before Mar 15 19:37:08 2021, ok
             not after  Mar 15 19:37:08 2031, ok (expires in 3651 days)
  serial:    04:ab:83:95:ed:95:a1:e8
  flags:     CA CRLSign self-signed 
  subjkeyId: 80:a7:df:a0:28:c4:6a:d4:ac:2b:8e:82:5d:c6:c2:ee:79:65:bc:8a
  pubkey:    ECDSA 256 bits
  keyid:     b3:d5:b2:d6:12:e8:c3:91:d5:c4:45:43:1e:be:b9:9b:02:da:ee:7a
  subjkey:   80:a7:df:a0:28:c4:6a:d4:ac:2b:8e:82:5d:c6:c2:ee:79:65:bc:8a
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
  validity:  not before Mar 15 19:37:56 2021, ok
             not after  Mar 15 19:37:56 2026, ok (expires in 1825 days)
  serial:    16:0d:65:68:c1:a1:45:2c
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: 80:a7:df:a0:28:c4:6a:d4:ac:2b:8e:82:5d:c6:c2:ee:79:65:bc:8a
  subjkeyId: 4a:3c:47:e6:4d:1a:cd:0e:de:27:18:6e:76:12:64:68:53:e1:ee:ae
  pubkey:    ECDSA 256 bits
  keyid:     2d:cb:20:6a:70:6a:82:0c:37:54:f7:2b:f8:38:6e:29:e5:19:27:72
  subjkey:   4a:3c:47:e6:4d:1a:cd:0e:de:27:18:6e:76:12:64:68:53:e1:ee:ae
```

### ECC Attestation Certificate

We again determine the exact size of the binary-encoded X.509 certificate
```console
# ls -l ak_ecc_Cert.der 
-rw-r--r-- 1 root root 445 Mar 15 19:37 ak_ecc_Cert.der
```
Next we allocate 445 bytes needed by the X.509 certificate in the NV-RAM
```console
# tpm2_nvdefine 0x01800004 -C o -s 445 -a 0x2000A
nv-index: 0x1800004
```
The certificate can now be stored in the NV memory under the handle `0x01800004`
```console
tpm2_nvwrite 0x01800004 -C o -i ak_ecc_Cert.der
```
There now two objects stored in NV-RAM
```console
# tpm2_getcap handles-nv-index
- 0x1800003
- 0x1800004
```
More details on the objects stored in NV-RAM can be displayed with
```console
# tpm2_nvreadpublic
0x1800003:
  name: 000b1e8b703ee37027964f336f9ce001933d6f80b032daf61b5c6f87886b94cf90fc
  hash algorithm:
    friendly: sha256
    value: 0xB
  attributes:
    friendly: ownerwrite|policywrite|ownerread|written
    value: 0xA000220
  size: 842
0x1800004:
  name: 000bfa0ac081aeea0f7fdc7d6efdf104ae883b7e6c82106bd0d64a4e68f5d7f1ded5
  hash algorithm:
    friendly: sha256
    value: 0xB
  attributes:
    friendly: ownerwrite|policywrite|ownerread|written
    value: 0xA000220
  size: 445

```
The `pki` tool can read the X.509 certificate stored in the TPM
```console
# pki --print --type x509 --keyid 0x01800004
TPM 2.0 via TSS2 v2 available
loaded certificate from TPM NV index 0x01800004
  subject:  "C=CH, O=Cyber, OU=user@strongswan.org"
  issuer:   "C=CH, O=Cyber, CN=My ECC CA"
  validity:  not before Mar 15 19:37:56 2021, ok
             not after  Mar 15 19:37:56 2026, ok (expires in 1825 days)
  serial:    16:0d:65:68:c1:a1:45:2c
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: 80:a7:df:a0:28:c4:6a:d4:ac:2b:8e:82:5d:c6:c2:ee:79:65:bc:8a
  subjkeyId: 4a:3c:47:e6:4d:1a:cd:0e:de:27:18:6e:76:12:64:68:53:e1:ee:ae
  pubkey:    ECDSA 256 bits
  keyid:     2d:cb:20:6a:70:6a:82:0c:37:54:f7:2b:f8:38:6e:29:e5:19:27:72
  subjkey:   4a:3c:47:e6:4d:1a:cd:0e:de:27:18:6e:76:12:64:68:53:e1:ee:ae
```

### Object Removal from Non-Volatile Memory

If an object isn't used anymore it can be removed from NV-RAM with the following command
```console
# tpm2_nvundefine 0x01800003 -C o
# tpm2_getcap handles-nv-index
- 0x1800004
```

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/

