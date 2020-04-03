# EAP-TLS-based Authentication 

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
```
The client-side connection definition for authentication based on *EAP-TLS* is defined in `/etc/swanctl/eap-tls.conf`
```console
eap-tls {
   remote_addrs = 192.168.0.2
   vips = 0.0.0.0

   local {
      auth = eap-tls 
      eap_id = client.strongswan.org
   }
   remote {
      auth = pubkey 
      id = server.strongswan.org
   }
   children {
      eap-tls {
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

The top-level configuration file is `/etc/swanctl/swanctl.conf` which also defines the pool used to allocate *Virtual IP* addresses:

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

The  VPN client initiates the  `eap-tls` CHILD_SA
```console
client# swanctl --initiate --child eap-tls > /dev/null
01[CFG] vici initiate CHILD_SA 'eap-tls'
16[IKE] initiating IKE_SA eap-tls[1] to 192.168.0.2
```
The  VPN client is sending the `IKE_SA_INIT` request to the VPN server
```console
16[ENC] generating IKE_SA_INIT request 0 [ SA KE No N(NATD_S_IP) N(NATD_D_IP) N(FRAG_SUP) N(HASH_ALG) N(REDIR_SUP) ]
16[NET] sending packet: from 192.168.0.3[500] to 192.168.0.2[500] (240 bytes)
```
The `IKE_SA_INIT` response is received from the VPN server
```console
12[NET] received packet: from 192.168.0.2[500] to 192.168.0.3[500] (273 bytes)
12[ENC] parsed IKE_SA_INIT response 0 [ SA KE No N(NATD_S_IP) N(NATD_D_IP) CERTREQ N(FRAG_SUP) N(HASH_ALG) N(CHDLESS_SUP) N(MULT_AUTH) ]
```
The `SA` payload contains the selected IKE_SA` proposal
```console
12[CFG] selected proposal: IKE:AES_CBC_256/HMAC_SHA2_256_128/PRF_HMAC_SHA2_256/CURVE_25519
```
In the `CERTREQ` payload the VPN server requested a client certificate issued by the `Cyber Root CA` , as does the VPN client
```console
12[IKE] received cert request for "C=CH, O=Cyber, CN=Cyber Root CA"
12[IKE] sending cert request for "C=CH, O=Cyber, CN=Cyber Root CA"
```
No explicit IKEv2 identity is defined, therefore by default `IDi` is set to the host  IP address `192.168.0.3`
```console
12[CFG] no IDi configured, fall back on IP address
```
The `SA`, `TSi` and `TSr` payloads for the `CHILD_SA` `psk` are added
```console
12[IKE] establishing CHILD_SA eap-tls{1}
```
The VPN client is sending the  `IKE_AUTH` request to the VPN server. No `AUTH`payload is sent, thereby signalling to the VPN server that EAP-based authentication is desired.

![IKE_AUTH request/response for EAP-based authentication][IKE_AUTH_EAP]

```console
12[ENC] generating IKE_AUTH request 1 [ IDi N(INIT_CONTACT) CERTREQ IDr CPRQ(ADDR) SA TSi TSr N(MOBIKE_SUP) N(NO_ADD_ADDR) N(MULT_AUTH) N(EAP_ONLY) N(MSG_ID_SYN_SUP) ]
12[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (304 bytes)
```
The first `IKE_AUTH` response is received from the VPN server
```console
09[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (752 bytes)
09[ENC] parsed IKE_AUTH response 1 [ IDr CERT AUTH EAP/REQ/ID ]
```
The certificate and the digital signature of the VPN server is verified
```console
09[IKE] received end entity cert "C=CH, O=Cyber, CN=server.strongswan.org"
09[CFG]   using certificate "C=CH, O=Cyber, CN=server.strongswan.org"
09[CFG]   using trusted ca certificate "C=CH, O=Cyber, CN=Cyber Root CA"
09[CFG]   reached self-signed root ca with a path length of 0
09[IKE] [CFG]   reached self-signed root ca with a path length of 0
09[IKE] authentication of 'server.strongswan.org' with ECDSA_WITH_SHA384_DER successful
```
The VPN server requests the VPN client's `EAP IDENTITY` 
```console
09[IKE] server requested EAP_IDENTITY (id 0x00), sending 'client.strongswan.org'
09[ENC] generating IKE_AUTH request 2 [ EAP/RES/ID ]
09[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (96 bytes)
```
The VPN server sends an `EAP-MD5` challenge
```console
08[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (96 bytes)
08[ENC] parsed IKE_AUTH response 2 [ EAP/REQ/MD5 ]
08[IKE] server requested EAP_MD5 authentication (id 0xCF)
```
The VPN client wants to do `EAP-TLS` instead and returns an `EAP-NAK` response
```console
08[IKE] requesting EAP_TLS authentication, sending EAP_NAK
08[ENC] generating IKE_AUTH request 3 [ EAP/RES/NAK ]
08[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (80 bytes)
```
The VPN server now requests `EAP-TLS`
```console
16[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (80 bytes)
16[ENC] parsed IKE_AUTH response 3 [ EAP/REQ/TLS ]
16[IKE] server requested EAP_TLS authentication (id 0x78)
```
The VPN client starts the `TLS` handshake with the `Client Hello` message
```console
16[ENC] generating IKE_AUTH request 4 [ EAP/RES/TLS ]
16[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (224 bytes)
```
and the VPN server replies with the `Server Hello` message establishing a `TLS`cipher suite
```console
14[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (928 bytes)
14[ENC] parsed IKE_AUTH response 4 [ EAP/REQ/TLS ]
14[TLS] negotiated TLS 1.2 using suite TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384
```
and again sends its server certificate, this time  for  the  `TLS` server authentication 
```console
14[TLS] received TLS server certificate 'C=CH, O=Cyber, CN=server.strongswan.org'
14[CFG]   using certificate "C=CH, O=Cyber, CN=server.strongswan.org"
14[CFG]   using trusted ca certificate "C=CH, O=Cyber, CN=Cyber Root CA"
14[CFG]   reached self-signed root ca with a path length of 0
```
The VPN client in turn sends its client certificate for `TLS` client authentication
```console
14[TLS] sending TLS peer certificate 'C=CH, O=Cyber, CN=client.strongswan.org'
14[ENC] generating IKE_AUTH request 5 [ EAP/RES/TLS ]
```
The VPN server sends its `Change Cipherspec` and `Finished` messages
```console
05[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (144 bytes)
05[ENC] parsed IKE_AUTH response 5 [ EAP/REQ/TLS ]
```
and the VPN client does the same
```console
05[ENC] generating IKE_AUTH request 6 [ EAP/RES/TLS ]
05[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (80 bytes)
```
The `EAP-TLS`  handshake  was successful
```console
11[NET] [NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (80 bytes)
received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (80 bytes)
11[ENC] parsed IKE_AUTH response 6 [ EAP/SUCC ]
11[IKE] EAP method EAP_TLS succeeded, MSK established
```
The VPN client returns an`AUTH` payload
```console
11[IKE] authentication of '192.168.0.3' (myself) with EAP
11[ENC] generating IKE_AUTH request 7 [ AUTH ]
11[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (112 bytes)
```
The final `IKE_AUTH` response is received from the VPN server
```console
12[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (256 bytes)
12[ENC] parsed IKE_AUTH response 7 [ AUTH CPRP(ADDR) SA TSi TSr N(MOBIKE_SUP) N(ADD_4_ADDR) ]
12[IKE] authentication of 'server.strongswan.org' with EAP successful
```
The `IKE_SA` `eap-tls`has been successfully established
```console
12[IKE] IKE_SA eap-tls[1] established between 192.168.0.3[192.168.0.3]...192.168.0.2[server.strongswan.org]
```
The  VPN server proposed the time of the next `IKE_SA` rekeying
```console
12[IKE] scheduling rekeying in 14015s
12[IKE] maximum IKE_SA lifetime 15455s
```
The  VPN server has assigned a *Virtual IP* address to the VPN client
```console
12[IKE] installing new virtual IP 10.3.0.1
```
The `SA`, `TSi` and `TSr` payloads received in the `IKE_AUTH` response define the crypto parameters and traffic selectors of the `CHILD_SA` to be established.
```console
12[CFG] selected proposal: ESP:AES_GCM_16_256/NO_EXT_SEQ
12[IKE] CHILD_SA eap-tls{1} established with SPIs c1a15a6f_i cd5a513a_o and TS 10.3.0.1/32 === 10.1.0.0/24 192.168.0.2/32
12[IKE] peer supports MOBIKE
```

[RFC_4555]: https://tools.ietf.org/html/rfc4555
[IKE_AUTH_EAP]: IKE_AUTH_EAP_665.png

## Tunneling Traffic <a name="section3"></a>

The VPN server using its internal IP address 10.1.0.2 pings the VPN client on its  Virtual IP address 10.3.0.1 twice. 
```console
server# ping -c 2 10.3.0.1
PING 10.3.0.1 (10.3.0.1) 56(84) bytes of data.
64 bytes from 10.3.0.1: icmp_seq=1 ttl=64 time=0.103 ms
64 bytes from 10.3.0.1: icmp_seq=2 ttl=64 time=0.306 ms
```
Lines 17-20 of the wireshark [ trace](#section5) show the encrypted ICMP messages as ESP packets with the SPIs of the `eap-tls` `CHILD_SA`.

When the VPN client pings the VPN server on its Internet address 192.168.0.2 the packet is encrypted as well because the traffic selector includes the external server IP address as well 
```console
client# # ping -c 1 192.168.0.2
PING 192.168.0.2 (192.168.0.2) 56(84) bytes of data.
64 bytes from 192.168.0.2: icmp_seq=1 ttl=64 time=0.140 ms
```
Lines 21-22 of the wireshark [ trace](#section5) show the encrypted ICMP messages as ESP packets with the SPIs of the `eap-tls` `CHILD_SA`.

The number of ESP packets are also shown by the following `swanctl` command
```console
client#  swanctl --list-sas
eap-tls: #1, ESTABLISHED, IKEv2, 604bd5b13769a774_i* 1cf8c891dbde3e24_r
  local  '192.168.0.3' @ 192.168.0.3[4500] [10.3.0.1]
  remote 'server.strongswan.org' @ 192.168.0.2[4500]
  AES_CBC-256/HMAC_SHA2_256_128/PRF_HMAC_SHA2_256/CURVE_25519
  established 44s ago, rekeying in 13971s
  eap-tls: #1, reqid 1, INSTALLED, TUNNEL, ESP:AES_GCM_16-256
    installed 44s ago, rekeying in 3458s, expires in 3916s
    in  c1a15a6f,    252 bytes,     3 packets,    18s ago
    out cd5a513a,    252 bytes,     3 packets,    18s ago
    local  10.3.0.1/32
    remote 10.1.0.0/24 192.168.0.2/32
```
## Terminating Connection <a name="section4"></a>

The `IKE_SA` `eap-tls` and the dependent `CHILD_SA` of the same name can be terminated with the following command
```console
client# swanctl --terminate --ike eap-tls
08[CFG] vici terminate IKE_SA 'eap-tls'
01[IKE] deleting IKE_SA eap-tls[1] between 192.168.0.3[192.168.0.3]...192.168.0.2[server.strongswan.org]
```
The VPN client is sending an `INFORMATIONAL` request containing a `DELETE` notification
```console
01[IKE] sending DELETE for IKE_SA eap-tls[1]
01[ENC] generating INFORMATIONAL request 8 [ D ]
01[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (80 bytes)
```
The `INFORMATIONAL` response is received from the VPN server causing the `IKE_SA` to be deleted
```console
11[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (80 bytes)
11[ENC] parsed INFORMATIONAL response 8 [ ]
11[IKE] IKE_SA deleted
```

## Wireshark Trace <a name="section5"></a>

![Wireshark Trace 5][WIRESHARK_5]

 * Line **1-2**:  IKE_SA_INIT request/response pair
 * Line **3-16**:  IKE_AUTH packets establishing EAP-TLS-based authentication
 * Line **17-20**:  Two encrypted pings from internal IP of VPN server to virtual IP of VPN client
 * Line **21-22**:  One encrypted ping to external IP of VPN server (Internet)
 * Line **23-24**:  INFORMATIONAL request/response pair deleting IKE_SA

[WIRESHARK_5]: Wireshark5.png

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
