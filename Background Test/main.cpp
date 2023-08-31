#include <windows.h>
#include <iostream>
#include<filesystem>

void SetWallpaper(std::string path) {
	if(!std::filesystem::exists(path)) {
		std::cout << "Wallpaper path does not exist!";
		return;
	}
	
    // Convert std::string to a C-style string
    const char* cPath = path.c_str();

    // Use SystemParametersInfo to set the wallpaper
    int result = SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*) cPath, SPIF_UPDATEINIFILE);

    if (result) {
        std::cout << "Wallpaper set\n";
    } else {
        std::cout << "Wallpaper not set\n";
        std::cout << "SPI returned " << result << '\n';
    }
}

int main() {
    SetWallpaper("C:/Users/aj200/Documents/GitHub/OpenGL-AI/Background Test/wallpaper.png");
    return 0;
}