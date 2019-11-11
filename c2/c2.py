# -*- coding: utf-8 -*-
"""
Created on Fri Nov  1 12:07:20 2019

@author: dbrac

Linux Rootkit Project

C2 Command Line Interface
"""

def execute():
    return "This will execute a command"

def gather():
    return "This will gather keylogged commands"

def uninstall():
    return "This will unistall the kernel module"

def shutdown():
    return "This will shutdown the C2"

def printMenu():
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

def convertTLV():
    return "Converting to TLV..."

def unconvertTLV():
    return "Converting from TLV..."

def encrypt():
    return "Encrypting the message..."

def send():
    return "Sending data..."

def wait():
    return "Waiting..."

def saveData():
    return "Data saved to file: ..."

def numbers_to_menu_options(choice):
    switcher = {
            1: execute(),
            2: gather(),
            3: uninstall(),
            4: shutdown(),
            }
    func = switcher.get(choice, lambda: "Invalid Choice")
    print(func)


if __name__ == "__main__":
    while True:
        printMenu()    
        i = int(input("Enter menu choice 1 - 4: "))
        numbers_to_menu_options(i)
        if i == 3 or i == 4:
            break
