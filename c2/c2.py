# -*- coding: utf-8 -*-
"""
Created on Fri Nov  1 12:07:20 2019
@author: dbrac
Linux Rootkit Project
	C2 Command Line Interface
	"""
	
import socket
import sys


def toTLV(option, commandStr = ""):
	commandSize = len(commandArr)
	commandStr.insert(0, commandSize)
	commandStr.insert(0, option);
	print(commandArr)
	return commandArr

def fromTLV():
	return "Converting from TLV..."

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
  
def Menu():
	printMenu()
	menuOpt = int(input("Enter menu choice 1 - 4: "))        
	if menuOpt == 1:
		commandStr = str(input("Enter command to execute: "))
		commandArr = toTLV(menuOpt, commandStr)
  elif menuOpt == 2:
    commandArr = toTLV(menuOpt)
  elif menuOpt == 3:
		commandArr = toTLV(menuOpt)
	elif menuOpt == 4:
		commandArr = toTLV(menuOpt)
  else:
    print("ERR")
    return Menu()
  return commandArr

def main():
  print(Menu())
main()