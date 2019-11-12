def encrypt(message, key):
    # big endian
    # int.from_bytes()
    byte_list = [bytes(message[i:i+4]) for i in range(0, len(message)-3, 4)]
    int_list = [int.from_bytes(byte_list[i], byteorder='big') for i in range(len(byte_list))]

    for i in range(len(int_list)):
        int_list[i] ^= int.from_bytes(key, byteorder='big')
        byte_list[i] = int_list[i].to_bytes(4, byteorder='big')
            
    return b''.join(byte_list)