class Process:
    def __init__(self, pid: int, name: str):
        self.pid = pid
        self.name = name
        self.is_running: bool = True

    def __str__(self) -> str:
        status = "running" if self.is_running else "terminated"
        return f"PID {self.pid}: {self.name} [{status}]"