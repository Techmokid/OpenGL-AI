import platform
import subprocess
import os

def detect_environment():
    # Detect the system environment
    system = platform.system()
    
    # Check if the system is Windows
    if system == "Windows":
        return "Windows"
    
    # Check if the system is Linux (covers Ubuntu and WSL)
    elif system == "Linux":
        # Detect if running inside WSL
        if "microsoft" in platform.uname().release.lower():
            return "WSL2 Ubuntu"
        else:
            return "Ubuntu"
    else:
        return "Unknown"

def run_script():
    env = detect_environment()
    if env == "Windows":
        subprocess.run(["RunWindows.bat"], shell=True)
        print("Windows")
    elif env == "Ubuntu":
        subprocess.run(["/bin/bash", "RunUbuntu.sh"])
        print("Ubuntu")
    elif env == "WSL2 Ubuntu":
        subprocess.run(["/bin/bash", "RunWSL2.sh"])
        print("WSL2 Ubuntu")
    else:
        print("Unknown environment. No script executed.")

if __name__ == "__main__":
    run_script()
