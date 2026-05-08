from os_core import OperatingSystem
from exceptions import ProcessNotFoundError, OutOfMemoryError

def main():
    os = OperatingSystem()
    print("=== OS Model CLI (Dynamic Memory) ===")
    print("Commands:")
    print("  create <name> <memory>  - create process")
    print("  terminate <pid>         - terminate process")
    print("  list                    - list all processes")
    print("  mem                     - show memory state")
    print("  exit                    - exit")

    while True:
        try:
            cmd = input("> ").strip().split()
            if not cmd:
                continue

            if cmd[0] == "exit":
                break

            elif cmd[0] == "create" and len(cmd) == 3:
                name = cmd[1]
                mem = int(cmd[2])
                try:
                    p = os.create_process(name, mem)
                    print(f"Created: {p}")
                except OutOfMemoryError as e:
                    print(f"Error: {e}")

            elif cmd[0] == "terminate" and len(cmd) == 2:
                pid = int(cmd[1])
                try:
                    os.terminate_process(pid)
                    print(f"Terminated PID {pid}")
                except ProcessNotFoundError as e:
                    print(f"Error: {e}")

            elif cmd[0] == "list":
                procs = os.list_processes()
                if not procs:
                    print("No processes")
                for p in procs:
                    print(f"  {p}")

            elif cmd[0] == "mem":
                blocks = os.show_memory()
                for b in blocks:
                    print(f"  {b}")

            else:
                print("Unknown command")

        except KeyboardInterrupt:
            print("\nExit")
            break
        except Exception as e:
            print(f"Unexpected error: {e}")

if __name__ == "__main__":
    main()