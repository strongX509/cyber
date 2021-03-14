# PSK-based Authentication 

## Table of Contents
1. [Configuration Files](#section1)
2. [Initiating Connection](#section2)
3. [Tunneling Traffic](#section3)
4. [Virtual IP Address Pool](#section4)
5. [Terminating Connection](#section5)
6. [Wireshark Trace](#section6)

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
   ike-hacker {
      id = hacker@strongswan.org
      secret = 0sH7+o6wysMoaELv5OBihKVa3F
   }
}
```
The client-side connection definition for authentication based on *Pre-Shared Keys*  (PSK) is defined in `/etc/swanctl/psk.conf`
```console
psk {
   remote_addrs = 192.168.0.2
   vips = 0.0.0.0

   local {
      auth = psk 
      id = hacker@strongswan.org
   }
   remote {
      auth = psk 
      id = server.strongswan.org
   }
   children {
      psk {
         remote_ts = 10.1.0.0/16

         esp_proposals = chacha20poly1305-x25519
         dpd_action = trap 
      }
   }
   version = 2
   proposals = aes256-sha256-x25519
   dpd_delay = 60s
   send_certreq = no
}
```

### VPN Server Configuration

The top-level configuration file is `/etc/swanctl/swanctl.conf` which also defines the pool used to allocate *Virtual IP* addresses and stores the *Pre-Shared Keys* of all remote access clients:
```console
connections {
   include rw.conf   include psk.conf
   include eap.conf
}

pools {
   rw_pool {
      addrs = 10.3.0.0/24
   }
}

secrets {
   ike-jane {
      id = jane@strongswan.org
      secret = 0sTtd7IOin6FuyjLsWtiM9o/1c
   }
   ike-hacker {
      id = hacker@strongswan.org
      secret = 0sH7+o6wysMoaELv5OBihKVa3F
   }
}
```
The server-side connection definition for authentication based on *Pre-Shared Keys* (PSK) is defined in `/etc/swanctl/psk.conf`
```console
psk {
   pools = rw_pool

   local {
      auth = psk 
      id = server.strongswan.org
   }
   remote {
      auth = psk 
   }
   children {
      psk {
         local_ts = 10.1.0.0/24

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

The  VPN client initiates the  `psk` CHILD_SA
```console
client# swanctl --initiate --child psk > /dev/null
14[CFG] vici initiate CHILD_SA 'psk'
16[IKE] initiating IKE_SA psk[2] to 192.168.0.2
```
The  VPN client is sending the `IKE_SA_INIT` request to the VPN server
```console
16[ENC] generating IKE_SA_INIT request 0 [ SA KE No N(NATD_S_IP) N(NATD_D_IP) N(FRAG_SUP) N(HASH_ALG) N(REDIR_SUP) ]
16[NET] sending packet: from 192.168.0.3[500] to 192.168.0.2[500] (240 bytes)
```
The `IKE_SA_INIT` response is received from the VPN server
```console
10[NET] received packet: from 192.168.0.2[500] to 192.168.0.3[500] (273 bytes)
10[ENC] parsed IKE_SA_INIT response 0 [ SA KE No N(NATD_S_IP) N(NATD_D_IP) CERTREQ N(FRAG_SUP) N(HASH_ALG) N(CHDLESS_SUP) N(MULT_AUTH) ]
```
The `SA` payload contains the selected IKE_SA` proposal
```console
10[CFG] selected proposal: IKE:AES_CBC_256/HMAC_SHA2_256_128/PRF_HMAC_SHA2_256/CURVE_25519
```
The VPN server doesn't know yet that the authentication will be `PSK`-based , therefore a `CERTREQ` payload is sent.
```console
10[IKE] received cert request for "C=CH, O=Cyber, CN=Cyber Root CA"
```
The  VPN client hashes its *Pre-Shared Key* into the `AUTH` payload
```console
10[IKE] authentication of 'hacker@strongswan.org' (myself) with pre-shared key
```
The `SA`, `TSi` and `TSr` payloads for the `CHILD_SA` `psk` are added
```console
10[IKE] establishing CHILD_SA psk{3}
```
The VPN client is sending the  `IKE_AUTH` request to the VPN server

![IKE_AUTH request/response for PSK-based authentication][IKE_AUTH_PSK]

```console
10[ENC] generating IKE_AUTH request 1 [ IDi N(INIT_CONTACT) IDr AUTH CPRQ(ADDR) SA TSi TSr N(MOBIKE_SUP) N(NO_ADD_ADDR) N(MULT_AUTH) N(EAP_ONLY) N(MSG_ID_SYN_SUP) ]
10[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (304 bytes)
```
The `IKE_AUTH` response is received from the VPN server
```console
06[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (256 bytes)
06[ENC] parsed IKE_AUTH response 1 [ IDr AUTH CPRP(ADDR) SA TSi TSr N(MOBIKE_SUP) N(ADD_4_ADDR) ]
```
The AUTH payload of the VPN server is verified 
```console
06[IKE] authentication of 'server.strongswan.org' with pre-shared key successful
```
The `IKE_SA` `psk`has been successfully established
```console
06[IKE] IKE_SA psk[2] established between 192.168.0.3[hacker@strongswan.org]...192.168.0.2[server.strongswan.org]
```
The  VPN server proposed the time of the next `IKE_SA` rekeying
```console
06[IKE] scheduling rekeying in 13651s
06[IKE] maximum IKE_SA lifetime 15091s
```
The  VPN server has assigned a *Virtual IP* address to the VPN client
```console
06[IKE] installing new virtual IP 10.3.0.2
```
The `SA`, `TSi` and `TSr` payloads received in the `IKE_AUTH`response define the crypto parameters and traffic selectors of the `CHILD_SA` to be established.
```console
06[CFG] selected proposal: ESP:CHACHA20_POLY1305/NO_EXT_SEQ
06[IKE] CHILD_SA psk{3} established with SPIs cbc9560b_i c8fe9401_o and TS 10.3.0.2/32 === 10.1.0.0/24
06[IKE] peer supports MOBIKE
```

[RFC_4555]: https://tools.ietf.org/html/rfc4555
[IKE_AUTH_PSK]: IKE_AUTH_PSK_665.png

## Tunneling Traffic <a name="section3"></a>

The VPN client pings the VPN server on its Intranet address 10.1.0.2 twice. The source address of the IP packets leaving the client equals the *Virtual IP* 10.3.0.2.
```console
client# ping -c 2 10.1.0.2
PING 10.1.0.2 (10.1.0.2) 56(84) bytes of data.
64 bytes from 10.1.0.2: icmp_seq=1 ttl=64 time=0.684 ms
64 bytes from 10.1.0.2: icmp_seq=2 ttl=64 time=0.312 ms
```
Lines 5-8 of the wireshark [ trace](#section5) show the encrypted ICMP messages as ESP packets with the SPIs of the `psk` `CHILD_SA`.

The number of ESP packets are also shown by the following `swanctl` command
```console
client#  swanctl --list-sas
psk: #2, ESTABLISHED, IKEv2, feb3507323a506cd_i* 05e7f2ba9a1bd9b7_r
  local  'hacker@strongswan.org' @ 192.168.0.3[4500] [10.3.0.2]
  remote 'server.strongswan.org' @ 192.168.0.2[4500]
  AES_CBC-256/HMAC_SHA2_256_128/PRF_HMAC_SHA2_256/CURVE_25519
  established 33s ago, rekeying in 13618s
  psk: #3, reqid 1, INSTALLED, TUNNEL, ESP:CHACHA20_POLY1305
    installed 33s ago, rekeying in 3353s, expires in 3927s
    in  cbc9560b,    168 bytes,     2 packets,    15s ago
    out c8fe9401,    168 bytes,     2 packets,    15s ago
    local  10.3.0.2/32
    remote 10.1.0.0/24
```
## Virtual IP Address Pool <a name="section4"></a>

Change for a while to the VPN server console window and have a look at the pool of virtual IP addresses and the existing address leases:
```console
server# swanctl --list-pools --leases
rw_pool              10.3.0.0                         1 / 1 / 254
  10.3.0.1                       offline  'client.strongswan.org'
  10.3.0.2                       online   'hacker@strongswan.org'
```
We see that for the current `psk` `IKE_SA` the virtual IP address `10.3.0.2` has been leased to the client `hacker@strongswan.org` and that from the previous `home` `IKE_SA` the `10.3.0.1` address has been reserved for the client `client.strongswan.org` which is currently offline.

## Terminating Connection <a name="section5"></a>

The `IKE_SA` `psk` and the dependent `CHILD_SA` of the same name can be terminated in the VPN client window with the following command
```console
client# swanctl --terminate --ike psk > /dev/null
11[CFG] vici terminate IKE_SA 'psk'
10[IKE] deleting IKE_SA psk[2] between 192.168.0.3[hacker@strongswan.org]...192.168.0.2[server.strongswan.org]
```
The VPN client is sending an `INFORMATIONAL` request containing a `DELETE` notification
```console
10[IKE] sending DELETE for IKE_SA psk[2]
10[ENC] generating INFORMATIONAL request 2 [ D ]
10[NET] sending packet: from 192.168.0.3[4500] to 192.168.0.2[4500] (80 bytes)
```
The `INFORMATIONAL` response is received from the VPN server causing the `IKE_SA` to be deleted
```console
05[NET] received packet: from 192.168.0.2[4500] to 192.168.0.3[4500] (80 bytes)
05[ENC] parsed INFORMATIONAL response 2 [ ]
05[IKE] IKE_SA deleted
```

## Wireshark Trace <a name="section6"></a>

![Wireshark Trace 3][WIRESHARK_3]

 * Line **1-2**:  IKE_SA_INIT request/response pair
 * Line **3-4**: IKE_AUTH request/response pair establishing PSK-based authentication
 * Line **5-8**:  Two encrypted pings to internal IP of VPN server (Intranet)
 * Line **9-10**:  INFORMATIONAL request/response pair deleting the IKE_SA


[WIRESHARK_3]: Wireshark3.png

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
