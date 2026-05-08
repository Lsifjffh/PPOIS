import unittest
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from os_core import OperatingSystem
from exceptions import ProcessNotFoundError, OutOfMemoryError

class TestOSModel(unittest.TestCase):
    def setUp(self):
        self.os = OperatingSystem()

    def test_create_process(self):
        p = self.os.create_process("test", 100)
        self.assertEqual(len(self.os.list_processes()), 1)
        self.assertEqual(p.allocated_memory, 100)

    def test_terminate_process(self):
        p = self.os.create_process("test", 100)
        self.os.terminate_process(p.pid)
        self.assertEqual(len(self.os.list_processes()), 0)

    def test_terminate_not_found(self):
        with self.assertRaises(ProcessNotFoundError):
            self.os.terminate_process(999)

    def test_out_of_memory(self):
        with self.assertRaises(OutOfMemoryError):
            self.os.create_process("too_big", 900)

    def test_multiple_processes_in_one_block(self):
        p1 = self.os.create_process("first", 100)
        p2 = self.os.create_process("second", 100)
        # оба должны быть в блоке 0 (256 MB)
        self.assertEqual(p1.memory_block_id, 0)
        self.assertEqual(p2.memory_block_id, 0)
        mem = self.os.show_memory()
        # в блоке 0 должно быть 100+100 = 200 занято, 56 свободно
        self.assertEqual(mem[0].free_space(), 56)

if __name__ == "__main__":
    unittest.main()