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
6b6843dae13e009a3267eed8f472bebe47e39e31  /bin/bash
# sha256sum /bin/bash
2c336c63e26881d2f02f34379024e7c314bce572c08cbaa319bacbbec29f93ed  /bin/bash
```

and  extend these values into PCR 10 of both the `sha256` and `sha1` PCR banks
```console
tpm2_pcrextend 10:sha256=2c336c63e26881d2f02f34379024e7c314bce572c08cbaa319bacbbec29f93ed,sha1=6b6843dae13e009a3267eed8f472bebe47e39e31
```

We are interested in the extended state of the PCR 10 registers only
```console
# tpm2_pcrread sha1:10+sha256:10
  sha1:
    10: 0x24FC4C679DFEF3C24E0BA32F371B380991A1A6BA
  sha256:
    10: 0x1471960C96CB1946ACCCCDE5DB98BD175298F90E43F92F47635C7D0594A59D8D
```

Now we compute the `sha256` hash of the `/bin/cp/` file
```console
# sha256sum /bin/cp
5cbd6e8d5aaa714a0910f5ee7710004cbf0a22c940eda1abec6f0194f430b94c  /bin/cp
```
and extend it into registers 10 and 16 of the `sha256` PCR bank
```console
tpm2_pcrextend 10:sha256=5cbd6e8d5aaa714a0910f5ee7710004cbf0a22c940eda1abec6f0194f430b94c 16:sha256=5cbd6e8d5aaa714a0910f5ee7710004cbf0a22c940eda1abec6f0194f430b94c
```

The following registers have now been extended
```console
# tpm2_pcrread sha1:10+sha256:10,16
  sha1:
    10: 0x24FC4C679DFEF3C24E0BA32F371B380991A1A6BA
  sha256:
    10: 0x13DBDF26B9BAB4A8FDF2FCFF30C560D7BFA5334B762318BCD859EB743985C592
    16: 0xD56AF09F9243A7F156C525AFA276F647BEF55F69C67D6F5F8A6F34B9626E2E9B
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
    10: 0x24FC4C679DFEF3C24E0BA32F371B380991A1A6BA
  sha256:
    10: 0x13DBDF26B9BAB4A8FDF2FCFF30C560D7BFA5334B762318BCD859EB743985C592
    16: 0x0000000000000000000000000000000000000000000000000000000000000000
```
We see that PCR 16 has been reset to all zeroes.

## Endorsement Keys<a name="section3">

Each TPM has unique non-erasable *Endorsement Seed*. From this seed an *Endorsement Key* `EK` of any chosen key type can be derived in a deterministic way.

### RSA Endorsement Key

First we derive an `RSA` `EK` and store it in non-volatile RAM under the handle `0x81010001`
```console
# tpm2_createek -G rsa3072 -c 0x81010001
# tpm2_getcap handles-persistent
- 0x81010001
```
With the help of its `tpm`plugin the strongSwan `pki` tool can read the unique keyid of a private key stored in a TPM based on its handle
```console
# pki --print --type priv --keyid 0x81010001
TPM 2.0 via TSS2 v2 available
encryption algorithm is AES-CFB with 256 bits
  privkey:   RSA 3072 bits
  keyid:     4f:8b:b8:02:3d:e0:cd:83:5e:47:b5:ff:68:28:20:d7:72:18:34:e6
  subjkey:   a9:22:9c:54:f0:56:da:57:d5:7d:70:07:7d:d9:a3:c7:af:4c:6c:f7
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
L3 - parameters:
=> 2 bytes @ 0x55dffd163d31
   0: 05 00                                            ..
L1 - subjectPublicKey:
=> 399 bytes @ 0x55dffd163d37
   0: 00 30 82 01 8A 02 82 01 81 00 99 7E 46 95 C0 13  .0.........~F...
  16: AE 0F A0 74 02 17 BB FD 5B A5 EC 5F D1 A3 98 FA  ...t....[.._....
  32: F9 D7 97 6B 67 BA 58 87 01 CE E3 9A 89 9A AF 02  ...kg.X.........
  48: B7 46 E0 53 4B 91 09 59 2C 08 47 90 26 C0 8A 8B  .F.SK..Y,.G.&...
  64: F7 9A DE 75 3F D6 73 50 5F 0A F2 0E D4 1F 7F 5C  ...u?.sP_......\
  80: D5 88 0C BE 8E 4B F6 B0 AC A6 1D 9F A5 62 05 1F  .....K.......b..
  96: 28 00 AD 12 84 32 FC BA A5 7D 93 63 F4 3E 0F 69  (....2...}.c.>.i
 112: D0 86 BD 84 84 DD 5C D3 A5 91 BE 10 85 0D 9A 5E  ......\........^
 128: E1 F8 57 9C 6E 74 55 61 EF 90 86 4A EC 83 B3 55  ..W.ntUa...J...U
 144: 4E 89 8C E7 E4 7D 7E 16 AA 73 39 B4 04 0A D7 82  N....}~..s9.....
 160: 38 0B A7 AF 78 F3 6D 86 A7 09 F1 B6 55 D1 C3 08  8...x.m.....U...
 176: 5A AF ED 2D 19 90 EB DC 68 E2 5C 34 E0 43 1B B2  Z..-....h.\4.C..
 192: 7C 8B 10 38 84 E3 E7 C1 21 FF 6A B9 29 7B F1 CB  |..8....!.j.){..
 208: 1A 67 65 73 89 E8 50 1C 69 E1 48 6C 36 F4 73 4B  .ges..P.i.Hl6.sK
 224: B2 09 30 50 F6 57 EE 92 EC 52 16 CF 2B 00 52 01  ..0P.W...R..+.R.
 240: FA B9 78 15 B6 3F C4 2C 8D B6 B1 79 1F AB 2A 03  ..x..?.,...y..*.
 256: 1F 5F 51 CA A8 FC C6 9D 72 4E 64 BC 1A 17 55 77  ._Q.....rNd...Uw
 272: 12 CB 3D 77 17 9D 26 5E 52 B1 94 FB 52 72 5D AE  ..=w..&^R...Rr].
 288: 97 E0 57 4F 4F 27 41 26 01 76 1F 65 04 06 A8 09  ..WOO'A&.v.e....
 304: 33 35 D1 5E 7E AF DC C2 13 4D 80 DF D2 0E DA 18  35.^~....M......
 320: 6C EC A7 DF C1 32 A4 3D 8F A7 D6 7D 45 DB 6F 97  l....2.=...}E.o.
 336: FE 25 30 C8 30 B2 98 2E DE 33 C7 B2 15 DE A8 1F  .%0.0....3......
 352: B1 60 8B 93 83 07 E1 FA 1F 62 91 00 38 56 CE 10  .`.......b..8V..
 368: 79 50 C4 09 56 64 93 76 F5 2A E5 99 DA 35 95 79  yP..Vd.v.*...5.y
 384: 60 77 A4 39 24 81 97 17 3B C3 02 03 01 00 01     `w.9$...;......
-- > --
L0 - RSAPublicKey:
=> 398 bytes @ 0x55dffd163d38
   0: 30 82 01 8A 02 82 01 81 00 99 7E 46 95 C0 13 AE  0.........~F....
  16: 0F A0 74 02 17 BB FD 5B A5 EC 5F D1 A3 98 FA F9  ..t....[.._.....
  32: D7 97 6B 67 BA 58 87 01 CE E3 9A 89 9A AF 02 B7  ..kg.X..........
  48: 46 E0 53 4B 91 09 59 2C 08 47 90 26 C0 8A 8B F7  F.SK..Y,.G.&....
  64: 9A DE 75 3F D6 73 50 5F 0A F2 0E D4 1F 7F 5C D5  ..u?.sP_......\.
  80: 88 0C BE 8E 4B F6 B0 AC A6 1D 9F A5 62 05 1F 28  ....K.......b..(
  96: 00 AD 12 84 32 FC BA A5 7D 93 63 F4 3E 0F 69 D0  ....2...}.c.>.i.
 112: 86 BD 84 84 DD 5C D3 A5 91 BE 10 85 0D 9A 5E E1  .....\........^.
 128: F8 57 9C 6E 74 55 61 EF 90 86 4A EC 83 B3 55 4E  .W.ntUa...J...UN
 144: 89 8C E7 E4 7D 7E 16 AA 73 39 B4 04 0A D7 82 38  ....}~..s9.....8
 160: 0B A7 AF 78 F3 6D 86 A7 09 F1 B6 55 D1 C3 08 5A  ...x.m.....U...Z
 176: AF ED 2D 19 90 EB DC 68 E2 5C 34 E0 43 1B B2 7C  ..-....h.\4.C..|
 192: 8B 10 38 84 E3 E7 C1 21 FF 6A B9 29 7B F1 CB 1A  ..8....!.j.){...
 208: 67 65 73 89 E8 50 1C 69 E1 48 6C 36 F4 73 4B B2  ges..P.i.Hl6.sK.
 224: 09 30 50 F6 57 EE 92 EC 52 16 CF 2B 00 52 01 FA  .0P.W...R..+.R..
 240: B9 78 15 B6 3F C4 2C 8D B6 B1 79 1F AB 2A 03 1F  .x..?.,...y..*..
 256: 5F 51 CA A8 FC C6 9D 72 4E 64 BC 1A 17 55 77 12  _Q.....rNd...Uw.
 272: CB 3D 77 17 9D 26 5E 52 B1 94 FB 52 72 5D AE 97  .=w..&^R...Rr]..
 288: E0 57 4F 4F 27 41 26 01 76 1F 65 04 06 A8 09 33  .WOO'A&.v.e....3
 304: 35 D1 5E 7E AF DC C2 13 4D 80 DF D2 0E DA 18 6C  5.^~....M......l
 320: EC A7 DF C1 32 A4 3D 8F A7 D6 7D 45 DB 6F 97 FE  ....2.=...}E.o..
 336: 25 30 C8 30 B2 98 2E DE 33 C7 B2 15 DE A8 1F B1  %0.0....3.......
 352: 60 8B 93 83 07 E1 FA 1F 62 91 00 38 56 CE 10 79  `.......b..8V..y
 368: 50 C4 09 56 64 93 76 F5 2A E5 99 DA 35 95 79 60  P..Vd.v.*...5.y`
 384: 77 A4 39 24 81 97 17 3B C3 02 03 01 00 01        w.9$...;......
L1 - modulus:
=> 385 bytes @ 0x55dffd163d40
   0: 00 99 7E 46 95 C0 13 AE 0F A0 74 02 17 BB FD 5B  ..~F......t....[
  16: A5 EC 5F D1 A3 98 FA F9 D7 97 6B 67 BA 58 87 01  .._.......kg.X..
  32: CE E3 9A 89 9A AF 02 B7 46 E0 53 4B 91 09 59 2C  ........F.SK..Y,
  48: 08 47 90 26 C0 8A 8B F7 9A DE 75 3F D6 73 50 5F  .G.&......u?.sP_
  64: 0A F2 0E D4 1F 7F 5C D5 88 0C BE 8E 4B F6 B0 AC  ......\.....K...
  80: A6 1D 9F A5 62 05 1F 28 00 AD 12 84 32 FC BA A5  ....b..(....2...
  96: 7D 93 63 F4 3E 0F 69 D0 86 BD 84 84 DD 5C D3 A5  }.c.>.i......\..
 112: 91 BE 10 85 0D 9A 5E E1 F8 57 9C 6E 74 55 61 EF  ......^..W.ntUa.
 128: 90 86 4A EC 83 B3 55 4E 89 8C E7 E4 7D 7E 16 AA  ..J...UN....}~..
 144: 73 39 B4 04 0A D7 82 38 0B A7 AF 78 F3 6D 86 A7  s9.....8...x.m..
 160: 09 F1 B6 55 D1 C3 08 5A AF ED 2D 19 90 EB DC 68  ...U...Z..-....h
 176: E2 5C 34 E0 43 1B B2 7C 8B 10 38 84 E3 E7 C1 21  .\4.C..|..8....!
 192: FF 6A B9 29 7B F1 CB 1A 67 65 73 89 E8 50 1C 69  .j.){...ges..P.i
 208: E1 48 6C 36 F4 73 4B B2 09 30 50 F6 57 EE 92 EC  .Hl6.sK..0P.W...
 224: 52 16 CF 2B 00 52 01 FA B9 78 15 B6 3F C4 2C 8D  R..+.R...x..?.,.
 240: B6 B1 79 1F AB 2A 03 1F 5F 51 CA A8 FC C6 9D 72  ..y..*.._Q.....r
 256: 4E 64 BC 1A 17 55 77 12 CB 3D 77 17 9D 26 5E 52  Nd...Uw..=w..&^R
 272: B1 94 FB 52 72 5D AE 97 E0 57 4F 4F 27 41 26 01  ...Rr]...WOO'A&.
 288: 76 1F 65 04 06 A8 09 33 35 D1 5E 7E AF DC C2 13  v.e....35.^~....
 304: 4D 80 DF D2 0E DA 18 6C EC A7 DF C1 32 A4 3D 8F  M......l....2.=.
 320: A7 D6 7D 45 DB 6F 97 FE 25 30 C8 30 B2 98 2E DE  ..}E.o..%0.0....
 336: 33 C7 B2 15 DE A8 1F B1 60 8B 93 83 07 E1 FA 1F  3.......`.......
 352: 62 91 00 38 56 CE 10 79 50 C4 09 56 64 93 76 F5  b..8V..yP..Vd.v.
 368: 2A E5 99 DA 35 95 79 60 77 A4 39 24 81 97 17 3B  *...5.y`w.9$...;
 384: C3                                               .
L1 - publicExponent:
=> 3 bytes @ 0x55dffd163ec3
   0: 01 00 01 
   -- < --
```
Similar information can be obtained with `openssl`
```console
# openssl rsa -in ek_rsa.pem -pubin -noout -text
Public-Key: (3072 bit)
Modulus:
    00:99:7e:46:95:c0:13:ae:0f:a0:74:02:17:bb:fd:
    5b:a5:ec:5f:d1:a3:98:fa:f9:d7:97:6b:67:ba:58:
    87:01:ce:e3:9a:89:9a:af:02:b7:46:e0:53:4b:91:
    09:59:2c:08:47:90:26:c0:8a:8b:f7:9a:de:75:3f:
    d6:73:50:5f:0a:f2:0e:d4:1f:7f:5c:d5:88:0c:be:
    8e:4b:f6:b0:ac:a6:1d:9f:a5:62:05:1f:28:00:ad:
    12:84:32:fc:ba:a5:7d:93:63:f4:3e:0f:69:d0:86:
    bd:84:84:dd:5c:d3:a5:91:be:10:85:0d:9a:5e:e1:
    f8:57:9c:6e:74:55:61:ef:90:86:4a:ec:83:b3:55:
    4e:89:8c:e7:e4:7d:7e:16:aa:73:39:b4:04:0a:d7:
    82:38:0b:a7:af:78:f3:6d:86:a7:09:f1:b6:55:d1:
    c3:08:5a:af:ed:2d:19:90:eb:dc:68:e2:5c:34:e0:
    43:1b:b2:7c:8b:10:38:84:e3:e7:c1:21:ff:6a:b9:
    29:7b:f1:cb:1a:67:65:73:89:e8:50:1c:69:e1:48:
    6c:36:f4:73:4b:b2:09:30:50:f6:57:ee:92:ec:52:
    16:cf:2b:00:52:01:fa:b9:78:15:b6:3f:c4:2c:8d:
    b6:b1:79:1f:ab:2a:03:1f:5f:51:ca:a8:fc:c6:9d:
    72:4e:64:bc:1a:17:55:77:12:cb:3d:77:17:9d:26:
    5e:52:b1:94:fb:52:72:5d:ae:97:e0:57:4f:4f:27:
    41:26:01:76:1f:65:04:06:a8:09:33:35:d1:5e:7e:
    af:dc:c2:13:4d:80:df:d2:0e:da:18:6c:ec:a7:df:
    c1:32:a4:3d:8f:a7:d6:7d:45:db:6f:97:fe:25:30:
    c8:30:b2:98:2e:de:33:c7:b2:15:de:a8:1f:b1:60:
    8b:93:83:07:e1:fa:1f:62:91:00:38:56:ce:10:79:
    50:c4:09:56:64:93:76:f5:2a:e5:99:da:35:95:79:
    60:77:a4:39:24:81:97:17:3b:c3
Exponent: 65537 (0x10001)
```

### ECC Endorsement Key

As an alternative we can derive an `ECC` `EK` based on *Elliptic Curve Cryptography* from the same secret *Endorsement Seed* and store it persistenly under the handle `0x81010002`
```console
# tpm2_createek -G ecc384 -c 0x81010002
# tpm2_getcap handles-persistent
- 0x81010001
- 0x81010002
```
The `pki` tool identifies the second `EK` as a 256 bit ECDSA decryption key
```console
# pki --print --type priv --keyid 0x81010002
TPM 2.0 via TSS2 v2 available
encryption algorithm is AES-CFB with 256 bits
  privkey:   ECDSA 384 bits
  keyid:     ac:bf:5f:9e:7b:bc:f2:82:dc:cf:67:db:62:01:cb:5d:73:93:66:f6
  subjkey:   d1:54:63:9e:3c:aa:0c:a9:03:a0:fb:fb:6c:0f:ef:16:bd:1f:89:e5
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
# tpm2_createek -G rsa3072 -c 0x81010001
# pki --print --type priv --keyid 0x81010001
TPM 2.0 via TSS2 v2 available
encryption algorithm is AES-CFB with 256 bits
  privkey:   RSA 3072 bits
  keyid:     4f:8b:b8:02:3d:e0:cd:83:5e:47:b5:ff:68:28:20:d7:72:18:34:e6
  subjkey:   a9:22:9c:54:f0:56:da:57:d5:7d:70:07:7d:d9:a3:c7:af:4c:6c:f7
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
# tpm2_createak -C 0x81010001 -G rsa3072 -g sha256 -s rsassa -p 123456 -c ak_rsa.ctx -u ak_rsa.pub -n ak_rsa.name
loaded-key:
  name: 000c3e9c99092f2eb88e158eaaaa454fa45c3e33bb8bed3a42f8fd9849bbe95ed755cbb70513c50309b6d0c281e4b4d51f84
  qualified name: 000c4cb4c6b23ccc1c2893564ea96bc6fc6c33c3fceef821b44a04c3ca66300083ca69c1aba0248b8053aede35385909ef40
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
  privkey:   RSA 3072 bits
  keyid:     65:3b:d0:d1:6c:a6:d3:72:0c:c6:d2:2e:ac:e8:ec:ad:97:0c:8e:e5
  subjkey:   03:a3:d4:16:34:02:02:c8:25:ae:41:89:32:e8:90:72:9b:31:be:aa
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
                Public-Key: (3072 bit)
                Modulus:
                    00:83:b6:63:fa:e6:df:91:44:63:ee:96:4f:4a:83:
                    03:09:5f:0c:d7:55:40:eb:fb:17:bf:65:fc:33:8f:
                    d2:66:34:f1:1e:b4:d4:15:6b:4a:41:46:cd:91:a1:
                    37:c1:4a:71:1e:79:e2:e4:51:bc:46:27:ab:ce:c1:
                    0d:72:f4:3f:88:f1:fa:45:c3:43:e5:62:74:be:37:
                    87:a4:02:73:37:fa:37:ca:68:b8:fb:bd:29:1a:5a:
                    6d:04:bf:88:90:49:dd:98:e0:3b:5a:85:bf:d6:bd:
                    f1:70:83:b3:87:72:e3:d6:b9:16:66:28:8f:fd:8c:
                    36:1f:6a:94:dd:c3:69:7e:34:a9:71:93:6c:02:d3:
                    b3:7f:c5:d8:c3:ea:6e:b9:2a:17:b2:3c:66:2f:c2:
                    9b:b3:69:e5:82:96:3b:03:c7:fb:af:5c:41:a8:a3:
                    89:4a:8f:f3:eb:ca:53:5d:0b:c2:1e:55:cc:26:dd:
                    19:74:dc:24:62:a4:b9:13:ee:18:e6:98:b1:95:4e:
                    d8:5a:83:77:e5:d3:ac:52:33:86:af:b9:f5:f9:bb:
                    e7:4f:eb:fe:eb:72:02:6b:cb:de:70:cb:aa:9b:0b:
                    5a:c6:77:06:e7:7b:25:2e:08:65:4b:d2:e9:02:c4:
                    ad:d2:80:0b:e2:c5:9f:60:9e:78:60:f1:a7:e2:5c:
                    04:5d:bc:13:7e:17:cc:8a:52:20:34:32:d2:3c:1f:
                    8b:74:15:a7:31:d7:28:b6:59:00:7e:79:9c:ab:31:
                    06:93:19:20:fe:87:3b:4c:99:b6:b6:6b:1b:cf:67:
                    49:e2:7e:e0:05:84:0e:c5:c7:c3:4d:a1:a5:db:7c:
                    80:40:59:5c:ee:72:a5:cd:d7:b1:35:de:a5:12:61:
                    53:58:48:cc:c1:e0:6e:7f:e9:7a:f9:88:0c:b8:0e:
                    bd:5f:f3:37:62:82:83:bf:43:1f:83:26:e5:b9:46:
                    ee:97:54:cf:55:03:78:fe:fb:88:db:b2:74:81:e1:
                    2d:01:90:2d:91:4b:be:60:53:bf
                Exponent: 65537 (0x10001)
        Attributes:
            Requested Extensions:
                X509v3 Subject Alternative Name: 
                    email:user@strongswan.org
    Signature Algorithm: sha256WithRSAEncryption
    Signature Value:
        42:46:47:8f:25:21:60:a0:f1:c8:d4:aa:98:b6:cf:10:ca:6a:
        96:3b:3c:b6:b3:b1:85:ef:27:ce:f6:ea:18:9d:39:2c:90:d1:
        de:35:08:55:64:87:58:86:fe:96:fc:11:6f:53:39:1c:b7:33:
        61:b4:0f:e9:4f:a4:43:e9:16:7d:b9:b5:3d:3d:8a:2a:55:27:
        af:b6:40:84:44:58:a5:7e:ea:a8:77:37:fb:51:4d:ee:8d:83:
        fa:66:35:d1:2e:a0:f7:65:c6:7f:7e:09:6e:e9:55:d8:2d:43:
        ba:14:42:43:f2:ab:3e:b2:fd:5a:e4:79:9d:2a:7c:00:4a:93:
        f7:5e:4f:c8:12:63:1f:52:91:f9:dc:86:ff:44:5a:cd:74:ba:
        d1:d5:ef:4c:01:7c:92:f7:56:ff:af:34:50:ed:53:b9:df:a3:
        0f:a5:94:2e:f7:a4:cf:13:b0:c1:05:55:71:9d:f1:40:cf:78:
        15:d5:81:d2:46:64:68:f9:e1:e9:2e:49:4b:37:19:ed:36:e4:
        e9:90:87:5f:08:e4:6f:98:03:27:2c:f2:94:c5:cc:74:e8:75:
        bf:c5:dc:03:0a:c8:2d:53:83:f4:f1:71:f3:3f:66:2e:bc:ad:
        68:3a:bd:08:f9:47:19:54:e3:f2:4f:ca:0e:79:a3:0e:1e:88:
        0d:00:72:1f:ff:f7:30:ad:a4:28:ce:2d:48:8b:2e:41:3b:fe:
        17:af:d2:3e:17:e1:73:16:cc:83:14:a9:52:4d:f8:c2:0e:e9:
        52:77:62:18:c5:1f:c0:c6:54:55:26:8b:cd:fa:6c:a9:a4:f4:
        9b:20:22:26:e5:64:3a:bf:7c:2b:7a:17:23:c7:5c:a1:03:89:
        83:f9:95:7e:fa:16:9c:68:b5:07:0c:cb:b9:c8:7f:91:56:62:
        a2:f4:f7:fd:fb:57:6a:2f:7e:9e:6d:76:51:f2:10:28:1a:7c:
        99:a7:d6:a1:96:87:04:1c:22:86:b7:d5:92:11:e5:e7:1f:e4:
        67:9f:73:b8:72:22
```

### RSA Certification Authority

The strongSwan `pki` tool can be used to generate a simple *Certification Authority* `CA`.  First a  private `CA` key is generated
```console
# pki --gen --type rsa --size 3072 --outform pem > My_RSA_CaKey.pem
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
  validity:  not before Oct 10 05:37:43 2022, ok
             not after  Oct 09 05:37:43 2032, ok (expires in 3651 days)
  serial:    60:75:a8:65:0e:26:b1:70
  flags:     CA CRLSign self-signed 
  subjkeyId: 03:2c:77:a4:e4:9f:0e:61:cb:cc:6d:c9:11:77:ba:ca:58:3a:a7:ef
  pubkey:    RSA 3072 bits
  keyid:     01:00:36:18:39:cb:28:69:db:06:cc:f3:7c:14:40:e1:71:15:e6:c1
  subjkey:   03:2c:77:a4:e4:9f:0e:61:cb:cc:6d:c9:11:77:ba:ca:58:3a:a7:ef
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
  validity:  not before Oct 10 05:39:04 2022, ok
             not after  Oct 10 05:39:04 2027, ok (expires in 1825 days)
  serial:    1a:01:95:6d:7d:9d:ae:7f
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: 03:2c:77:a4:e4:9f:0e:61:cb:cc:6d:c9:11:77:ba:ca:58:3a:a7:ef
  subjkeyId: 03:a3:d4:16:34:02:02:c8:25:ae:41:89:32:e8:90:72:9b:31:be:aa
  pubkey:    RSA 3072 bits
  keyid:     65:3b:d0:d1:6c:a6:d3:72:0c:c6:d2:2e:ac:e8:ec:ad:97:0c:8e:e5
  subjkey:   03:a3:d4:16:34:02:02:c8:25:ae:41:89:32:e8:90:72:9b:31:be:aa
```

### RSA Attestation Certificate

In order to protect the `AK` certificate received from the `CA` from accidental deletion from the system harddisk we want to store it in the *Non-Volatile* `NV` memory of the TPM. To allocate the required amount of  `NV` memory we determine the exact size of the binary-encoded X.509 certificate
```console
ls -l ak_rsa_Cert.der 
-rw-r--r-- 1 root root 1098 Oct 10 05:39 ak_rsa_Cert.der
```
and then allocate 842 bytes of `NV` memory under  the handle `0x01800003`
```console
# tpm2_nvdefine 0x01800003 -C o -s 1098 -a 0x2000A
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
  validity:  not before Oct 10 05:39:04 2022, ok
             not after  Oct 10 05:39:04 2027, ok (expires in 1825 days)
  serial:    1a:01:95:6d:7d:9d:ae:7f
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: 03:2c:77:a4:e4:9f:0e:61:cb:cc:6d:c9:11:77:ba:ca:58:3a:a7:ef
  subjkeyId: 03:a3:d4:16:34:02:02:c8:25:ae:41:89:32:e8:90:72:9b:31:be:aa
  pubkey:    RSA 3072 bits
  keyid:     65:3b:d0:d1:6c:a6:d3:72:0c:c6:d2:2e:ac:e8:ec:ad:97:0c:8e:e5
  subjkey:   03:a3:d4:16:34:02:02:c8:25:ae:41:89:32:e8:90:72:9b:31:be:aa
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
# tpm2_createak -C 0x81010002 -G ecc384 -g sha384 -s ecdsa -c ak_ecc.ctx -u ak_ecc.pub -n ak_ecc.name
loaded-key:
  name: 000c1713877420a369483d20fa56e760ba65494e11a08a8423eed893cd718753b3fdf810ac58b8aae22fba121f26d479942e
  qualified name: 000cb9ee290bdd27aef214c3947c74b3f81daf1b30da19ac4560dacf4fe6b0d10496ad718b83e1c2b730f438c4a8cbfdfc9b
# tpm2_evictcontrol -C o -c ak_ecc.ctx 0x81010004
persistent-handle: 0x81010004
action: persisted
```
The `pki` command shows that an ECDSA signature key has been generated
```console
# pki --print --type priv --keyid 0x81010004
TPM 2.0 via TSS2 v2 available
signature algorithm is ECDSA with SHA384 hash
  privkey:   ECDSA 384 bits
  keyid:     fa:6a:57:e1:f7:c7:c0:2e:d3:56:d6:e5:13:4f:bb:87:ea:10:a6:66
  subjkey:   52:b0:c9:b7:62:43:da:c3:b7:60:4d:d9:d2:09:5b:55:b6:a6:46:53
```
In order to obtain an X.509 certificate issued and signed by a trusted *Certification Authority* `CA` we again generate a `PKCS#10` *Certificate Request* signed by the private `AK` stored in the TPM.
```console
# pki --req --keyid 0x81010004 --outform pem --dn "C=CH, O=Cyber, OU=user@strongswan.org" --san user@strongswan.org > ak_ecc_Req.pem
TPM 2.0 via TSS2 v2 available
signature algorithm is ECDSA with SHA384 hash
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
                Public-Key: (384 bit)
                pub:
                    04:40:02:7a:16:58:bc:29:95:6c:81:86:0b:3f:ce:
                    8b:1c:1e:03:64:06:ea:e5:41:62:4f:85:d5:b0:8d:
                    92:f1:bf:de:59:0e:ed:8b:6e:cd:c3:b3:d0:88:66:
                    06:89:2c:c0:99:c8:89:82:16:2d:5e:e3:b5:09:27:
                    28:e2:51:c8:e3:32:6d:29:7d:44:03:f4:5d:1c:f9:
                    65:ef:8f:88:ce:7d:0a:06:11:ea:2c:93:3a:f1:8e:
                    a3:f6:5e:bc:61:48:94
                ASN1 OID: secp384r1
                NIST CURVE: P-384
        Attributes:
            Requested Extensions:
                X509v3 Subject Alternative Name: 
                    email:user@strongswan.org
    Signature Algorithm: ecdsa-with-SHA384
    Signature Value:
        30:66:02:31:00:8f:18:54:f2:c5:a8:03:73:40:84:cc:26:0d:
        d9:b7:fa:8d:49:03:4c:17:81:bf:18:97:5d:e6:8b:36:9f:e3:
        bf:72:ab:aa:8f:40:d9:42:19:34:53:1c:1c:84:80:2c:95:02:
        31:00:8e:b0:e7:0e:da:26:3d:0e:98:0c:cd:d6:42:4b:bc:c3:
        e2:4f:15:0b:a4:d6:7d:ec:1a:cd:4c:00:f4:ef:5e:3b:12:14:
        ce:65:36:bc:b9:c1:93:38:e5:28:c5:f7:7a:59
```

### ECC Certification Authority

In order to issue ECC end entity certificates we create a *Certification Authority* of our own. We start be generating a private ECC key
```console
# pki --gen --type ecdsa --size 384 --outform pem > My_ECC_CaKey.pem
```
and use it to construct a self-signed `CA` certiificate
```console
# pki --self --ca --type ecdsa --in My_ECC_CaKey.pem --dn "C=CH, O=Cyber, CN=My ECC CA" --lifetime 3652 --outform pem > My_ECC_CaCert.pem
```
listed below
```console
# pki --print --type x509 --in My_ECC_CaCert.pem 
  subject:  "C=CH, O=Cyber, CN=My ECC CA"
  issuer:   "C=CH, O=Cyber, CN=My ECC CA"
  validity:  not before Oct 10 05:52:01 2022, ok
             not after  Oct 09 05:52:01 2032, ok (expires in 3651 days)
  serial:    2d:eb:fa:2c:b4:f7:df:8c
  flags:     CA CRLSign self-signed 
  subjkeyId: 24:be:9a:27:51:71:e4:79:63:37:a1:02:e8:51:30:4b:65:19:58:59
  pubkey:    ECDSA 384 bits
  keyid:     f3:50:0c:ab:33:2f:af:87:aa:72:69:f8:95:26:2d:8c:6c:69:91:ae
  subjkey:   24:be:9a:27:51:71:e4:79:63:37:a1:02:e8:51:30:4b:65:19:58:59
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
  validity:  not before Oct 10 05:52:51 2022, ok
             not after  Oct 10 05:52:51 2027, ok (expires in 1825 days)
  serial:    09:12:94:3b:ce:3a:9f:f4
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: 24:be:9a:27:51:71:e4:79:63:37:a1:02:e8:51:30:4b:65:19:58:59
  subjkeyId: 52:b0:c9:b7:62:43:da:c3:b7:60:4d:d9:d2:09:5b:55:b6:a6:46:53
  pubkey:    ECDSA 384 bits
  keyid:     fa:6a:57:e1:f7:c7:c0:2e:d3:56:d6:e5:13:4f:bb:87:ea:10:a6:66
  subjkey:   52:b0:c9:b7:62:43:da:c3:b7:60:4d:d9:d2:09:5b:55:b6:a6:46:53
```

### ECC Attestation Certificate

We again determine the exact size of the binary-encoded X.509 certificate
```console
# ls -l ak_ecc_Cert.der 
-rw-r--r-- 1 root root 508 Oct 10 05:52 ak_ecc_Cert.der
```
Next we allocate 445 bytes needed by the X.509 certificate in the NV-RAM
```console
# tpm2_nvdefine 0x01800004 -C o -s 508 -a 0x2000A
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
  name: 000b2a068cdb1717d19392ee7125a527064d7370b8c3a21b41327dbfe0672cf95547
  hash algorithm:
    friendly: sha256
    value: 0xB
  attributes:
    friendly: ownerwrite|policywrite|ownerread|written
    value: 0x2002000A
  size: 1098

0x1800004:
  name: 000b0a7c324108965e93c98559cc5f758739a3f4107bd85a94416fa50eb59a639724
  hash algorithm:
    friendly: sha256
    value: 0xB
  attributes:
    friendly: ownerwrite|policywrite|ownerread|written
    value: 0x2002000A
  size: 508
```
The `pki` tool can read the X.509 certificate stored in the TPM
```console
# pki --print --type x509 --keyid 0x01800004
TPM 2.0 via TSS2 v2 available
loaded certificate from TPM NV index 0x01800004
  subject:  "C=CH, O=Cyber, OU=user@strongswan.org"
  issuer:   "C=CH, O=Cyber, CN=My ECC CA"
  validity:  not before Oct 10 05:52:51 2022, ok
             not after  Oct 10 05:52:51 2027, ok (expires in 1825 days)
  serial:    09:12:94:3b:ce:3a:9f:f4
  altNames:  user@strongswan.org
  flags:     clientAuth 
  authkeyId: 24:be:9a:27:51:71:e4:79:63:37:a1:02:e8:51:30:4b:65:19:58:59
  subjkeyId: 52:b0:c9:b7:62:43:da:c3:b7:60:4d:d9:d2:09:5b:55:b6:a6:46:53
  pubkey:    ECDSA 384 bits
  keyid:     fa:6a:57:e1:f7:c7:c0:2e:d3:56:d6:e5:13:4f:bb:87:ea:10:a6:66
  subjkey:   52:b0:c9:b7:62:43:da:c3:b7:60:4d:d9:d2:09:5b:55:b6:a6:46:53
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

