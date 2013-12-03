source: http://www.securityfocus.com/bid/2551/info

PIX is an enterprise firewall engineered and maintained by Cisco Systems. It is designed to provide robust features and multiple methods of access control and filtering.

A problem with the PIX could allow a denial of service. PIX firewalls using TACACS+ are vulnerable to a resource starvation attack which results in a denial of service. Upon receiving multiple requests for TACACS+ authentication from an unauthorized user, the firewalls resources can be exhausted. This causes the firewall to crash, requiring power cycling to resume regular service.

This makes it possible for a user from either the public or private side of the PIX to crash the firewall, and deny service to legitimate users.

All PIX Firewalls having configuration lines beginning with the following line are affected:
pixfirewall# aaa authentication

Any configurations not including aaa authentication are not affected. 

while (true); do (wget http://external.system 2>/dev/null &); done 