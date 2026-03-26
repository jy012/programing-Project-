#include "imgui.h" // core  of dear imgui
#include "imgui_impl_glfw.h" // adapter for  GFLW
#include "imgui_impl_opengl3.h" // adapter for  open GFLW 3
#include <iostream>
#define GL_SILENCE_DEPRECATION // Silense of obsolet fucntions in opengl

#include <stdio.h>
#include <GLFW/glfw3.h> // librery f or create windows  adn  contexts opengl
#include <cmath>


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main() {
   
    glfwSetErrorCallback(glfw_error_callback); // error mesague displayer 
    if (!glfwInit()) return 1; // initialize gflw

    //define the vertion 
#if defined(IMGUI_IMPL_OPENGL_ES3)
    //window vertion (prove for windows)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint (GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

#elif defined(__APPLE__)
    // mac vertion 
    const char* glsl_version = "#version 150";
    glfwWindowHint (GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);


#else
    // GL 3.0 + GLSL 130 //standert vetion (mostly linux)
    const char* glsl_version = "#version 130";
    glfwWindowHint (GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only


#endif


     // window creation 
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); 
     GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "", NULL, NULL);
    if (!window) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 

// open variables 

    bool open_window1 = false;    
    bool open_prove =true ;  

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    

    // Setup scaling and syle 
     ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);      
    style.FontScaleDpi = main_scale; 
    
    //style 
    style.FrameRounding = 6.0f;


    // 3. Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 4. mian loops
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draws on the mian window 
        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        ImVec2 p = ImVec2(width-(width / 2.5f),height/2.0f);
        float thickness = 1.0f;
        static float angle = 0.0f;

        // radar draw
        angle += ImGui::GetIO().DeltaTime * 2.0f; // Velocidad
        float large = 400.0f; // large of the line 
        const int line_trail = 100;       // the lines that  make the trail
        const float rate_d = 0.01f;     // at what rate the lines  dissapear
        // animation of the trail 


        for (int i = line_trail; i > 0; i--) { 
            float old_angle = angle - (i * rate_d);

            ImVec2 p_old = ImVec2(p.x + std::cos(old_angle) * large, p.y + std::sin(old_angle) * large);
            // alpha calculate the opacacity of the lines 
            int alpha = (int)((1.0f - ((float)i / line_trail)) * 255);

            draw_list->AddLine(p, p_old, IM_COL32(0, 255, 0, alpha), 1.0f);

        }

        ImVec2 p2 = ImVec2(p.x + std::cos(angle) * large,  p.y + std::sin(angle) * large); //position and motion
        draw_list ->AddLine(p, p2, IM_COL32(0, 255, 0, 255), 2.0f);

        ImU32 color = IM_COL32(0,255,0,255);
        draw_list ->AddCircleFilled(p,5.0f,color,0); 
        draw_list ->AddCircle(p,50.0f,color,0,thickness); draw_list ->AddCircle(p,100.0f,color,0,thickness);
        draw_list ->AddCircle(p,150.0f,color,0,thickness); draw_list ->AddCircle(p,200.0f,color,0,thickness);
        draw_list ->AddCircle(p,250.0f,color,0,thickness); draw_list ->AddCircle(p,300.0f,color,0,thickness);
        draw_list ->AddCircle(p,350.0f,color,0,thickness); draw_list ->AddCircle(p,400.0f,color,0,thickness); 
 
        // menu
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(33.0f/255, 33.0f/255, 33.0f/255, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(2.0f/255, 82.0f/255, 12.0f/255, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(5.0f/255, 143.0f/255, 18.0f/255, 1.0f));
        ImGui::SetNextWindowPos(ImVec2(0,0));        
        ImGui::SetNextWindowSize(ImVec2(300,height));
        ImGui::Begin("prove",&open_prove,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize| ImGuiWindowFlags_NoMove);
        ImGui::Text("Proveeeee");
       
        
        if (ImGui::Button("halooo")){
            
        }



        ImGui::Checkbox("yes ?",&open_window1);
        ImGui::Text("width %2.0d",width);
        ImGui::Text("height %2.0d",height);


        ImGui::End();
        ImGui::PopStyleColor(3); 


        // Render
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h); // get the buffersize 
        glViewport(0, 0, display_w, display_h);// definethe renderize area 

        // define the colours in the window.
        glClearColor( 25.0f/255, 25.0f/255, 25.0f/255, 1.0f);   
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 
        glfwSwapBuffers(window);
    }

    // 5. cleaning 
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
