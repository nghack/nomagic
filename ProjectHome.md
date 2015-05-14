A memory editing library.

Features:
  * Debugging
    * Hardware Breakpoints
    * Memory Breakpoints
    * Top Level Exception Filter

  * Just In Time Assembler (FASMLib)

  * Memory wrapper class with the following functions:
    * 2 different pattern scanners with customizable algorithm (uses BoyerMooreHorspool algorithm by default)
    * MS Detours and Import Address Table detours
    * Functions to allocate, free, write and read memory

  * Module wrapper class which includes:
    * Information such as the modules base address, its size and more
    * Provides easy access to the modules PE-Header

  * Process wrapper class which provides:
    * Functions to retrieve the process by a given window name, its PID or a window Handle
    * Enables your application to grant itself debug privileges
    * Information such as the PID, the process handle and the process name

  * Thread wrapper class
    * Functions to suspend, terminate and resume threads
    * Allows you to create threads in other processes
    * Allows you to redirect a thread at any given time

  * Advanced Dll Injector
    * Injects or ejects your dll into any given process
    * Calls your exported "Start" and "End" function to prevent deadlocks (you don't need DllMain any longer)

  * Almost fully x64 compatible! Only these 3 things that aren't:
    * Memory::DetourFunction
    * Memory::RemoveDetour
    * Just In Time Assembler (fasm namespace)



Expect more to come and feel free to contribute!