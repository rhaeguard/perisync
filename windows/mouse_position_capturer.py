# pip install paramiko scp

import ctypes
import paramiko

# SSH connection parameters
hostname = '<HOSTNAME>'
username = '<USERNAME>'
password = '<PASSWORD>'

# Connect to the Ubuntu machine
ssh_client = paramiko.SSHClient()
ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh_client.connect(hostname, username=username, password=password)

# =========================================

TARGET_FILE = "<TARGET_FILE_TO_PUT_COORDS_IN>"

# Load the user32.dll library
user32 = ctypes.WinDLL("user32")

class POINT(ctypes.Structure):
    _fields_ = [("x", ctypes.c_long), ("y", ctypes.c_long)]

# Define the GetCursorPos function prototype
GetCursorPos = user32.GetCursorPos
# GetCursorPos.argtypes = [ctypes.POINTER(POINT)]
# GetCursorPos.restype = ctypes.c_bool

try:
    while True:
        p = POINT()
        GetCursorPos(ctypes.byref(p))

        # Command to append data to a file
        append_command = f'echo "{p.x} {p.y}" > {TARGET_FILE}'

        # Execute the command on the Ubuntu machine
        stdin, stdout, stderr = ssh_client.exec_command(append_command)

        # Check for any errors or get the command output if needed
        print(stderr.read().decode())
        print(stdout.read().decode())
except:
    # Close the SSH connection
    ssh_client.close()
    print("closed the ssh connection")