# -*- coding: utf-8 -*-
"""
Created on Fri Nov  1 12:07:20 2019

@author: dbrac

Linux Rootkit Project

	C2 Command Line Interface
	"""
	
import socket
import sys
from ctypes import *

"""TLV c-like struct"""
class tvl(Structure):
	fields = [("type", c_uint32), ("length", c_uint32), ("value", c_void_p)]
	
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
	"""switcher = {
		1: execute(),
		2: gather(),
		3: uninstall(),
		4: shutdown(),
		}
	func = switcher.get(choice, lambda: "Invalid Choice")
	func()"""
def main():
	while True:
		printMenu()
		menuOpt = int(input("Enter menu choice 1 - 4: "))        
		if menuOpt == 1:
			commandStr = str(input("Enter command to execute: "))
			commandArr = bytearray(commandStr, 'utf-8')
			commandSize = len(commandArr)
			commandArr.insert(0, commandSize)
			commandArr.insert(0, 1);
			print(commandArr)
			#send command packet
		elif menuOpt == 2:
			gatherCommand = bytearray(b'\x02\x00')
			print(gatherCommand)
			#send gather command
		elif menuOpt == 3:
			uninstallCommand = bytearray(b'\x03\x00')
			print(uninstallCommand)
			#send uninstall command
			break
		elif menuOpt == 4:
			shutdownCommand = bytearray(b'\x04\x00')
			print(shutdownCommand)
			#send shutdown command
			break
			
main()