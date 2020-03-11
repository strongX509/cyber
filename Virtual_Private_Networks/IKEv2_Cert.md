# Certificate-based Authentication 

## Table of Contents
1. [Configuration Files](#section1)
2. [Initiating Connection](#section2)
3. [Tunneling Traffic](#section3)
3. [Adding Second Child SA](#section4)
4. [Dead Peer Detection](#section5)
5. [Terminating Connection](#section6)
4. [Wireshark Trace](#section7)

strongSwan exercise: &nbsp; [strongSwan1](#strongswan1) 

## Configuration Files <a name="section1"></a>

### VPN Client Configuration

strongSwan connections are defined in `/etc/swanctl/swanctl.conf` which in turn can include other configuration files:
```console
connections {
   include home.conf
   include psk.conf
   include eap.conf
}
```
The client-side connection definition for certificate-based authentication is defined in `/etc/swanctl/home.conf`
```console
home {
   remote_addrs = 192.168.0.2
   vips = 0.0.0.0

   local {
      auth = pubkey
      certs = clientCert.pem
      id = client.strongswan.org
   }
   remote {
      auth = pubkey
      id = server.strongswan.org
   }
   children {
      net {
         remote_ts = 10.1.0.0/16

         esp_proposals = aes256gcm128-chacha20poly1305-x25519
         dpd_action = trap 
      }
      host {
         esp_proposals = aes256gcm128-chacha20poly1305-x25519
         dpd_action = trap
      }
   }
   version = 2
   proposals = aes256-sha256-x25519
   dpd_delay = 60s
}
```

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
The server-side connection definition for certificate-based authentication is defined in `/etc/swanctl/rw.conf`
```console
rw {
   pools = rw_pool

   local {
      auth = pubkey
      certs = serverCert.pem
      id = server.strongswan.org
   }
   remote {
      auth = pubkey
      cacerts = caCert.pem
   }
   children {
      net {
         local_ts = 10.1.0.0/24

         esp_proposals = aes256gcm128-chacha20poly1305-x25519
         dpd_action = clear
      }
      host {
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

The  VPN client initiates the  `net` CHILD_SA

```console
client# swanctl --initiate --child net > /dev/null
09[CFG] vici initiate CHILD_SA 'net'
14[IKE] initiating IKE_SA home[1] to 192.168.0.2
```
The  VPN client is sending the `IKE_SA_INIT` request to the VPN server
```console
14[ENC] generating IKE_SA_INIT request 0 [ SA KE No N(NATD_S_IP) N(NATD_D_IP) N(FRAG_SUP) N(HASH_ALG) N(REDIR_SUP) ]
14[NET] sending packet: from 192.168.0.3[500] to 192.168.0.2[500] (240 bytes)
```
The `IKE_SA_INIT` response is received from the VPN server
```console
05[NET] received packet: from 192.168.0.2[500] to 192.168.0.3[500] (273 bytes)
05[ENC] parsed IKE_SA_INIT response 0 [ SA KE No N(NATD_S_IP) N(NATD_D_IP) CERTREQ N(FRAG_SUP) N(HASH_ALG) N(CHDLESS_SUP) N(MULT_AUTH) ]
```
The `SA` payload contains the selected IKE_SA` proposal
```console
05[CFG] selected proposal: IKE:AES_CBC_256/HMAC_SHA2_256_128/PRF_HMAC_SHA2_256/CURVE_25519
```
In the `CERTREQ` payload the VPN server requested a client certificate issued by the `Cyber Root CA` , as does the VPN client
```console
05[IKE] received cert request for "C=CH, O=Cyber, CN=Cyber Root CA"
05[IKE] sending cert request for "C=CH, O=Cyber, CN=Cyber Root CA"
```
The  VPN client generates a digital signature using its ECDSA private key and adds the matching client certificate
```console
05[IKE] authentication of 'client.strongswan.org' (myself) with ECDSA_WITH_SHA384_DER successful
05[IKE] sending end entity cert "C=CH, O=Cyber, CN=client.strongswan.org"
```
The `SA`, `TSi` and `TSr` payloads for the `CHILD_SA` `net` are added
```console
05[IKE] establishing CHILD_SA net{1}
```
The VPN client is sending the  `IKE_AUTH` request to the VPN server

![IKE_AUTH request/response for certificate-based authentication][IKE_AUTH]

```console
05[ENC] generating IKE_AUTH request 1 [ IDi CERT N(INIT_CONTACT) CERTREQ IDr AUTH CPRQ(ADDR) SA TSi TSr N(MOBIKE_SUP) N(NO_ADD_ADDR) N(MULT_AUTH) N(EAP_ONLY) N(MSG_ID_SYN_SUP) ]
05[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (944 bytes)
```
The `IKE_AUTH` response is received from the VPN server
```console
12[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (864 bytes)
12[ENC] parsed IKE_AUTH response 1 [ IDr CERT AUTH CPRP(ADDR) SA TSi TSr N(MOBIKE_SUP) N(ADD_4_ADDR) ]
```
The certificate and the digital signature of the VPN server is verified
```console
12[IKE] received end entity cert "C=CH, O=Cyber, CN=server.strongswan.org"
12[CFG]   using certificate "C=CH, O=Cyber, CN=server.strongswan.org"
12[CFG]   using trusted ca certificate "C=CH, O=Cyber, CN=Cyber Root CA"
12[CFG]   reached self-signed root ca with a path length of 0
12[IKE] authentication of 'server.strongswan.org' with ECDSA_WITH_SHA384_DER successful
```
The `IKE_SA` `home`has been successfully established
```console
12[IKE] IKE_SA home[1] established between 192.168.0.3[client.strongswan.org]...192.168.0.2[server.strongswan.org]
```
The  VPN server proposed the time of the next `IKE_SA` rekeying
```console
12[IKE] scheduling rekeying in 13867s
12[IKE] maximum IKE_SA lifetime 15307s
```
The  VPN server has assigned a *Virtual IP* address to the VPN client
```console
12[IKE] installing new virtual IP 10.3.0.1
```
The `SA`, `TSi` and `TSr` payloads received in the `IKE_AUTH`response define the crypto parameters and traffic selectors of the `CHILD_SA` to be established.
```console
12[CFG] selected proposal: ESP:AES_GCM_16_256/NO_EXT_SEQ
12[IKE] CHILD_SA net{1} established with SPIs cd491506_i c91df918_o and TS 10.3.0.1/32 === 10.1.0.0/24
12[IKE] peer supports MOBIKE

```
Additionally the `N(MOBIKE_SUP)` notification tells that the VPN server supports the *IKEv2 Mobility and Multihoming* protocol (MOBIKE) defined in [RFC 4555][RFC_4555].
This is the reason that starting with the `IKE_AUTH` request the IKE port floats from UDP 500 to UDP 4500.

[RFC_4555]: https://tools.ietf.org/html/rfc4555
[IKE_AUTH]: IKE_AUTH_665.png
 
## Tunneling Traffic <a name="section3"></a>

The VPN client pings the VPN server on its Intranet address 10.1.0.2 twice. The source address of the IP packets leaving the client equals the *Virtual IP* 10.3.0.1.
```console
client# ping -c 2 10.1.0.2
PING 10.1.0.2 (10.1.0.2) 56(84) bytes of data.
64 bytes from 10.1.0.2: icmp_seq=1 ttl=64 time=0.198 ms
64 bytes from 10.1.0.2: icmp_seq=2 ttl=64 time=0.253 ms
```
Lines 5-8 of the wireshark [ trace](#section7) show the encrypted ICMP messages as ESP packets with the SPIs of the `net` `CHILD_SA`.

The number of ESP packets are also shown by the following `swanctl` command
```console
client# swanctl --list-sas
home: #1, ESTABLISHED, IKEv2, 2d0754f555a7516b_i* 940b49992d99ca82_r
  local  'client.strongswan.org' @ 192.168.0.3[4500] [10.3.0.1]
  remote 'server.strongswan.org' @ 192.168.0.2[4500]
  AES_CBC-256/HMAC_SHA2_256_128/PRF_HMAC_SHA2_256/CURVE_25519
  established 26s ago, rekeying in 13769s
  net: #1, reqid 1, INSTALLED, TUNNEL, ESP:AES_GCM_16-256
    installed 26s ago, rekeying in 3239s, expires in 3934s
    in  cd491506,    168 bytes,     2 packets,     7s ago
    out c91df918,    168 bytes,     2 packets,     7s ago
    local  10.3.0.1/32
    remote 10.1.0.0/24
```
When the external IP address 192.168.0.2 of the VPN server is pinged, there is no match with the traffic selectors of the `net` `CHILD_SA` and the ICMP request and reply messages are sent in plaintext as lines 9-10 of the wireshark [trace](#section7) shows.
```console
client# ping -c 1 192.168.0.2
PING 192.168.0.2 (192.168.0.2) 56(84) bytes of data.
64 bytes from 192.168.0.2: icmp_seq=1 ttl=64 time=0.170 ms
```

## Adding Second Child SA <a name="section4"></a>

We now initiate the `host` `CHILD_SA` which will encrypt traffic to the external IP address of the VPN server.
```console
client# swanctl --initiate --child host
07[CFG] vici initiate CHILD_SA 'host'
06[IKE] establishing CHILD_SA host{2}
```
The VPN client is sending a `CREATE_CHILD_SA` request
```console
06[ENC] generating CREATE_CHILD_SA request 2 [ SA No KE TSi TSr ]
06[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (256 bytes)
```
The `CREATE_CHILD_SA` response is received from the VPN server
```console
08[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (240 bytes)
08[ENC] parsed CREATE_CHILD_SA response 2 [ SA No KE TSi TSr ]
```
The `SA`, `TSi` and `TSr` payloads received with the `CREATE_CHILD_SA` response define the crypto parameters and traffic selectors of the `CHILD_SA` to be established
```console
08[CFG] selected proposal: ESP:AES_GCM_16_256/CURVE_25519/NO_EXT_SEQ
08[IKE] CHILD_SA host{2} established with SPIs c4066caa_i c3e8cfc8_o and TS 10.3.0.1/32 === 192.168.0.2/32
```
Now when we ping the external IP address 192.168.0.2 of the VPN server again
```console
client# ping -c 1 192.168.0.2
PING 192.168.0.2 (192.168.0.2) 56(84) bytes of data.
64 bytes from 192.168.0.2: icmp_seq=1 ttl=64 time=0.170 ms
```
lines 13-14 of the wireshark [trace](#section7) show the encrypted ICMP messages as ESP packets with the SPIs of the `host` `CHILD_SA`.

The same information can be retrieved from the IPsec stack in the Linux kernel via `swanctl`
```console
client# swanctl --list-sas
home: #1, ESTABLISHED, IKEv2, 2d0754f555a7516b_i* 940b49992d99ca82_r
  local  'client.strongswan.org' @ 192.168.0.3[4500] [10.3.0.1]
  remote 'server.strongswan.org' @ 192.168.0.2[4500]
  AES_CBC-256/HMAC_SHA2_256_128/PRF_HMAC_SHA2_256/CURVE_25519
  established 88s ago, rekeying in 13707s
  net: #1, reqid 1, INSTALLED, TUNNEL, ESP:AES_GCM_16-256
    installed 88s ago, rekeying in 3177s, expires in 3872s
    in  cd491506,    168 bytes,     2 packets,    70s ago
    out c91df918,    168 bytes,     2 packets,    69s ago
    local  10.3.0.1/32
    remote 10.1.0.0/24
  host: #2, reqid 2, INSTALLED, TUNNEL, ESP:AES_GCM_16-256/CURVE_25519
    installed 30s ago, rekeying in 3428s, expires in 3930s
    in  c4066caa,     84 bytes,     1 packets,     7s ago
    out c3e8cfc8,     84 bytes,     1 packets,     7s ago
    local  10.3.0.1/32
    remote 192.168.0.2/32
```

## Dead Peer Detection <a name="section5"></a>

In order to check if the peer in an IPsec connection is still alive,  a *Dead Peer Detection* message  is sent if no `ESP` or `IKE` packet has been received in `dpd_delay = 60s`.  This optional heart beat is based on empty `INFORMATIONAL` request/response pairs as lines 15-18 of the wireshark [trace](#section7) show 
```console
14[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (80 bytes)
14[ENC] parsed INFORMATIONAL request 0 [ ]
14[ENC] generating INFORMATIONAL response 0 [ ]
14[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (80 bytes)
```
and again 60 seconds later
```console
05[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (80 bytes)
05[ENC] parsed INFORMATIONAL request 1 [ ]
05[ENC] generating INFORMATIONAL response 1 [ ]
05[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (80 bytes)
```

## Terminating Connection <a name="section6"></a>

The `IKE_SA` `home` and the dependent `CHILD_SAs`  `net` and `host` can be terminated with the following command
```console
client# swanctl --terminate --ike home > /dev/null
10[CFG] vici terminate IKE_SA 'home'
14[IKE] deleting IKE_SA home[1] between 192.168.0.3[client.strongswan.org]...192.168.0.2[server.strongswan.org]
```
The VPN client is sending an `INFORMATIONAL` request containing a `DELETE` notification
```console
14[IKE] sending DELETE for IKE_SA home[1]
14[ENC] generating INFORMATIONAL request 3 [ D ]
14[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (80 bytes)
```
The `INFORMATIONAL` response is received from the VPN server causing the `IKE_SA` to be deleted
```console
16[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (80 bytes)
16[ENC] parsed INFORMATIONAL response 3 [ ]
16[IKE] IKE_SA deleted
```

## Wireshark Trace <a name="section7"></a>

![Wireshark Trace 2][WIRESHARK_2]

 * Line **1-2**:  IKE_SA_INIT request/response pair
 * Line **3-4**: IKE_AUTH request/response pair establishing certificate-based authentication
 * Line **5-8**:  Two encrypted pings to internal IP of VPN server (Intranet)
 * Line **9-10**:  One plaintext ping to external IP of VPN server (Internet)
 * Line **11-12**: CREATE_CHILD_SA request/response pair establishing second CHILD_SA
 * Line **13-14**:  One encrypted ping to external IP of VPN server (Internet)
 * Line **15-18**: INFORMATIONAL request/response pairs  every 60 s as DPD heart beats
 * Line **19-20**: INFORMATIONAL request/response pair deleting IKE_SA

[WIRESHARK_2]: Wireshark2.png

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
