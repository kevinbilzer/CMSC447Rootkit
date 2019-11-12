# Encryption Test #1

import encrypt

def main():
	# Encrypt
	l = encrypt.encrypt(b"\x01\x07\x00\x00\x00\x6c\x73\x20\x2f\x74\x6d\x70", b'\xde\xad\xbe\xef')
	print(l)
	print()

	# Decrypt
	l = encrypt.encrypt(l, b'\xde\xad\xbe\xef')
	print(l)
	
main()