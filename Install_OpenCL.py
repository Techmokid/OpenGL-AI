import os
import sys
import zipfile
import shutil
import subprocess


GPU_TYPE = "NVIDIA"

windows_unzip_location = "C:/Temp/OPENCLLIBS"
currentWorkingDir = os.path.dirname(os.path.abspath(__file__))

def get_gcc_path():
    try:
        # Run the "where gcc" command
        result = subprocess.run(["where", "gcc"], capture_output=True, text=True, check=True)

        # Print the output (the directory where gcc is found)
        gcc_path = result.stdout.strip()  # Get the output and remove any trailing whitespace
        return gcc_path
    except subprocess.CalledProcessError:
        # Handle the case where gcc is not found
        return "GCC not found"

if os.name == "nt":
    print("Detected OS: Windows")
    gcc_path = get_gcc_path()
    if gcc_path == "GCC not found":
        raise Exception("GCC commandline tool was not found")
    print("Detected MinGW GCC installation: " + gcc_path)
    print()

    # UNZIP OPENCL LIBRARY FILES
    print("Proceeding with zipfile extraction")
    zip_file_path = os.path.join(currentWorkingDir, "OpenCL Libraries.zip")

    if os.path.exists(windows_unzip_location):
        shutil.rmtree(windows_unzip_location)
    os.makedirs(windows_unzip_location, exist_ok=True)

    with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
        zip_ref.extractall(windows_unzip_location)
    print("Unzip successful: " + windows_unzip_location)
    print()

    # GATHER DIRECTORY DATA
    mingwPath          = os.path.dirname(os.path.dirname(gcc_path))
    print("MinGW base directory: " + mingwPath)
    
    mingw32Path        = os.path.join(mingwPath,   "i686-w64-mingw32")
    mingw32BinPath     = os.path.join(mingw32Path, "bin")
    mingw32IncludePath = os.path.join(mingw32Path, "include")
    mingw32IncludeCLPath = os.path.join(mingw32IncludePath, "CL")
    mingw32LibPath     = os.path.join(mingw32Path, "lib")
    
    mingw64Path        = os.path.join(mingwPath,   "x86_64-w64-mingw32")
    mingw64BinPath     = os.path.join(mingw64Path, "bin")
    mingw64IncludePath = os.path.join(mingw64Path, "include")
    mingw64IncludeCLPath = os.path.join(mingw64IncludePath, "CL")
    mingw64LibPath     = os.path.join(mingw64Path, "lib")
    
    # INSTALL 32 BIT OPENCL
    if os.path.exists(mingw32Path):
        print("Installing 32-bit files to directory: " + mingw32Path)
        if not os.path.exists(mingw32IncludeCLPath):
            os.makedirs(mingw32IncludeCLPath, exist_ok=True)

        # Included Libraries
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl.h"),mingw32IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl.hpp"),mingw32IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl_ext.h"),mingw32IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl_gl.h"),mingw32IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl_gl_ext.h"),mingw32IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl_platform.h"),mingw32IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","opencl.h"),mingw32IncludeCLPath)

        # Static Libraries
        shutil.copy(os.path.join(windows_unzip_location,"lib","libOpenCL.so"),mingw32LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"lib","OpenCL.lib"),mingw32LibPath)
    else:
        print("MinGW32 not installed. Ignoring 32-bit files")

    # INSTALL 64 BIT OPENCL
    if os.path.exists(mingw64Path):
        print("Installing 64-bit files to directory: " + mingw64Path)
        if not os.path.exists(mingw64IncludeCLPath):
            os.makedirs(mingw64IncludeCLPath, exist_ok=True)

        # Included Libraries
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl.h"),mingw64IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl.hpp"),mingw64IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl_ext.h"),mingw64IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl_gl.h"),mingw64IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl_gl_ext.h"),mingw64IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","cl_platform.h"),mingw64IncludeCLPath)
        shutil.copy(os.path.join(windows_unzip_location,"include","CL","opencl.h"),mingw64IncludeCLPath)

        # Static Libraries
        shutil.copy(os.path.join(windows_unzip_location,"lib","libOpenCL.so"),mingw64LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"lib","OpenCL.lib"),mingw64LibPath)
    else:
        print("MinGW64 not installed. Ignoring 32-bit files")
    
    shutil.rmtree(windows_unzip_location)
elif os.name == "posix":
    gpu_info = subprocess.run(['lspci'], capture_output=True, text=True).stdout
    if 'NVIDIA' in gpu_info:
        GPU_TYPE = "NVIDIA"
    elif 'AMD' in gpu_info:
        GPU_TYPE = "AMD"
    elif 'Intel' in gpu_info:
        GPU_TYPE = "INTEL"
    else:
        print("Unknown GPU detected")
        sys.exit(1)
        
    print("Detected OS: Linux")
    print("Assuming \"sudo apt install\" format")
    
    try:
        # Update package lists
        subprocess.run(['sudo', 'apt', 'update'], check=True)

        # Install GPU-Specific Files
        if GPU_TYPE == "NVIDIA":
            subprocess.run(['sudo', 'apt', 'install', 'nvidia-opencl-dev', 'nvidia-opencl-icd'], check=True)
        elif GPU_TYPE == "INTEL":
            subprocess.run(['sudo', 'apt', 'install', 'intel-opencl-icd'], check=True)
        elif GPU_TYPE == "AMD":
            subprocess.run(['sudo', 'apt', 'install', 'rocm-opencl-dev'], check=True)
        else:
            print(f"UNSUPPORTED GPU TYPE: {GPU_TYPE}")
            sys.exit(1)

        # Install OpenCL Headers
        subprocess.run(['sudo', 'apt', 'install', 'ocl-icd-opencl-dev'], check=True)

        print("All libraries have been installed successfully!")
    
    except subprocess.CalledProcessError as e:
        print(f"An error occurred during installation: {e}")
else:
    raise Exception("Unknown OS")

print()
print()
print()
input("Press enter to close")
