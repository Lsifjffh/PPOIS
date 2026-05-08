from typing import List
from process import Process
from memory import MemoryBlock
from exceptions import ProcessNotFoundError, OutOfMemoryError

class OperatingSystem:
    def __init__(self) -> None:
        self.processes: List[Process] = []
        self.next_pid: int = 1
        self.memory: List[MemoryBlock] = [
            MemoryBlock(0, 256),
            MemoryBlock(1, 256),
            MemoryBlock(2, 512),
        ]

    def create_process(self, name: str, memory_required: int) -> Process:
        """Создать процесс с выделением памяти"""
        if memory_required <= 0:
            raise OutOfMemoryError("Memory required must be positive")

        # ищем блок, где есть место
        for block in self.memory:
            if block.free_space() >= memory_required:
                if block.allocate(self.next_pid, memory_required):
                    p = Process(self.next_pid, name)
                    p.allocated_memory = memory_required
                    p.memory_block_id = block.block_id
                    self.processes.append(p)
                    self.next_pid += 1
                    return p

        raise OutOfMemoryError(f"No free memory for {memory_required} MB")

    def terminate_process(self, pid: int) -> None:
        """Завершить процесс и освободить его память"""
        for i, p in enumerate(self.processes):
            if p.pid == pid:
                # освобождаем память в блоке
                for block in self.memory:
                    block.free_all_by_pid(pid)
                p.is_running = False
                self.processes.pop(i)
                return
        raise ProcessNotFoundError(f"Process with PID {pid} not found")

    def list_processes(self) -> List[Process]:
        return self.processes

    def show_memory(self) -> List[MemoryBlock]:
        return self.memory