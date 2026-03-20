if you want to compile more esay, you need to put this  taks.json, and launch.json files,  in your vscode configuration.

with this files the button on the top rigth corner works. (when you press the button there will be some options, choose the configuration name. (the name of the configuration changue  if you use windows, mac etc ... are in the folder "files json for compile")

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
    -o prove


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
-o prove.exe

for linux: (I dont know if works :C):

g++ -g "${PWD}/prove.cpp" \
    "${PWD}/dependecies/imgui/imgui"*.cpp \
    "${PWD}/dependecies/imgui/backends/imgui_impl_glfw.cpp" \
    "${PWD}/dependecies/imgui/backends/imgui_impl_opengl3.cpp" \
    -I "${PWD}/dependecies/imgui" \
    -I "${PWD}/dependecies/imgui/backends" \
    -I "${PWD}/dependecies/GFLW/include" \
    -lglfw -lGL -ldl -lpthread -lX11 \
    -o prove



