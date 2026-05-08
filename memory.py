from typing import List, Optional, Tuple

class MemoryBlock:
    def __init__(self, block_id: int, size: int):
        self.block_id = block_id
        self.size = size  # общий размер блока
        # список занятых участков: (начало, конец, pid)
        self.allocations: List[Tuple[int, int, int]] = []

    def free_space(self) -> int:
        """Сколько свободно в блоке"""
        used = sum(end - start for start, end, _ in self.allocations)
        return self.size - used

    def allocate(self, pid: int, requested_size: int) -> bool:
        """Выделить память, если хватает места. Вернуть True если успешно"""
        if self.free_space() < requested_size:
            return False

        # ищем первое свободное место
        if not self.allocations:
            # блок пуст
            self.allocations.append((0, requested_size, pid))
            return True

        # сортируем по началу
        self.allocations.sort(key=lambda x: x[0])

        # проверяем начало блока
        first_start = self.allocations[0][0]
        if first_start >= requested_size:
            self.allocations.insert(0, (0, requested_size, pid))
            return True

        # проверяем промежутки между выделенными участками
        for i in range(len(self.allocations) - 1):
            gap_start = self.allocations[i][1]
            gap_end = self.allocations[i + 1][0]
            gap_size = gap_end - gap_start
            if gap_size >= requested_size:
                self.allocations.insert(i + 1, (gap_start, gap_start + requested_size, pid))
                return True

        # проверяем конец блока
        last_end = self.allocations[-1][1]
        if self.size - last_end >= requested_size:
            self.allocations.append((last_end, last_end + requested_size, pid))
            return True

        return False

    def free_all_by_pid(self, pid: int) -> None:
        """Освободить всю память процесса"""
        self.allocations = [a for a in self.allocations if a[2] != pid]

    def get_allocations_for_pid(self, pid: int) -> List[Tuple[int, int]]:
        """Вернуть список участков процесса"""
        return [(start, end) for start, end, p in self.allocations if p == pid]

    def __str__(self) -> str:
        if not self.allocations:
            return f"Block {self.block_id}: {self.size} MB (free: {self.free_space()} MB)"

        used_parts = []
        for start, end, pid in self.allocations:
            used_parts.append(f"({start}-{end}: PID {pid})")
        return f"Block {self.block_id}: {self.size} MB | used: {used_parts} | free: {self.free_space()} MB"