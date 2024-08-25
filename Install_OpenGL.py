import os
import zipfile
import shutil
import subprocess

windows_unzip_location = "C:/Temp/OPENGLLIBS"
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

    # UNZIP OPENGL LIBRARY FILES
    print("Proceeding with zipfile extraction")
    zip_file_path = os.path.join(currentWorkingDir, "OpenGL Libraries.zip")

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
    mingw32IncludeGLPath = os.path.join(mingw32IncludePath,"GL")
    mingw32IncludeGLFWPath = os.path.join(mingw32IncludePath,"GLFW")
    mingw32LibPath     = os.path.join(mingw32Path, "lib")
    
    mingw64Path        = os.path.join(mingwPath,   "x86_64-w64-mingw32")
    mingw64BinPath     = os.path.join(mingw64Path, "bin")
    mingw64IncludePath = os.path.join(mingw64Path, "include")
    mingw64IncludeGLPath = os.path.join(mingw64IncludePath,"GL")
    mingw64IncludeGLFWPath = os.path.join(mingw64IncludePath,"GLFW")
    mingw64LibPath     = os.path.join(mingw64Path, "lib")
    
    # INSTALL 32 BIT OPENGL
    if os.path.exists(mingw32Path):
        print("Installing 32-bit files to directory: " + mingw32Path)
        if not os.path.exists(mingw32IncludeGLPath):
            os.makedirs(mingw32IncludeGLPath, exist_ok=True)
        if not os.path.exists(mingw32IncludeGLFWPath):
            os.makedirs(mingw32IncludeGLFWPath, exist_ok=True)
        
        # Bin DLLs
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","bin","freeglut.dll"),mingw32BinPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","bin","Release","Win32","glew32.dll"),mingw32BinPath)
        shutil.copy(os.path.join(windows_unzip_location,"glfw-3.4.bin.WIN32","lib-mingw-w64","glfw3.dll"),mingw32BinPath)

        # Included Libraries
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","include","GL","freeglut.h"),mingw32IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","include","GL","freeglut_ext.h"),mingw32IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","include","GL","freeglut_std.h"),mingw32IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","include","GL","glut.h"),mingw32IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","include","GL","eglew.h"),mingw32IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","include","GL","glew.h"),mingw32IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","include","GL","glxew.h"),mingw32IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","include","GL","wglew.h"),mingw32IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"glfw-3.4.bin.WIN32","include","GLFW","glfw3.h"),mingw32IncludeGLFWPath)
        shutil.copy(os.path.join(windows_unzip_location,"glfw-3.4.bin.WIN32","include","GLFW","glfw3native.h"),mingw32IncludeGLFWPath)
        

        # Static Libraries
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","lib","libfreeglut.a"),mingw32LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","lib","libfreeglut_static.a"),mingw32LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","lib","Release","Win32","glew32.lib"),mingw32LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","lib","Release","Win32","glew32s.lib"),mingw32LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"glfw-3.4.bin.WIN32","lib-mingw-w64","libglfw3.a"),mingw32LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"glfw-3.4.bin.WIN32","lib-mingw-w64","libglfw3dll.a"),mingw32LibPath)
    else:
        print("MinGW32 not installed. Ignoring 32-bit files")

    # INSTALL 64 BIT OPENGL
    if os.path.exists(mingw64Path):
        print("Installing 64-bit files to directory: " + mingw64Path)
        if not os.path.exists(mingw64IncludeGLPath):
            os.makedirs(mingw64IncludeGLPath, exist_ok=True)
        if not os.path.exists(mingw64IncludeGLFWPath):
            os.makedirs(mingw64IncludeGLFWPath, exist_ok=True)
        
        # Bin DLLs
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","bin","x64","freeglut.dll"),mingw64BinPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","bin","Release","x64","glew32.dll"),mingw64BinPath)
        shutil.copy(os.path.join(windows_unzip_location,"glfw-3.4.bin.WIN64","lib-mingw-w64","glfw3.dll"),mingw64BinPath)

        # Included Libraries
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","include","GL","freeglut.h"),mingw64IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","include","GL","freeglut_ext.h"),mingw64IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","include","GL","freeglut_std.h"),mingw64IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","include","GL","glut.h"),mingw64IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","include","GL","eglew.h"),mingw64IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","include","GL","glew.h"),mingw64IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","include","GL","glxew.h"),mingw64IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","include","GL","wglew.h"),mingw64IncludeGLPath)
        shutil.copy(os.path.join(windows_unzip_location,"glfw-3.4.bin.WIN64","include","GLFW","glfw3.h"),mingw64IncludeGLFWPath)
        shutil.copy(os.path.join(windows_unzip_location,"glfw-3.4.bin.WIN64","include","GLFW","glfw3native.h"),mingw64IncludeGLFWPath)
        

        # Static Libraries
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","lib","x64","libfreeglut.a"),mingw64LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"freeglut","lib","x64","libfreeglut_static.a"),mingw64LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","lib","Release","x64","glew32.lib"),mingw64LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"glew-2.1.0","lib","Release","x64","glew32s.lib"),mingw64LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"glfw-3.4.bin.WIN64","lib-mingw-w64","libglfw3.a"),mingw64LibPath)
        shutil.copy(os.path.join(windows_unzip_location,"glfw-3.4.bin.WIN64","lib-mingw-w64","libglfw3dll.a"),mingw64LibPath)
    else:
        print("MinGW64 not installed. Ignoring 32-bit files")
    
    shutil.rmtree(windows_unzip_location)
elif os.name == "posix":
    print("Detected OS: Linux")
    print("Assuming \"sudo apt install\" format")
    try:
        # Update package lists
        subprocess.run(['sudo', 'apt', 'update'], check=True)
        
        # Install OpenGL development libraries
        subprocess.run(['sudo', 'apt', 'install', '-y', 'libglu1-mesa-dev', 'freeglut3-dev', 'mesa-common-dev'], check=True)
        
        # Install GLEW (OpenGL Extension Wrangler)
        subprocess.run(['sudo', 'apt', 'install', '-y', 'libglew-dev'], check=True)
        
        # Install GLFW (OpenGL framework)
        subprocess.run(['sudo', 'apt', 'install', '-y', 'libglfw3-dev'], check=True)
        
        # Install GLM (OpenGL Mathematics Library)
        subprocess.run(['sudo', 'apt', 'install', '-y', 'libglm-dev'], check=True)

        print("All libraries have been installed successfully!")
    
    except subprocess.CalledProcessError as e:
        print(f"An error occurred during installation: {e}")
else:
    raise Exception("Unknown OS")
