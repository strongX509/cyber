# IP Security (IPsec)

## Table of Contents
1. [Encapsulating Security Payload (ESP)](#section1)
2. [Internet Key Exchange Version 2 (IKEv2)](#section2)
3. [strongSwan VPN Solution](#section3)
4. [Public Key Infrastructure (PKI)](#section4)

strongSwan exercise: &nbsp; [strongSwan1](#strongswan1) 

## Encapsulating Security Payload (ESP) <a name="section1"></a>

The *Encapsulationg Security Payload (ESP)* protocol defined in [RFC 4303][RFC_4303]
allows the encryption of *IP packets* on the network layer carrying e.g. layer 4 TCP traffic.

![IP Packet][IP_PACKET]

In *IPsec tunnel* mode the complete IP packet is encapsulated by ESP and an outer IP header is prepended:

![ESP_Packet][ESP_PACKET]

An ESP packet consists of an ESP header, the encrypted IP payload body and an ESP trailer needed for padding.
The *Authentication Data* field appended at the end as a cryptographic checksum guarantees *data integrity*:

![ESP Header/Body/Trailer][ESP_HEADER]

[RFC_4303]: https://tools.ietf.org/html/rfc4303
[IP_PACKET]:  IP_Packet_280.png
[ESP_PACKET]: ESP_Packet_600.png
[ESP_HEADER]: ESP_Header_500.png

## Internet Key Exchange Version 2 (IKEv2) <a name="section2"></a>

Version 2 of the Internet Key Exchange (IKEv2) protocol defined in [RFC 7296][RFC_7296] manages the setup of IPsec connections. The IKEv2 auxiliary protocol uses UDP datagrams with both source and destination ports set to the well-known port UDP 500. 

### IKE_SA_INIT request/response

The *Initiator* starts the negotiation be sending an `IKE_SA_INIT` request which is answered by the *Responder* with an `IKE_SA_INIT`response.

![IKE_SA_INIT request/response pair][IKE_SA_INIT]

If the *Responder* comes to the conclusion that it is under a *Denial of Service* attack, then it can request a *Cookie* from the *Initiator*  before sending the computationally expensive *Key Exchange* `KE` payload in the `IKE_SA_INIT`response.

![IKE_SA_INIT with cookie][IKE_COOKIE]

Based on the exchange of the *Key Exchange* `KE` and *Nonces* `N`payloads in `IKE_SA_INIT` both endpoints can derive a *Shared Secret* which allows them to encrypt all following IKE messages based on the `IKE_SA` established via the `SA1i`and `SA1r` *Security Association* payloads.

### IKE_AUTH request/response

In the `ÌKE_AUTH` request the *Initiator* authenticates itself by sending its identity `IDi` and a *Digital Signature* in the `AUTHi` payload accompanied by an optional *Certificate* `CERTi`. The *Responder* verifies the validty and trustworthiness of the received end entity certificate by going up the X.509 trust chain until a locally stored Root CA certificate is reached.

Additionally the *Initiator* sends a *Security Association* proposal `SA2i`and a set of *Traffic Selectors `TSi`and `TSr` to be used for the first `CHILD_SA`.

![IKE_AUTH request/response pair][IKE_AUTH]

The *Responder* authenticates itself in turn with the `IKE_AUTH` response and includes a selected *Security Association* `SA2r` proposal and a possibly *narrowed* set of *Traffic Selectors* `TSi`and `TSr`. With this information the `CHILD_SA` defining the encryption and data integrity of the IPsec payload packets can be installed and activated.

### CREATE_CHILD_SA request/response

`CREATE_CHILD_SA` request/response pairs are used to negotiate additional `CHILD_SAs` or to do the periodic rekeying of either the `IKE_SA` or the `CHILD_SAs`. 

![CREATE_CHILD_SA request/response pair][CREATE_CHILD_SA]

Without the `N(REKEY_SA)` notification the `IKE_SA` is rekeyed, the fresh `KE` *Key Exchange* payloads guaranteeing *Perfect Forward Secrecy* (PFS). With a `N(REKEY_SA)` notification included a `CHILD_SA` is rekeyed, the `KE`exchange being optional.

[RFC_7296]: https://tools.ietf.org/html/rfc7296
[IKE_SA_INIT]: IKE_SA_INIT_625.png
[IKE_COOKIE]:  IKE_COOKIE_630.png
[IKE_AUTH]:    IKE_AUTH_665.png
[CREATE_CHILD_SA]: CREATE_CHILD_SA_650.png

## strongSwan VPN Solution <a name="section3"></a>

Using the [strongSwan][STRONGSWAN] open source VPN solution we want to set up an IPsec tunnel based on IKEv2 authentication using X.509 certificates.

**strongSwan 1**: <a name="strongswan1"></a>  First we install the `strongx509/strongswan` docker image, clone the `docker-compose` environment and bring the containers up:

```console
$ git clone https://github.com/strongX509/docker.git
$ cd docker/strongswan
$ docker pull strongx509/strongswan
$ docker-compose up
Creating vpn-server ... done
Creating vpn-client ... done
Attaching to vpn-server, vpn-client
```
strongSwan options can be configured in the `/etc/strongswan.conf` file which in our case just contains the startup scripts and a logging directive diverting the debug output to `stderr`.
```console
charon {
   start-scripts {
      creds = swanctl --load-creds
      conns = swanctl --load-conns
      pools = swanctl --load-pools
   }
   filelog {
      stderr {
         default = 1
      }
   }
}
```

### Network Topology

The network topology that has been created looks as follows:
```
               +------------+                        +------------+
  10.3.0.1 --- | VPN Client | === 192.168.0.0/24 === | VPN Server | --- 10.1.0.0/16 
 Virtual IP    +------------+ .3     Internet     .2 +------------+ .2    Intranet
```
A VPN client with IP address 192.168.0.3 connects over the *Internet* to a VPN server with IP address 192.168.0.2 in order to access the *Intranet* 10.1.0.0/24. Within the IPsec tunnel the VPN client is using the *Virtual IP* address 10.3.0.1 assigned via IKEv2 by the VPN server.

The following command shows the two network bridges that have been created
```console
$ ip route list | grep br
10.1.0.0/16    dev br-6cc6a0c4ddf5 proto kernel scope link src 10.1.0.1
192.168.0.0/24 dev br-db882ffcce1f proto kernel scope link src 192.168.0.1
```
Now we connect `wireshark` to the `br-db882ffcce1f`bridge which implements the `192.168.0.0/24` network. The capture filter will catch `ESP` (proto 50), `IKE` (port 500), `NAT-T` (port 4500) and `ICMP` packets:

![Wireshark 1][WIRESHARK_1]

[WIRESHARK_1]: Wireshark1_500.png

### VPN Server

In a second console window we connect with the `vpn-server`docker container using a `bash`shell. We start the strongSwan `charon`daemon in the background which after loading its plugins spawns 16 worker threads.
```console
server$ docker exec -ti vpn-server /bin/bash
server# ./charon &
00[DMN] Starting IKE charon daemon (strongSwan 5.8.2, Linux 5.4.0-kali3-amd64, x86_64)
00[LIB] loaded plugins: charon random nonce x509 constraints pubkey pkcs1 pkcs8 pkcs12 pem openssl drbg kernel-netlink socket-default vici updown eap-identity eap-md5 eap-tls
00[JOB] spawning 16 worker threads
```
Next the root CA certificate, the server certificate and a matching ECDSA private key are loaded along with a couple of pre-shared secrets 
```console
00[DMN] executing start script 'creds' (swanctl --load-creds)
15[CFG] loaded certificate 'C=CH, O=Cyber, CN=server.strongswan.org'
07[CFG] loaded certificate 'C=CH, O=Cyber, CN=Cyber Root CA'
11[CFG] loaded ECDSA private key
16[CFG] loaded IKE shared key with id 'ike-jane' for: 'jane@strongswan.org'
07[CFG] loaded IKE shared key with id 'ike-hacker' for: 'hacker@strongswan.org'
12[CFG] loaded EAP shared key with id 'eap-jane' for: 'jane'
16[CFG] loaded EAP shared key with id 'eap-hacker' for: 'hacker'
00[DMN] creds: loaded certificate from '/etc/swanctl/x509/serverCert.pem'
00[DMN] creds: loaded certificate from '/etc/swanctl/x509ca/caCert.pem'
00[DMN] creds: loaded ecdsa key from '/etc/swanctl/ecdsa/serverKey.pem'
00[DMN] creds: loaded ike secret 'ike-jane'
00[DMN] creds: loaded ike secret 'ike-hacker'
00[DMN] creds: loaded eap secret 'eap-jane'
00[DMN] creds: loaded eap secret 'eap-hacker'
```
then three different VPN connection definitions are loaded
```console
00[DMN] executing start script 'conns' (swanctl --load-conns)
16[CFG] added vici connection: rw
08[CFG] added vici connection: psk
12[CFG] added vici connection: eap
00[DMN] conns: loaded connection 'rw'
00[DMN] conns: loaded connection 'psk'
00[DMN] conns: loaded connection 'eap'
00[DMN] conns: successfully loaded 3 connections, 0 unloaded
```
and finally a *Virtual IP* address pool is defined
```console
00[DMN] executing start script 'pools' (swanctl --load-pools)
13[CFG] added vici pool rw_pool: 10.3.0.0, 254 entries
00[DMN] pools: loaded pool 'rw_pool'
00[DMN] pools: successfully loaded 1 pools, 0 unloaded
```
The loaded server and root CA certificates can be viewed with the command
```console
server# swanctl --list-certs
```
```console
List of X.509 End Entity Certificates
  subject:  "C=CH, O=Cyber, CN=server.strongswan.org"
  issuer:   "C=CH, O=Cyber, CN=Cyber Root CA"
  validity:  not before Mar 09 12:03:38 2020, ok
             not after  Mar 09 12:03:38 2024, ok (expires in 1458 days)
  serial:    5a:91:0a:7e:c8:7e:42:a6
  altNames:  server.strongswan.org
  flags:     serverAuth 
  authkeyId: b8:d2:60:8a:87:cf:48:ee:71:5d:d8:51:20:23:5a:c1:aa:e5:f7:b2
  subjkeyId: 88:b3:45:15:b5:53:3a:86:86:ad:3f:44:67:ef:c5:23:d1:72:2c:eb
  pubkey:    ECDSA 384 bits, has private key
  keyid:     a4:4c:74:b5:bc:96:a3:19:78:88:ee:c6:7a:2d:7f:aa:f7:8a:90:27
  subjkey:   88:b3:45:15:b5:53:3a:86:86:ad:3f:44:67:ef:c5:23:d1:72:2c:eb
```
```console
List of X.509 CA Certificates
  subject:  "C=CH, O=Cyber, CN=Cyber Root CA"
  issuer:   "C=CH, O=Cyber, CN=Cyber Root CA"
  validity:  not before Mar 09 12:02:09 2020, ok
             not after  Mar 09 12:02:09 2030, ok (expires in 3649 days)
  serial:    59:6a:63:a9:e2:d9:f4:af
  flags:     CA CRLSign self-signed 
  subjkeyId: b8:d2:60:8a:87:cf:48:ee:71:5d:d8:51:20:23:5a:c1:aa:e5:f7:b2
  pubkey:    ECDSA 384 bits
  keyid:     44:63:01:6f:1c:45:00:89:b2:6d:26:76:cf:55:70:d8:b6:33:d3:28
  subjkey:   b8:d2:60:8a:87:cf:48:ee:71:5d:d8:51:20:23:5a:c1:aa:e5:f7:b2
```
and the loaded server-side connection definitions with
```console
server# swanctl --list-conns
```
```console
rw: IKEv2, no reauthentication, rekeying every 14400s, dpd delay 60s
  local:  %any
  remote: %any
  local public key authentication:
    id: server.strongswan.org
    certs: C=CH, O=Cyber, CN=server.strongswan.org
  remote public key authentication:
    cacerts: C=CH, O=Cyber, CN=Cyber Root CA
  net: TUNNEL, rekeying every 3600s, dpd action is clear
    local:  10.1.0.0/24
    remote: dynamic
  host: TUNNEL, rekeying every 3600s, dpd action is clear
    local:  dynamic
    remote: dynamic
```
```console
psk: IKEv2, no reauthentication, rekeying every 14400s, dpd delay 60s
  local:  %any
  remote: %any
  local pre-shared key authentication:
    id: server.strongswan.org
  remote pre-shared key authentication:
  psk: TUNNEL, rekeying every 3600s, dpd action is clear
    local:  10.1.0.0/24
    remote: dynamic
```
```console
eap: IKEv2, no reauthentication, rekeying every 14400s, dpd delay 60s
  local:  %any
  remote: %any
  local public key authentication:
    id: server.strongswan.org
    certs: C=CH, O=Cyber, CN=server.strongswan.org
  remote EAP_MD5 authentication:
    eap_id: %any
  eap: TUNNEL, rekeying every 3600s, dpd action is clear
    local:  10.1.0.0/24 192.168.0.2/32
    remote: dynamic
```

### VPN client

In a third console window we connect with the `vpn-client`docker container using a `bash`shell. We start the strongSwan `charon`daemon in the background which after loading its plugins spawns 16 worker threads.
```console
client$ docker exec -ti vpn-client /bin/bash
client# ./charon &
00[DMN] Starting IKE charon daemon (strongSwan 5.8.2, Linux 5.4.0-kali3-amd64, x86_64)
00[LIB] loaded plugins: charon random nonce x509 constraints pubkey pkcs1 pkcs8 pkcs12 pem openssl drbg kernel-netlink socket-default vici updown eap-identity eap-md5 eap-tls
00[JOB] spawning 16 worker threads
```
```console
00[DMN] executing start script 'creds' (swanctl --load-creds)
01[CFG] loaded certificate 'C=CH, O=Cyber, CN=client.strongswan.org'
07[CFG] loaded certificate 'C=CH, O=Cyber, CN=Cyber Root CA'
16[CFG] loaded ECDSA private key
11[CFG] loaded IKE shared key with id 'ike-hacker' for: 'hacker@strongswan.org'
08[CFG] loaded EAP shared key with id 'eap-hacker' for: 'hacker'
00[DMN] creds: loaded certificate from '/etc/swanctl/x509/clientCert.pem'
00[DMN] creds: loaded certificate from '/etc/swanctl/x509ca/caCert.pem'
00[DMN] creds: loaded ecdsa key from '/etc/swanctl/ecdsa/clientKey.pem'
00[DMN] creds: loaded ike secret 'ike-hacker'
00[DMN] creds: loaded eap secret 'eap-hacker'
```
```console
00[DMN] executing start script 'conns' (swanctl --load-conns)
14[CFG] added vici connection: home
07[CFG] added vici connection: psk
16[CFG] added vici connection: eap
00[DMN] conns: loaded connection 'home'
00[DMN] conns: loaded connection 'psk'
00[DMN] conns: loaded connection 'eap'
00[DMN] conns: successfully loaded 3 connections, 0 unloaded
```
The loaded client and root CA certificates can be viewed with the command
```console
client# swanctl --list-certs
```
```console
List of X.509 End Entity Certificates
  subject:  "C=CH, O=Cyber, CN=client.strongswan.org"
  issuer:   "C=CH, O=Cyber, CN=Cyber Root CA"
  validity:  not before Mar 09 13:59:06 2020, ok
             not after  Mar 09 13:59:06 2024, ok (expires in 1458 days)
  serial:    0a:60:29:9a:71:f1:6e:95
  altNames:  client.strongswan.org
  flags:     clientAuth 
  authkeyId: b8:d2:60:8a:87:cf:48:ee:71:5d:d8:51:20:23:5a:c1:aa:e5:f7:b2
  subjkeyId: 2e:f4:4d:08:0c:3d:0b:a9:b6:3d:5e:1e:b3:a3:01:40:3d:65:91:16
  pubkey:    ECDSA 384 bits, has private key
  keyid:     9e:8e:2d:e7:37:52:c0:24:81:09:39:29:11:4d:d5:57:b8:94:db:47
  subjkey:   2e:f4:4d:08:0c:3d:0b:a9:b6:3d:5e:1e:b3:a3:01:40:3d:65:91:16
```
```console
List of X.509 CA Certificates
  subject:  "C=CH, O=Cyber, CN=Cyber Root CA"
  issuer:   "C=CH, O=Cyber, CN=Cyber Root CA"
  validity:  not before Mar 09 12:02:09 2020, ok
             not after  Mar 09 12:02:09 2030, ok (expires in 3649 days)
  serial:    59:6a:63:a9:e2:d9:f4:af
  flags:     CA CRLSign self-signed 
  subjkeyId: b8:d2:60:8a:87:cf:48:ee:71:5d:d8:51:20:23:5a:c1:aa:e5:f7:b2
  pubkey:    ECDSA 384 bits
  keyid:     44:63:01:6f:1c:45:00:89:b2:6d:26:76:cf:55:70:d8:b6:33:d3:28
  subjkey:   b8:d2:60:8a:87:cf:48:ee:71:5d:d8:51:20:23:5a:c1:aa:e5:f7:b2
```
and the loaded client-side connection definitions with
```console
client# swanctl --list-conns
```
```console
home: IKEv2, no reauthentication, rekeying every 14400s, dpd delay 60s
  local:  %any
  remote: 192.168.0.2
  local public key authentication:
    id: client.strongswan.org
    certs: C=CH, O=Cyber, CN=client.strongswan.org
  remote public key authentication:
    id: server.strongswan.org
  net: TUNNEL, rekeying every 3600s, dpd action is hold
    local:  dynamic
    remote: 10.1.0.0/16
  host: TUNNEL, rekeying every 3600s, dpd action is hold
    local:  dynamic
    remote: dynamic
```
```console
psk: IKEv2, no reauthentication, rekeying every 14400s, dpd delay 60s
  local:  %any
  remote: 192.168.0.2
  local pre-shared key authentication:
    id: hacker@strongswan.org
  remote pre-shared key authentication:
    id: server.strongswan.org
  psk: TUNNEL, rekeying every 3600s, dpd action is hold
    local:  dynamic
    remote: 10.1.0.0/16
```
```console
eap: IKEv2, no reauthentication, rekeying every 14400s, dpd delay 60s
  local:  %any
  remote: 192.168.0.2
  local EAP_MD5 authentication:
    eap_id: hacker
  remote public key authentication:
    id: server.strongswan.org
  eap: TUNNEL, rekeying every 3600s, dpd action is hold
    local:  dynamic
    remote: 10.1.0.0/16 192.168.0.2/32
```
## Public Key Infrastructure <a name="section4"></a>

The public key pairs and the corresponding X.509 certificates needed in the previous section were generated using the powerful strongSwan [pki][PKI] tool. The signature keys are based on 384 bit ellliptic curves which guarantee a security strength of 192 bits (at least as long as no practical quantum computer exists).

### Root CA Certificate

We generate the 384 bit ECDSA Root CA private key
```console
pki --gen --type ecdsa --size 384 --outform pem > caKey.pem
```
and a matching self-signed Root CA certificate
```console
pki --self --type ecdsa --in caKey.pem --ca --lifetime 3652 \
    --dn "C=CH, O=Cyber, CN=Cyber Root CA"                  \
    --outform pem > caCert.pem
```
### Server Certificate

We generate the 384 bit ECDSA Server private key
```console
pki --gen --type ecdsa --size 384 --outform pem > serverKey.pem
```
and a matching Server certificate signed by the Root CA
```console
pki --issue --cacert caCert.pem --cakey caKey.pem   \
    --type ecdsa --in serverKey.pem --lifetime 1461 \
    --dn "C=CH, O=Cyber, CN=server.strongswan.org"  \
    --san server.strongswan.org --flag serverAuth   \
    --outform pem > serverCert.pem
```
Many commercial VPN clients (e.g. Windows 10) require the `serverAuthentication` *Extended Key Usage (EKU)* flag to be set in server certificates. 

A private key can be printed with the command
```console
pki --print --type ecdsa --in serverKey.pem
  privkey:   ECDSA 384 bits
  keyid:     a4:4c:74:b5:bc:96:a3:19:78:88:ee:c6:7a:2d:7f:aa:f7:8a:90:27
  subjkey:   88:b3:45:15:b5:53:3a:86:86:ad:3f:44:67:ef:c5:23:d1:72:2c:eb
```
and a X.509 certificate with
```console
pki --print --type x509 --in /etc/swanctl/x509/serverCert.pem
  subject:  "C=CH, O=Cyber, CN=server.strongswan.org"
  issuer:   "C=CH, O=Cyber, CN=Cyber Root CA"
  validity:  not before Mar 09 12:03:38 2020, ok
             not after  Mar 09 12:03:38 2024, ok (expires in 1454 days)
  serial:    5a:91:0a:7e:c8:7e:42:a6
  altNames:  server.strongswan.org
  flags:     serverAuth
  authkeyId: b8:d2:60:8a:87:cf:48:ee:71:5d:d8:51:20:23:5a:c1:aa:e5:f7:b2
  subjkeyId: 88:b3:45:15:b5:53:3a:86:86:ad:3f:44:67:ef:c5:23:d1:72:2c:eb
  pubkey:    ECDSA 384 bits
  keyid:     a4:4c:74:b5:bc:96:a3:19:78:88:ee:c6:7a:2d:7f:aa:f7:8a:90:27
  subjkey:   88:b3:45:15:b5:53:3a:86:86:ad:3f:44:67:ef:c5:23:d1:72:2c:eb
```
### Client Certificate

We generate the 384 bit ECDSA Client private key
```console
pki --gen --type ecdsa --size 384 --outform pem > clientKey.pem
```
and a matching Server certificate signed by the Root CA
```console
pki --issue --cacert caCert.pem --cakey caKey.pem    \
     --type ecdsa --in clientKey.pem --lifetime 1461 \
     --dn "C=CH, O=Cyber, CN=client.strongswan.org"  \
     --san client.strongswan.org --flag clientAuth   \
     --outform pem > clientCert.pem
```
The use of the `clientAuthentication` *Extended Key Usage (EKU)* flag in client certificates is voluntary.

[STRONGSWAN]: https://www.strongswan.org
[PKI]: https://wiki.strongswan.org/projects/strongswan/wiki/IpsecPKI

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
