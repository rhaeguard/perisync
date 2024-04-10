# mouse sync

This repository contains two pieces of code that you can run together to syncronize the mouse movement across two machines - a Windows producer (the one that emits the mouse position) and a Linux (tested on Ubuntu/Wayland) receiver.

## steps

- Python (windows) and C (linux) compilers required.
- Install `paramiko` and `scp` on Windows
- Make sure you can SSH into the linux device
- Compile the C code on linux
- Create the target file on linux
- Run windows script first and make sure it is writing to the file
- Once everything works, run the linux executable using `sudo`
    - at the moment, to gently stop the script, all you need to do is to move your cursor to the top-left corner on windows which is the `(0, 0)` position. That will stop the script.