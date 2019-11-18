# -*- coding: utf-8 -*-
"""
Created on Fri Nov 8 21:00:51 2019

@author: dbrac
"""

import socket

# Create a socket object
s = socket.socket()

# Define the port

port = 1337

# Connect to server on local machine
s.connect(("127.0.0.1", port))



s.close