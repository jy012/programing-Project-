if you want to compile more esay you need to put this task.json, and launch.json files in your vscode configuration.
with this files the button on the top rigth corner works. (when you press the button there will be some options, choose “Debug imgui“)

if you want to using comand use this one : 

for mac :
g++ prove.cpp \
    dependecies/imgui/imgui*.cpp \
    dependecies/imgui/backends/imgui_impl_glfw.cpp \
    dependecies/imgui/backends/imgui_impl_opengl3.cpp \
    -I dependecies/imgui \
    -I dependecies/imgui/backends \
    -I /opt/homebrew/include \
    -L /opt/homebrew/lib \
    -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -o main


for windows : (I NEED TO PROVE IT )

g++ prove.cpp ^
    dependecies/imgui/imgui*.cpp ^
    dependecies/imgui/backends/imgui_impl_glfw.cpp ^
    dependecies/imgui/backends/imgui_impl_opengl3.cpp ^
    -I dependecies/imgui ^
    -I dependecies/imgui/backends ^
    -I dependecies/GFLW/include ^
    -L dependecies/GFLW/lib-mingw-w64 ^
    -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 ^
    -o mi_programa.exe

