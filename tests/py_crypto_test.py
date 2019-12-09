import unittest
import string
from random import seed, choice, randint

class CryptoTest(unittest.TestCase):
    def _crypto(self, message):
        message = bytearray(message)
        for i in range(0, len(message)):
            message[i] = message[i] ^ self.key[i % len(self.key)]

        return message

    def _test_crypto(self, message):
        print("Message to be encrypted: ", message)
        encrypted = self._crypto(message)
        decrypted = self._crypto(encrypted)
        self.assertEqual(decrypted, message)

    @classmethod
    def setUpClass(cls):
        cls.key = b'\xde\xad\xbe\xef'
        seed()

    def setUp(self):
        print("Running Testcase:" + self._testMethodName)

    def test_simple(self):
        message = b'deadbeef'
        self._test_crypto(message)

    def test_unaligned_less(self):
        message = b'a'
        self._test_crypto(message)

    def test_unaligned_greater(self):
        message = b'deadbeefa'
        self._test_crypto(message)

    def test_random_string_lowercase(self):
        message = ''.join(choice(string.ascii_lowercase) for x in range(16)).encode('utf-8')
        self._test_crypto(message)

    def test_random_string_lowercase_random_length(self):
        message = ''.join(choice(string.ascii_lowercase) for x in range(randint(1, 100))).encode('utf-8')
        self._test_crypto(message)

    def test_random_string_uppercase(self):
        message = ''.join(choice(string.ascii_uppercase) for x in range(16)).encode('utf-8')
        self._test_crypto(message)

    def test_random_string_uppercase_random_length(self):
        message = ''.join(choice(string.ascii_uppercase) for x in range(randint(1, 100))).encode('utf-8')
        self._test_crypto(message)

if __name__ == '__main__':
    unittest.main()
