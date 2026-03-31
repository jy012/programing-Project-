#include "imgui.h" // core  of dear imgui
#include "imgui_impl_glfw.h" // adapter for  GFLW
#include "imgui_impl_opengl3.h" // adapter for  open GFLW 3
#include <iostream>
#include <imgui_internal.h>
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
   style.WindowRounding= 8.0f;



    // 3. initialize backends
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
        ImGui::SetNextWindowSize(ImVec2(340,height));
        // configuration of the header 
        
        ImGui::Begin("menu",NULL,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize| ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoTitleBar);{
            
            // configuration of the header 
            bool estaActiva = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows); // know if we focus in this window 
            ImVec4 colorHeader = estaActiva 
                ? ImVec4(5.0f/255, 143.0f/255, 18.0f/255, 1.0f)   
                : ImVec4(2.0f/255, 82.0f/255, 12.0f/255, 1.0f); 


             ImVec2 padding = ImGui::GetStyle().WindowPadding;  // this take of the padding 
            ImGui::PushStyleColor(ImGuiCol_ChildBg,ImVec4(5.0f/255, 143.0f/255, 18.0f/255, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ChildBg, colorHeader);
            ImGui::SetCursorPos(ImVec2(0,0));
            if (ImGui::BeginChild("Header",ImVec2(340, 40),false)){
                const char* title = "Objects in the air";
               ImVec2 textSize = ImGui::CalcTextSize(title);
               float posX = (340 - textSize.x) * 0.5f;
               float posY = (30.0f - textSize.y) * 0.5f;
               ImGui::SetCursorPos(ImVec2(posX, posY));
               ImGui::Text("%s",title);
            }
            ImGui::EndChild();
            ImGui::PopStyleColor(2);


            // buttons menu
           
        }
        
        ImGui::End();
        ImGui::PopStyleColor(3);

         // buttons menu
        ImGui::SetNextWindowPos(ImVec2(340.0f,height/2.0f));  
        ImGui::SetNextWindowSize(ImVec2(50,200));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(81.0f/255, 81.0f/255, 81.0f/255, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg,ImVec4(33.0f/255, 33.0f/255, 33.0f/255, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(33.0f/255, 33.0f/255, 33.0f/255, 1.0f)); // normal color
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f)); //  when they dected the mouse
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.1f, 0.1f, 0.1f, 1.0f)); // whent they do click 
           
        if (ImGui::Begin("buttons",NULL,ImGuiChildFlags_Borders|ImGuiWindowFlags_NoScrollbar)){
                
              // made buttons have no space with each other 
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                float total_width = ImGui::GetContentRegionAvail().x;
                float total_height = ImGui::GetContentRegionAvail().y;
    
                 // Dividimos el ancho total entre 4
                ImVec2 size_Button = ImVec2(total_width , total_height/4.0f);

                if (ImGui::Button("X", size_Button)) { /* ... */ }
               
                 if (ImGui::Button("Y", size_Button)) { /* ... */ }
                 
                if (ImGui::Button("Z", size_Button)) { /* ... */ }
           
                 if (ImGui::Button("W", size_Button)) { /* ... */ }

                ImGui::PopStyleVar();

            }

            ImGui::End();
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(5);



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
