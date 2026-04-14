For compile please use the next comands in the treminal (the compilation will takke 20 to 30 seconds for the use alot of files)
if you want to compile more easy, you need to put this  task.json, and launch.json files,  in your vscode configuration. (some paths of the compiler need to be chaged, depended of where you storage the compiler )
make sure in windows is open with CDM. 

got to Go to the Terminal menu at the top and select Configure Tasks...
Select the option Create tasks.json file from template copy and paste the task.json and the launch.json 



for mac : (you need home brew for this one ):
/usr/bin/g++ -std=c++20 -g RadarApp.cpp \
dependencies/imgui/imgui.cpp \
dependencies/imgui/imgui_draw.cpp \
dependencies/imgui/imgui_tables.cpp \
dependencies/imgui/imgui_widgets.cpp \
dependencies/imgui/backends/imgui_impl_glfw.cpp \
dependencies/imgui/backends/imgui_impl_opengl3.cpp \
-Idependencies/imgui \
-I"dependencies/headers and backends" \
-Idependencies/GFLW/include \
-Idependencies/imgui/backends \
-I/opt/homebrew/include \
-L/opt/homebrew/lib \
-lglfw \
-framework OpenGL \
-framework Cocoa \
-framework IOKit \
-framework CoreVideo \
-o RadarApp

    
With out homwbrew  :
g++ -std=c++20 -fdiagnostics-color=always -g \
    RadarApp.cpp \
    dependencies/imgui/imgui.cpp \
    dependencies/imgui/imgui_draw.cpp \
    dependencies/imgui/imgui_tables.cpp \
    dependencies/imgui/imgui_widgets.cpp \
    dependencies/imgui/backends/imgui_impl_glfw.cpp \
    dependencies/imgui/backends/imgui_impl_opengl3.cpp \
    -I"dependencies/headers and backends" \
    -Idependencies/imgui \
    -Idependencies/imgui/backends \
    -Idependencies/GFLW/include \
    -lglfw \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -o RadarApp

for windows : 

g++ -std=c++20 -fdiagnostics-color=always -g `
    RadarApp.cpp `
    dependencies/imgui/imgui.cpp `
    dependencies/imgui/imgui_draw.cpp `
    dependencies/imgui/imgui_tables.cpp `
    dependencies/imgui/imgui_widgets.cpp `
    dependencies/imgui/backends/imgui_impl_glfw.cpp `
    dependencies/imgui/backends/imgui_impl_opengl3.cpp `
    -I"dependencies/headers and backends" `
    -I"dependencies/imgui" `
    -I"dependencies/imgui/backends" `
    -I"dependencies/GFLW/include" `
    -L"dependencies/GFLW/glfw-3.4.bin.WIN64/lib-mingw-w64" `
    -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -lwinmm -limm32 -lcomdlg32 `
    -o RadarApp.exe

for linux: (Ubuntu, Debian, Fedora, etc.):

gg++ -std=c++20 -fdiagnostics-color=always -g \
    RadarApp.cpp \
    dependencies/imgui/imgui.cpp \
    dependencies/imgui/imgui_draw.cpp \
    dependencies/imgui/imgui_tables.cpp \
    dependencies/imgui/imgui_widgets.cpp \
    dependencies/imgui/backends/imgui_impl_glfw.cpp \
    dependencies/imgui/backends/imgui_impl_opengl3.cpp \
    -I"dependencies/headers and backends" \
    -Idependencies/imgui \
    -Idependencies/imgui/backends \
    -Idependencies/GFLW/include \
    -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl \
    -o RadarApp


