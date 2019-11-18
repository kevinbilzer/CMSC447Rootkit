# -*- coding: utf-8 -*-
"""
Created on Fri Nov  1 12:07:20 2019

@author: dbrac

Linux Rootkit Project

C2 Command Line Interface
"""

import socket
import binascii
import sys

def print_menu():
    print(
            """
######                                        #     #                      
#     #  ####   ####  ##### #    # # #####    ##   ## ###### #    # #    #
#     # #    # #    #   #   #   #  #   #      # # # # #      ##   # #    #
######  #    # #    #   #   ####   #   #      #  #  # #####  # #  # #    #
#   #   #    # #    #   #   #  #   #   #      #     # #      #  # # #    #
#    #  #    # #    #   #   #   #  #   #      #     # #      #   ## #    # 
#     #  ####   ####    #   #    # #   #      #     # ###### #    #  #### 
            
                        1: Execute Command
                        2: Collect Data
                        3: Uninstall
                        4: Shutdown
            """
        )

def execute(socket):
    return "This will execute a command"

def collect(socket):
    return "This will gather keylogged commands"

def uninstall(socket):
    return "This will unistall the kernel module"

def shutdown(socket):
    return "This will shutdown the C2"

def numbers_to_menu_options(choice, socket):
    switcher = {
            1: execute(socket),
            2: collect(socket),
            3: uninstall(socket),
            4: shutdown(socket),
            }
    func = switcher.get(choice, lambda: "Invalid Choice")
    print(func)
    
def encrypt(message, key):
    byte_list = [bytes(message[i:i+4]) for i in range(0, len(message)-3, 4)]
    int_list = [int.from_bytes(byte_list[i]) for i in range(len(byte_list))]

    for i in range(len(int_list)):
        int_list[i] ^= int.from_bytes(key)
        byte_list[i] = int_list[i].to_bytes(4)

    return b''.join(byte_list)

def to_TLV(message): 
    print("Converting to TLV...")
    return message

def from_TLV(message):
    
    message = "\x01\x07\x00\x00\x00\x6c\x73\x20\x2f\x74\x6d\x70".encode()
    my_array = []
    
    for i in range (len(message)):
        string = message[i]
        my_array.append(string)

def send(message):
    print("Sending data...")

def save_data(message):
    print("Data saved to file: ...")
    print(f"Your message was: {message}")


# Main
if __name__ == "__main__":
    
    # create a socket object
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("Socket created, binding to port...")
    
    #reserve port 1337
    port =  1337
    
    # Bind to the port, input empty string for ip
    s.bind(('', port))
    print (f"Socket bound to {port} successfully, waiting for response...")
    
    s.listen(5)
    
    while True:
         # Establish connection with client
         c, addr = s.accept()
         print (f"Connection from {addr} accepted.  Awaiting commands...")
         # Print the menu once before the Menu Loop
         print_menu() 
         while True:
             i = int(input("Enter menu choice 1 - 4: "))
             numbers_to_menu_options(i, c)
             if i == 3 or i == 4:
                 break
         break
