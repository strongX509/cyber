# EAP-based Authentication 

## Table of Contents
1. [Configuration Files](#section1)
2. [Initiating Connection](#section2)
3. [Tunneling Traffic](#section3)
3. [Terminating Connection](#section4)
4. [Wireshark Trace](#section5)

strongSwan exercise: &nbsp; [strongSwan1](#strongswan1) 

## Configuration Files <a name="section1"></a>

### VPN Client Configuration

strongSwan connections are defined in `/etc/swanctl/swanctl.conf` which in turn can include other configuration files:
```console
connections {
   include home.conf
   include psk.conf
   include eap.conf
   include eap-tls.conf
}

secrets {
   eap-hacker {
      id = hacker
      secret = K8FW9/N0VIAJ
   }
}

```
The client-side connection definition for authentication based on the  *Extended Authentication Protocol*  (EAP) is defined in `/etc/swanctl/eap.conf`
```console
eap {
   remote_addrs = 192.168.0.2
   vips = 0.0.0.0

   local {
      auth = eap-md5 
      eap_id = hacker
   }
   remote {
      auth = pubkey 
      id = server.strongswan.org
   }
   children {
      eap {
         remote_ts = 10.1.0.0/16,192.168.0.2

         esp_proposals = aes256gcm128-x25519
         dpd_action = trap 
      }
   }
   version = 2
   proposals = aes256-sha256-x25519
   dpd_delay = 60s
}
```
The remote traffic selector `TSr` is now a concatenation of the Intranet `10.1.0.0/16` and the Internet address `192.168.0.2` of the VPN server.

### VPN Server Configuration

The top-level configuration file is `/etc/swanctl/swanctl.conf` which also defines the pool used to allocate *Virtual IP* addresses and stores the *EAP* credentials of all remote access clients:

```console
connections {
   include rw.conf
   include psk.conf
   include eap.conf
}

pools {
   rw_pool {
      addrs = 10.3.0.0/24
   }
}

secrets {
   eap-jane {
      id = jane
      secret = 3s9RFGdWE5EW
   }
   eap-hacker {
      id = hacker
      secret = K8FW9/N0VIAJ
   }
}
```
The server-side connection definition for authentication based on the  *Extended Authentication Protocol* (EAP) is defined in `/etc/swanctl/eap.conf`
```console
eap {
   pools = rw_pool

   local {
      auth = pubkey
      certs = serverCert.pem
      id = server.strongswan.org
   }
   remote {
      auth = eap-dynamic
      eap_id = %any
   }
   children {
      eap {
         local_ts = 10.1.0.0/24,192.168.0.2

         esp_proposals = aes256gcm128-chacha20poly1305-x25519
         dpd_action = clear
      }
   }
   version = 2
   proposals = aes256-sha256-x25519
   dpd_delay = 60s
}
```
## Initiating Connection <a name="section2"></a>

**strongSwan 1**: <a name="strongswan1"></a> 

The  VPN client initiates the  `eap` CHILD_SA
```console
client# swanctl --initiate --child eap > /dev/null
10[CFG] vici initiate CHILD_SA 'eap'
14[IKE] initiating IKE_SA eap[1] to 192.168.0.2
```
The  VPN client is sending the `IKE_SA_INIT` request to the VPN server
```console
14[ENC] generating IKE_SA_INIT request 0 [ SA KE No N(NATD_S_IP) N(NATD_D_IP) N(FRAG_SUP) N(HASH_ALG) N(REDIR_SUP) ]
14[NET] sending packet: from 192.168.0.3[500] to 192.168.0.2[500] (240 bytes)
```
The `IKE_SA_INIT` response is received from the VPN server
```console
13[NET] received packet: from 192.168.0.2[500] to 192.168.0.3[500] (273 bytes)
13[ENC] parsed IKE_SA_INIT response 0 [ SA KE No N(NATD_S_IP) N(NATD_D_IP) CERTREQ N(FRAG_SUP) N(HASH_ALG) N(CHDLESS_SUP) N(MULT_AUTH) ]
```
The `SA` payload contains the selected IKE_SA` proposal
```console
13[CFG] selected proposal: IKE:AES_CBC_256/HMAC_SHA2_256_128/PRF_HMAC_SHA2_256/CURVE_25519
```
In the `CERTREQ` payload the VPN server requested a client certificate issued by the `Cyber Root CA` , as does the VPN client
```console
13[IKE] received cert request for "C=CH, O=Cyber, CN=Cyber Root CA"
13[IKE] sending cert request for "C=CH, O=Cyber, CN=Cyber Root CA"
```
No explicit IKEv2 identity is defined, therefore by default `IDi` is set to the host  IP address `192.168.0.3`
```console
13[CFG] no IDi configured, fall back on IP address
```
The `SA`, `TSi` and `TSr` payloads for the `CHILD_SA` `psk` are added
```console
13[IKE] establishing CHILD_SA eap{1}
```
The VPN client is sending the  `IKE_AUTH` request to the VPN server. No `AUTH`payload is sent, thereby signalling to the VPN server that EAP-based authentication is desired.

![IKE_AUTH request/response for EAP-based authentication][IKE_AUTH_EAP]
```console
13[ENC] generating IKE_AUTH request 1 [ IDi N(INIT_CONTACT) CERTREQ IDr CPRQ(ADDR) SA TSi TSr N(MOBIKE_SUP) N(NO_ADD_ADDR) N(MULT_AUTH) N(EAP_ONLY) N(MSG_ID_SYN_SUP) ]
13[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (304 bytes)
```
The first `IKE_AUTH` response is received from the VPN server
```console
05[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (752 bytes)
05[ENC] parsed IKE_AUTH response 1 [ IDr CERT AUTH EAP/REQ/ID ]
```
The certificate and the digital signature of the VPN server is verified
```console
05[IKE] received end entity cert "C=CH, O=Cyber, CN=server.strongswan.org"
05[CFG]   using certificate "C=CH, O=Cyber, CN=server.strongswan.org"
05[CFG]   using trusted ca certificate "C=CH, O=Cyber, CN=Cyber Root CA"
05[CFG]   reached self-signed root ca with a path length of 0
05[IKE] authentication of 'server.strongswan.org' with ECDSA_WITH_SHA384_DER successful
```
The VPN server requests the VPN client's `EAP IDENTITY` 
```console
05[IKE] server requested EAP_IDENTITY (id 0x00), sending 'hacker'
05[ENC] generating IKE_AUTH request 2 [ EAP/RES/ID ]
05[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (80 bytes)
```
The VPN server sends an `EAP-MD5` challenge
```console
14[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (96 bytes)
14[ENC] parsed IKE_AUTH response 2 [ EAP/REQ/MD5 ]
14[IKE] server requested EAP_MD5 authentication (id 0xB2)
```
The VPN client returns the `EAP-MD5` response
```console
14[ENC] generating IKE_AUTH request 3 [ EAP/RES/MD5 ]
14[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (96 bytes)
```
The `EAP-MD5`  authentication was successful
```console
13[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (80 bytes)
13[ENC] parsed IKE_AUTH response 3 [ EAP/SUCC ]
13[IKE] EAP method EAP_MD5 succeeded, no MSK established
```
The VPN client returns an`AUTH` payload
```console
13[IKE] authentication of '192.168.0.3' (myself) with EAP
13[ENC] generating IKE_AUTH request 4 [ AUTH ]
13[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (112 bytes)
```
The final `IKE_AUTH` response is received from the VPN server
```console
05[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (256 bytes)
05[ENC] parsed IKE_AUTH response 4 [ AUTH CPRP(ADDR) SA TSi TSr N(MOBIKE_SUP) N(ADD_4_ADDR) ]
05[IKE] authentication of 'server.strongswan.org' with EAP successful
```
The `IKE_SA` `eap`has been successfully established
```console
05[IKE] IKE_SA eap[1] established between 192.168.0.3[192.168.0.3]...192.168.0.2[server.strongswan.org]
```
The  VPN server proposed the time of the next `IKE_SA` rekeying
```console
05[IKE] scheduling rekeying in 12971s
05[IKE] maximum IKE_SA lifetime 14411s
```
The  VPN server has assigned a *Virtual IP* address to the VPN client
```console
05[IKE] installing new virtual IP 10.3.0.1
```
The `SA`, `TSi` and `TSr` payloads received in the `IKE_AUTH` response define the crypto parameters and traffic selectors of the `CHILD_SA` to be established.
```console
05[CFG] selected proposal: ESP:AES_GCM_16_256/NO_EXT_SEQ
05[IKE] CHILD_SA eap{1} established with SPIs c8f898c5_i cdb938a8_o and TS 10.3.0.1/32 === 10.1.0.0/24 192.168.0.2/32
05[IKE] peer supports MOBIKE
```

[RFC_4555]: https://tools.ietf.org/html/rfc4555
[IKE_AUTH_EAP]: IKE_AUTH_EAP_665.png

## Tunneling Traffic <a name="section3"></a>

The VPN client pings the VPN server on its Intranet address 10.1.0.2 twice. The source address of the IP packets leaving the client equals the *Virtual IP* 10.3.0.1.
```console
client# # ping -c 2 10.1.0.2
PING 10.1.0.2 (10.1.0.2) 56(84) bytes of data.
64 bytes from 10.1.0.2: icmp_seq=1 ttl=64 time=0.257 ms
64 bytes from 10.1.0.2: icmp_seq=2 ttl=64 time=0.275 ms
```
Lines 11-14 of the wireshark [ trace](#section5) show the encrypted ICMP messages as ESP packets with the SPIs of the `eap` `CHILD_SA`.

When the VPN client pings the VPN server on its Internet address 192.168.0.2 the packet is encrypted as well because the traffic selector includes the external server IP address as well 
```console
client# # ping -c 1 192.168.0.2
PING 192.168.0.2 (192.168.0.2) 56(84) bytes of data.
64 bytes from 192.168.0.2: icmp_seq=1 ttl=64 time=0.162 ms
```
Lines 15-16 of the wireshark [ trace](#section5) show the encrypted ICMP messages as ESP packets with the SPIs of the `eap ` `CHILD_SA`.

The number of ESP packets are also shown by the following `swanctl` command
```console
client#  swanctl --list-sas
eap: #1, ESTABLISHED, IKEv2, 9e4c18ae6de25007_i* 086a0efb7d745d8e_r
  local  '192.168.0.3' @ 192.168.0.3[4500] [10.3.0.1]
  remote 'server.strongswan.org' @ 192.168.0.2[4500]
  AES_CBC-256/HMAC_SHA2_256_128/PRF_HMAC_SHA2_256/CURVE_25519
  established 50s ago, rekeying in 12921s
  eap: #1, reqid 1, INSTALLED, TUNNEL, ESP:AES_GCM_16-256
    installed 50s ago, rekeying in 3291s, expires in 3910s
    in  c8f898c5,    252 bytes,     3 packets,    19s ago
    out cdb938a8,    252 bytes,     3 packets,    19s ago
    local  10.3.0.1/32
    remote 10.1.0.0/24 192.168.0.2/32
```
## Terminating Connection <a name="section4"></a>

The `IKE_SA` `eap` and the dependent `CHILD_SA` of the same name can be terminated with the following command
```console
client# swanctl --terminate --ike eap > /dev/null
06[CFG] vici terminate IKE_SA 'eap'
03[IKE] deleting IKE_SA eap[1] between 192.168.0.3[192.168.0.3]...192.168.0.2[server.strongswan.org]
```
The VPN client is sending an `INFORMATIONAL` request containing a `DELETE` notification
```console
03[IKE] sending DELETE for IKE_SA eap[1]
03[ENC] generating INFORMATIONAL request 5 [ D ]
03[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (80 bytes)
```
The `INFORMATIONAL` response is received from the VPN server causing the `IKE_SA` to be deleted
```console
13[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (80 bytes)
13[ENC] parsed INFORMATIONAL response 5 [ ]
13[IKE] IKE_SA deleted
```

## Wireshark Trace <a name="section5"></a>

![Wireshark Trace 4][WIRESHARK_4]

 * Line **1-2**:  IKE_SA_INIT request/response pair
 * Line **3-10**:  IKE_AUTH packets establishing EAP-based authentication
 * Line **11-14**:  Two encrypted pings to internal IP of VPN server (Intranet)
 * Line **15-16**:  One encrypted ping to external IP of VPN server (Internet)
 * Line **17-18**:  INFORMATIONAL request/response pair deleting IKE_SA

[WIRESHARK_4]: Wireshark4.png

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
