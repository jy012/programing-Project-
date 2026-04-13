
#include "imgui.h"              // core  of dear imgui
#include "imgui_impl_glfw.h"    // adapter for  GFLW
#include "imgui_impl_opengl3.h" // adapter for  open GFLW 3
#include <imgui_internal.h>
#define GL_SILENCE_DEPRECATION // Silense of obsolet fucntions in opengl
#include <GLFW/glfw3.h>        // librery f or create windows  adn  contexts opengl
// c++ libraries
#include <iostream>
#include <format>
#include <array>
#include <vector>
#include <cmath>
#include <thread>
#include <stdio.h>
#include <cmath>
#include <string>
#include <thread>
#include <chrono>
#include <mutex> // for protect the thread of the clean button
// Radar Libraries  and extras
#include "Radar.h"
#include "Position.h"
#include "Plane.h"
#include "Functions.h"
#include <future>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// libraries ofr upload images
#if defined(__APPLE__)
// no libraries need it
#else
#include "glcorearb.h"

#endif

using namespace std;

bool running = true;
mutex plane_mutex;
vector<Plane *> planeList; // list of all planes
static int counterID = 0;
static bool show_plane_menu = false;
static bool show_bulk_menu = false;
static bool show_clean_menu = false;
static bool show_settings_menu = false;

size_t detector_newplane = planeList.size();

void Plane_generator()
{
    string ID = format("A {}", counterID);
    lock_guard<std::mutex> lock(plane_mutex);
    planeList.push_back(new Plane(ID));
}

// // auxiliar function for uplowd a texture to opnegl from a file
bool LoadTextureFromFile(const char *filename, GLuint *out_texture, int *out_width, int *out_height)
{
    // Cargar imagen
    int image_width, image_height, image_channels;
    unsigned char *image_data = stbi_load(filename, &image_width, &image_height, &image_channels, 4);
    if (image_data == NULL)
        return false;

    // Crear textura de OpenGL
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Configuración necesaria para que la imagen se vea bien
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Subir píxeles a la GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

Position p(0, 0, 0);
long long range = 100000;
int minDeg = 0;
int maxDeg = 360;
int elevation = 40;
int degPerSec = 30;
int gain = 34;

Radar radar(
    p,         // position
    minDeg,    // minimum angle
    maxDeg,    // maximum angle
    range,     // range
    elevation, // elevation
    degPerSec, // degrees per second
    gain       // gain (dBi)
);

// variables for create a personalized aiplane
string name_airplane;
double vx;
double vy;
double vz;
double fp_x;
double fp_y;
double fp_z;
Position pos_airplane;

void Plane_generator_custom(string name, double Vx, double Vy, double Vz, double tp_x, double tp_y, double tp_z, Position pos)
{
    pos.set_position(fp_x, fp_y, fp_z);
    lock_guard<std::mutex> lock(plane_mutex);
    planeList.push_back(new Plane(name, Vx, Vy, Vz, pos));
}

void Radar_loop()
{
    Position origin = radar.getPosition(); // radar position (never changes)

    while (running)
    {                                                                                                // makes it loop forever
        std::this_thread::sleep_for(std::chrono::milliseconds(1));                                   // without this, it runs so fast that the degrees turned is so close to 0 it gets rounded to 0 and the radar never actually turns
        auto nowTime = chrono::steady_clock::now();                                                  // get the current time
        auto duration = chrono::duration_cast<std::chrono::milliseconds>(nowTime - radar.getTime()); // get the time between now and the last time the radar turned (should always be about 1 millisecond)
        radar.updateTime(nowTime);                                                                   // update the time the radar last turned

        double currentDeg = radar.getDeg();                            // get the currente radar angle
        double dPS = radar.getDPS();                                   // get how fast the radar turns
        double seconds = static_cast<double>(duration.count()) / 1000; // convert the duration from milliseconds to seconds
        double degreesTurned = seconds * dPS;                          // get how many degrees the radar turned

        radar.updateDeg(currentDeg + degreesTurned); // update current degree of radar

        {
            lock_guard<std::mutex> lock(plane_mutex);
            for (int p{0}; p < planeList.size(); p++)
            {
                // loop through planes{}
                planeList[p]->positionUpdate(nowTime); // update the plane position ba{}sed on its speed and length of time since last update
                // double xp, yp, zp;
                // planeList[p].getPosition(xp, yp, zp);
                // Position pos = {xp, yp, zp}; //position of the plane
                Position pos = planeList[p]->getPosition();
                // cout << pos.toString() << endl;

                double planeMagnitude = distanceMagnitude(radar.getPosition(), pos); // distance from the radar to the plane
                double planeFlatDeg = relativeFlatAngle_D(pos, radar);     // calculate flat angle
                double planeHeightDeg = relativeHeightAngle_D(pos, radar); // calculate height angle
                bool planeDetected = false;                                          // this is just to prevent having to call the the printPlaneDetected twice for both edge cases
                if (planeHeightDeg <= radar.getElevation())
                {
                    if (planeMagnitude <= radar.getRange())
                    {                                                       // check if plane is within range
                        
                        if ((currentDeg + degreesTurned) > 360)
                        {                                                         // for special case (e.g. 300 + 70 = 370, without extra logic it wont detect planes within that 0-10 degree window)
                            double extraDeg = (currentDeg + degreesTurned) - 360; // this wont factor in if it did a full circle more than once, but that is in all likelihood not possible
                            if (extraDeg >= currentDeg)
                            { // radar has pinged every angle
                                planeDetected = true;
                            }
                        }
                        else
                        { // if not special case
                            if (planeFlatDeg >= currentDeg && planeFlatDeg <= (currentDeg + degreesTurned))
                            { // if plane lies within the degrees the radar has scanned
                                planeDetected = true;
                            }
                        }
                    }
                }

                if (planeDetected)
                {
                    double SNR = calculateSNR(pos, radar); // calculate the Signal to Noise ratio
                    planeList[p]->info = radar.printPlaneDetected_final(pos, planeFlatDeg, planeHeightDeg, planeMagnitude, SNR, rangeAccuracy(SNR, radar), angularAccuracy(SNR, radar));
                    planeList[p]->lastDetectedTime = std::chrono::steady_clock::now();
                    planeList[p]->lastDetectedAngle = planeFlatDeg;
                    planeList[p]->lastDetectedDistance = planeMagnitude;
                    planeList[p]->isVisible = true;
                }
            }
        }
    }
}

ImVec2 GetDirectiont(ImVec2 from, ImVec2 to)
{
    ImVec2 d = {to.x - from.x, to.y - from.y};
    double len = sqrt(d.x * d.x + d.y * d.y);
    if (len > 0.0001)
    {
        d.x /= len;
        d.y /= len;
    }
    return d;
}

static bool isPickingPosition = false;
float manual_vz = 0;
static int pickingStep = 0;
static Position startPos(0, 0, 0);
static float manual_speed = 200.0f; // Magnitud de velocidad deseada
static float manual_z_pos = 0;

void HandleRadarTwoClicks(ImDrawList *draw_list, ImVec2 center, float radius, long long range)
{
    if (!isPickingPosition || pickingStep == 0)
        return;

    ImGuiIO &io = ImGui::GetIO();
    ImVec2 mousePos = io.MousePos;

    // mouse distance bettewn the center of the raar
    float dx = mousePos.x - center.x;
    float dy = mousePos.y - center.y;

    float distPx = sqrtf(dx * dx + dy * dy);
    bool mouseOverRadar = (distPx <= radius);

    if (mouseOverRadar)
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    // convert pilex to meters
    double scale = (double)range / (double)radius;

    // drawing
    if (pickingStep == 1)
    {
        if (mouseOverRadar)
        {
            draw_list->AddCircleFilled(mousePos, 6.0f, IM_COL32(0, 255, 255, 150)); // made it dynamic 
        }
    }
    else if (pickingStep == 2)
    {
        // convert the position to pixels
        ImVec2 originPx = ImVec2(
            center.x + (float)(startPos.getX() / scale),
            center.y + (float)(startPos.getY() / scale));

        draw_list->AddCircleFilled(originPx, 6.0f, IM_COL32(0, 255, 255, 255)); // made it dynamic 

        if (mouseOverRadar)
        {
            draw_list->AddLine(originPx, mousePos, IM_COL32(0, 255, 255, 200), 2.0f);
        }
        else
        {
            // blue line draw
            float angleToMouse = atan2f(dy, dx);
            ImVec2 edgePoint = ImVec2(center.x + cosf(angleToMouse) * radius, center.y + sinf(angleToMouse) * radius);
            draw_list->AddLine(originPx, edgePoint, IM_COL32(0, 255, 255, 100), 2.0f);
        }
    }

    // process clicks
    if (mouseOverRadar && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
    {
        // Conversión directa: Píxel relativo -> Mundo
        double worldX = (double)dx * scale;
        double worldY = (double)dy * scale;

        if (pickingStep == 1)
        {
            startPos.set_position(worldX, worldY, (double)manual_z_pos);
            pickingStep = 2;
        }
        else if (pickingStep == 2)
        {
            double dirX = worldX - startPos.getX();
            double dirY = worldY - startPos.getY();
            double distance = sqrt(dirX * dirX + dirY * dirY);

            if (distance > 0.1)
            {

                double vx = (dirX / distance) * (double)manual_speed;
                double vy = (dirY / distance) * (double)manual_speed;

                counterID++;
                string name = format("A-T-{}", counterID);

                Plane *newPlane = new Plane(name, vx, vy, (double)manual_vz, startPos);

                // Data before the firts scan
                newPlane->lastDetectedTime = std::chrono::steady_clock::now();
                double angleRad= atan2(startPos.getY(), startPos.getX());
                double angleDeg = angleRad * (180.0/pi);
                
                if (angleDeg < 0)
                    angleDeg += 360.0; // Rango 0-360 coherente con Radar_loop

                newPlane->lastDetectedAngle = (float)angleDeg;
                newPlane->lastDetectedDistance = (float)sqrt(pow(startPos.getX(), 2) + pow(startPos.getY(), 2));
                newPlane->isVisible = true;
                newPlane->info = "Manual placement";

                {
                    lock_guard<std::mutex> lock(plane_mutex);
                    planeList.push_back(newPlane);
                }
            }

            pickingStep = 0;
            isPickingPosition = false;
            show_plane_menu = false;
        }
    }
}

void window_style()
{
    // window colors
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(33.0f / 255, 33.0f / 255, 33.0f / 255, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(2.0f / 255, 82.0f / 255, 12.0f / 255, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(5.0f / 255, 143.0f / 255, 18.0f / 255, 1.0f));

    // buttons colors
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(2.0f / 255, 100.0f / 255, 12.0f / 255, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(5.0f / 255, 143.0f / 255, 18.0f / 255, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(10.0f / 255, 180.0f / 255, 25.0f / 255, 1.0f));
    // background  slider colors
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(15.0f / 255, 15.0f / 255, 15.0f / 255, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(20.0f / 255, 20.0f / 255, 20.0f / 255, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(25.0f / 255, 25.0f / 255, 25.0f / 255, 1.0f));

    // grab colors
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(2.0f / 255, 100.0f / 255, 12.0f / 255, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(10.0f / 255, 180.0f / 255, 25.0f / 255, 1.0f));
}

void F_style()
{

    ImGui::PopStyleColor(11);
}

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main()
{
    static int planes_to_add = 1;

    glfwSetErrorCallback(glfw_error_callback); // error mesage displayer
    if (!glfwInit())
        return 1; // initialize gflw

    // define the vertion
#if defined(IMGUI_IMPL_OPENGL_ES3)
    // window vertion (prove for windows)
    const char *glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

#elif defined(__APPLE__)
    // mac vertion
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

#else
    // GL 3.0 + GLSL 130 //standert vetion (mostly linux)
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

#endif

    // window creation
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    GLFWwindow *window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }
    // images
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    GLuint iconX, iconY, iconZ, iconW, iconMin, iconMax;
    int my_width, my_height;

    bool retX = LoadTextureFromFile("dependencies/icons/Agregar.png", &iconX, &my_width, &my_height);
    bool retY = LoadTextureFromFile("dependencies/icons/Random.png", &iconY, &my_width, &my_height);
    bool retZ = LoadTextureFromFile("dependencies/icons/Eliminar.png", &iconZ, &my_width, &my_height);
    bool retW = LoadTextureFromFile("dependencies/icons/Ajustes.png", &iconW, &my_width, &my_height);
    bool inMI = LoadTextureFromFile("dependencies/icons/Flecha izquierda.png", &iconMin, &my_width, &my_height);
    bool inMA = LoadTextureFromFile("dependencies/icons/Flecha derecha.png", &iconMax, &my_width, &my_height);

    // open bool variables

    bool open_window1 = false;
    static bool no_collapsed = true;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup scaling and syle
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // 3. initialize backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 4. mian loop
    thread radarThread(Radar_loop);

    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // for erase the planes

        // Draws on the main window
        ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        static ImDrawListSplitter splitter;
        // chanel 1  or cape 1
        splitter.Split(draw_list, 3);
        ImVec2 p = ImVec2(width * 0.6f, height * 0.5f);
        float thickness = 1.0f;
        static float angle = 0.0f;

        // radar draw animation
        // David the line is created

        float visualAngle = (float)radar.getDeg();
        float radians = visualAngle * (IM_PI / 180.0f);

        // velocity and direction
        float large = height * 0.45; // large of the line
        const int line_trail = 100;  // the lines that  make the trail
        const float rate_d = 0.5f;   // at what rate the lines  dissapear

        // animation of the trail  variables
        float radius = (height * 0.45f);
        splitter.SetCurrentChannel(draw_list, 2);
        HandleRadarTwoClicks(draw_list, p, large, radar.getRange());

        // plane drawing
        auto now = std::chrono::steady_clock::now();
        for (Plane *plane : planeList)
        {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - plane->lastDetectedTime);

            if (duration.count() < 2000)
            {

                float angleRad = plane->lastDetectedAngle * (3.14159f / 180.0f);

                // ser the plane distance
                float distPixels = (plane->lastDetectedDistance / radar.getRange()) * (height * 0.45f);

                // Calculate the finalposition
                ImVec2 blipPos = ImVec2(
                    p.x + std::cos(angleRad) * distPixels,
                    p.y + std::sin(angleRad) * distPixels);

                // 4. Draw
                float alpha = 1.0f - (duration.count() / 2000.0f);
                draw_list->AddCircleFilled(blipPos, (width * 0.035f) / 5.0f, IM_COL32(255, 0, 0, (int)(alpha * 255)));
            }
        }

        splitter.SetCurrentChannel(draw_list, 1); // cape 2 or chanel 2
        // radar draw animation
        for (int i = line_trail; i > 0; i--)
        {

            float trailAngle = visualAngle - (i * rate_d);
            float trailRad = trailAngle * (IM_PI / 180.0f);

            ImVec2 p_old = ImVec2(p.x + std::cos(trailRad) * large, p.y + std::sin(trailRad) * large);
            // alpha calculate the opacacity of the lines
            int alpha = (int)((1.0f - ((float)i / line_trail)) * 255);

            draw_list->AddLine(p, p_old, IM_COL32(0, 255, 0, alpha), 1.0f);
        }

        // draw the mian line
        ImVec2 p2 = ImVec2(p.x + std::cos(radians) * large, p.y + std::sin(radians) * large); // position and motion
        draw_list->AddLine(p, p2, IM_COL32(0, 255, 0, 255), 2.0f);

        float constant_height = height * 0.5f;

        // Circles

        ImU32 color = IM_COL32(0, 255, 0, 255);

        for (int i = 1; i <= 8; i++)
        {
            float multiplier = (i * 0.125f);
            float f_radius = radius * multiplier;
            std::string distanceText = std::format("{:.1f}k", i * 12.5f);
            ImVec2 textSize = ImGui::CalcTextSize(distanceText.c_str());

            ImVec2 posH = ImVec2(p.x - f_radius - (textSize.x / 2.0f), p.y - 20.0f);
            draw_list->AddText(posH, color, distanceText.c_str());

            ImVec2 posV = ImVec2(p.x - textSize.x - 10.0f, p.y - f_radius - (textSize.y / 2.0f));
            draw_list->AddText(posV, color, distanceText.c_str());

            draw_list->AddCircle(p, f_radius, color, 0, thickness);
        }

        // draw for teup the position

        float H1 = (constant_height - (radius));
        float H2 = (constant_height + (radius));

        float L1 = ((width * 0.6f) - (radius));
        float L2 = ((width * 0.6f) + (radius));

        draw_list->AddLine(ImVec2((width * 0.6f), H1), ImVec2((width * 0.6f), H2), IM_COL32(0, 255, 0, 255), 2.0f);
        draw_list->AddLine(ImVec2(L1, (height * 0.5f)), ImVec2(L2, (height * 0.5f)), IM_COL32(0, 255, 0, 255), 2.0f);

        float green_line = (large * 2);

        // Last chanel or last cape
        splitter.SetCurrentChannel(draw_list, 0);

        // horizontal lines
        for (int i = 0; i <= 8; i++)
        {
            float multiplier_y1 = 1.0f - (i * 0.125f);
            float y1 = (constant_height - (radius * multiplier_y1)) - 2.0f;
            draw_list->AddLine(ImVec2(0.0f, y1), ImVec2(width, y1), IM_COL32(255, 0, 0, 50), 2.0f);
        }

        for (int i = 8; i >= 1; i--)
        {
            float multiplier = (i / 8.0f);
            float y2 = (constant_height + (radius * multiplier)) + 2.0f;
            draw_list->AddLine(ImVec2(0.0f, y2), ImVec2(width, y2), IM_COL32(255, 0, 0, 50), 2.0f);
        }

        // vertical lines
        for (int i = 0; i <= 18; i++)
        {
            float multiplier_x1 = (i * 0.125f);
            float x1 = ((width * 0.6f) - (radius * multiplier_x1)) - 2.0f;
            draw_list->AddLine(ImVec2(x1, 0.0f), ImVec2(x1, height), IM_COL32(255, 0, 0, 50), 2.0f);
        }

        for (int i = 1; i <= 18; i++)
        {
            float multiplier_x1 = (i * 0.125f);
            float x1 = ((width * 0.6f) + (radius * multiplier_x1)) + 2.0f;
            draw_list->AddLine(ImVec2(x1, 0.0f), ImVec2(x1, height), IM_COL32(255, 0, 0, 50), 2.0f);
        }

        splitter.Merge(draw_list); // conbines all the capes of drawing

        if (no_collapsed)
        {
            // menu
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(33.0f / 255, 33.0f / 255, 33.0f / 255, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(2.0f / 255, 82.0f / 255, 12.0f / 255, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(5.0f / 255, 143.0f / 255, 18.0f / 255, 1.0f));
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2((width * 0.22f), height));
            // configuration of the header

            ImGui::Begin("menu", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
            {

                // configuration of the header
                bool estaActiva = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows); // know if we focus in this window
                ImVec4 colorHeader = estaActiva
                                         ? ImVec4(5.0f / 255, 143.0f / 255, 18.0f / 255, 1.0f)
                                         : ImVec4(2.0f / 255, 82.0f / 255, 12.0f / 255, 1.0f);

                // header

                ImVec2 padding = ImGui::GetStyle().WindowPadding; // this take of the padding
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(5.0f / 255, 143.0f / 255, 18.0f / 255, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ChildBg, colorHeader);
                ImGui::SetCursorPos(ImVec2(0, 0));

                if (ImGui::BeginChild("Header", ImVec2(width * 0.22f, height * 0.05f), false))
                {
                    const char *title = "Objects in the air";
                    ImVec2 textSize = ImGui::CalcTextSize(title);
                    float posX = (width * 0.22f - textSize.x) * 0.5f;
                    float posY = (height * 0.05f - textSize.y) * 0.5f;
                    ImGui::SetCursorPos(ImVec2(posX, posY));
                    ImGui::Text("%s", title);
                }
                ImGui::EndChild();
                ImGui::PopStyleColor(2);

                if (!planeList.empty())
                {

                    for (size_t i = 0; i < planeList.size(); i++)
                    {
                        std::string title = "Plane #" + std::to_string(i + 1);
                        ImGui::PushID((int)i);

                        ImVec4 verdeNormal = ImVec4(2.0f / 255, 100.0f / 255, 12.0f / 255, 1.0f);
                        ImVec4 verdeHover = ImVec4(2.0f / 255, 82.0f / 255, 12.0f / 255, 1.0f);
                        ImVec4 verdeActive = ImVec4(5.0f / 255, 143.0f / 255, 18.0f / 255, 1.0f);

                        ImGui::PushStyleColor(ImGuiCol_Header, verdeNormal);
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, verdeHover);
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, verdeActive);
                        bool open_collap = ImGui::CollapsingHeader(title.c_str());
                        ImGui::PopStyleColor(3);

                        if (open_collap)
                        {
                            Plane *plane = planeList[i];
                            // get the the last time was the plane detected
                            auto now = std::chrono::steady_clock::now();
                            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - plane->lastDetectedTime);
                            long long totalSeconds = duration.count();
                            int minutes = (int)(totalSeconds / 60);
                            int seconds = (int)(totalSeconds % 60);

                            Position pos = plane->getPosition();

                            ImGui::TextWrapped("ID: %zu", i);
                            ImGui::TextWrapped("Position: %s", pos.showpos().c_str());
                            ImGui::TextWrapped("Radar: %s", plane->info.c_str());
                            ImGui::TextWrapped("last time localised: %02d:%02d", minutes, seconds);

                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(2.0f / 255, 100.0f / 255, 12.0f / 255, 1.0f));
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(5.0f / 255, 143.0f / 255, 18.0f / 255, 1.0f));
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f / 255, 180.0f / 255, 25.0f / 255, 1.0f));
                            bool deletePressed = ImGui::Button("Delete");
                            ImGui::PopStyleColor(3);
                            if (deletePressed)
                            {
                                std::lock_guard<std::mutex> lock(plane_mutex);
                                delete planeList[i];
                                planeList.erase(planeList.begin() + i);
                                ImGui::PopID();
                                break;
                            }
                        }
                        ImGui::PopID();
                    }
                }
            }
            ImGui::End();
            ImGui::PopStyleColor(3);
        }

        // buttons menu

        ImVec2 buttons_position_1(width * 0.22f, height * 0.05f);
        ImVec2 buttons_position_2(width * 0, height * 0.05f);
        ImVec2 final_buttons_position(0.0f, 0.0f);

        if (no_collapsed)
        {
            final_buttons_position = buttons_position_1;
        }
        else
        {
            final_buttons_position = buttons_position_2;
        }
        ImVec2 colllapsed_button_position((final_buttons_position.x) + (width * 0.035f), width * 0.070f);

        ImGui::SetNextWindowPos(final_buttons_position);
        ImGui::SetNextWindowSize(ImVec2(width * 0.035f, (width * 0.035f) * 4.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 3.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(81.0f / 255, 81.0f / 255, 81.0f / 255, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(33.0f / 255, 33.0f / 255, 33.0f / 255, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(33.0f / 255, 33.0f / 255, 33.0f / 255, 1.0f)); // normal color
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));               //  when they dected the mouse
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));                // whent they do click

        if (ImGui::Begin("buttons", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
        {

            // made buttons have no space with each other
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            float total_width = ImGui::GetContentRegionAvail().x;
            float total_height = ImGui::GetContentRegionAvail().y;
            ImVec2 button_size = ImVec2(total_width, total_height / 4); // scale for the icons  the icons
            ImVec2 iconSize = ImVec2(radius * (1.1f * 0.125f), radius * (1.1f * 0.125f));

            float padX = (button_size.x - iconSize.x) / 2.0f;
            float padY = (button_size.y - iconSize.y) / 2.0f;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padX, padY));

            if (ImGui::ImageButton("X", (ImTextureID)(intptr_t)iconX, iconSize))
            {
                show_plane_menu = !show_plane_menu;
            }

            if (ImGui::ImageButton("Y", (ImTextureID)(intptr_t)iconY, iconSize))
            {
                show_bulk_menu = !show_bulk_menu;
            }

            if (ImGui::ImageButton("Z", (ImTextureID)(intptr_t)iconZ, iconSize))
            {
                show_clean_menu = !show_clean_menu;
            }

            if (ImGui::ImageButton("W", (ImTextureID)(intptr_t)iconW, iconSize))
            {
                show_settings_menu = !show_settings_menu;
            }

            ImGui::PopStyleVar(2);
        }

        ImGui::End();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);

        // buttons functions
        if (show_bulk_menu)
        {
            ImGui::SetNextWindowPos(ImVec2(width * 0.40f, height * 0.40f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(radius * (5 * 0.125f), radius * (2.8 * 0.125f))); //
            window_style();
            ImGui::Begin("randomizer generator", &show_bulk_menu, ImGuiWindowFlags_NoResize);
            ImGui::Text("choose the number of airplanes:");
            ImGui::SliderInt("##number", &planes_to_add, 1, 100); // Slider de 1 a 100

            if (ImGui::Button("okey", ImVec2(-FLT_MIN, 0)))
            {
                for (int i = 0; i < planes_to_add; i++)
                {
                    Plane_generator();
                    counterID++;
                }
                show_bulk_menu = false;
            }

            if (ImGui::Button("cancel", ImVec2(-FLT_MIN, 0)))
            {
                show_bulk_menu = false;
            }

            ImGui::End();
            F_style();
        }
        static char bufferNombre[100] = ""; // maximun charaters for the name

        if (show_plane_menu)
        {
            ImGui::SetNextWindowPos(ImVec2(width * 0.5f, height * 0.45f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(radius * (7.5 * 0.125f), radius * (5.2 * 0.125f)));
            window_style();
            ImGui::Begin("Plane generator", &show_plane_menu, ImGuiWindowFlags_NoResize);

            ImGui::Text("put settings for the airplane:");
            ImGui::Text("1. define the velocity (units/s):");
            ImGui::SliderFloat("Total velocity ", &manual_speed, 10.0f, 600.0f);
            ImGui::SliderFloat("Velocity Z", &manual_vz, -100.0f, 100.0f);
            ImGui::Separator();
            ImGui::Text("2. initial altitude :");
            ImGui::SliderFloat("position Z", &manual_z_pos, 0.0f, 10000.0f);

            ImGui::Spacing();

            if (pickingStep == 0)
            {
                if (ImGui::Button("Define trayectory", ImVec2(-FLT_MIN, 40)))
                {
                    isPickingPosition = true;
                    pickingStep = 1;
                }
            }
            else
            {
                string msg = (pickingStep == 1) ? "made click for put started point  ": "made it the clikc for the destiny ";
                ImGui::TextColored(ImVec4(0, 1, 1, 1), ">>> %s <<<", msg.c_str());
                if (ImGui::Button("return", ImVec2(-FLT_MIN, 0)))
                {
                    pickingStep = 0;
                    isPickingPosition = false;
                }
            }

            if (ImGui::Button("cancel", ImVec2(-FLT_MIN, 0)))
            {
                show_plane_menu = false;
                isPickingPosition = false;
            }

            ImGui::End();
            F_style();
        }

        if (show_clean_menu)
        {
            ImGui::SetNextWindowPos(ImVec2(width * 0.40f, height * 0.65), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(radius * (6 * 0.125f), radius * (2.2 * 0.125f)));
            window_style();
            ImGui::Begin("Comfirm clean", &show_clean_menu, ImGuiWindowFlags_NoResize);
            ImGui::TextWrapped("you want to erase all the planes ?");
            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
            if (ImGui::Button("erase ALL", ImVec2(-FLT_MIN, 0)))
            {
                std::lock_guard<std::mutex> lock(plane_mutex);
                for (Plane *plane : planeList)
                {
                    delete plane;
                }
                planeList.clear();
                show_clean_menu = false;
            }
            ImGui::PopStyleColor();

            if (ImGui::Button("return", ImVec2(-FLT_MIN, 0)))
            {
                show_clean_menu = false;
            }
            ImGui::End();
            F_style();
        }

        if (show_settings_menu)
        {
            ImGui::SetNextWindowPos(ImVec2(width * 0.5f, height * 0.65f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(radius * (7.4 * 0.125f), radius * (5.5 * 0.125f)));
            window_style();
            ImGui::Begin("Radar Settings", &show_settings_menu, ImGuiWindowFlags_NoResize);
            {

                static int s_range = (int)range;
                static int s_minDeg = minDeg;
                static int s_maxDeg = maxDeg;
                static int s_dps = degPerSec;
                static int s_gain = gain;

                ImGui::Text("Radar settings");
                ImGui::Separator();

                ImGui::SliderInt("Range (m)", &s_range, 1000, 100000);
                ImGui::SliderInt("Angle Mín", &s_minDeg, 0, 359);
                ImGui::SliderInt("Angle Máx", &s_maxDeg, 1, 360);
                ImGui::SliderInt("Velocity (deg/s)", &s_dps, 1, 360);
                ImGui::SliderInt("Gain (dBi)", &s_gain, 1, 60);

                ImGui::Spacing();

                if (ImGui::Button("Aply changes", ImVec2(-FLT_MIN, 0)))
                {

                    radar.setRange(s_range);
                    radar.setDPS(s_dps);
                    range = s_range;
                    degPerSec = s_dps;
                    gain = s_gain;

                    show_settings_menu = false;
                }

                if (ImGui::Button("Default settings", ImVec2(-FLT_MIN, 0)))
                {

                    s_range = 100000;
                    s_dps = 30;
                    s_gain = 34;

                    range = s_range;
                    degPerSec = s_dps;
                    gain = s_gain;
                }

                if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0)))
                {
                    show_settings_menu = false;
                }
            }
            ImGui::End();
            F_style();
        }

        ImGui::SetNextWindowPos(ImVec2(colllapsed_button_position));
        ImGui::SetNextWindowSize(ImVec2((width * 0.035f) / 3.0f, radius * 0.22f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(33.0f / 255, 33.0f / 255, 33.0f / 255, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));                             // normal color
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(33.0f / 255, 33.0f / 255, 33.0f / 255, 1.0f)); //  when they dected the mouse
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));                       // whent they do click

        if (ImGui::Begin("BotonControl", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground))
        {

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(81.0f / 255, 81.0f / 255, 81.0f / 255, 1.0f)); // white
            float total_width = ImGui::GetContentRegionAvail().x;
            float total_height = ImGui::GetContentRegionAvail().y;
            ImVec2 size_Button = ImVec2(total_width, total_height);
            ImVec2 iconSize = ImVec2(24 * main_scale, 24 * main_scale);
            float padX = (size_Button.x - iconSize.x) / 2.0f;
            float padY = (size_Button.y - iconSize.y) / 2.0f;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padX, padY));

            ImTextureID Actual = no_collapsed ? iconMin : iconMax;
            if (ImGui::ImageButton("colap_b", Actual, iconSize))
            {
                no_collapsed = !no_collapsed;
            }

            ImGui::PopStyleVar(4);
            ImGui::PopStyleColor();
        }

        ImGui::End();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(4);

        // Render
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h); // get the buffersize
        glViewport(0, 0, display_w, display_h);                 // define the renderize area

        // define the colours in the window.
        glClearColor(25.0f / 255, 25.0f / 255, 25.0f / 255, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    running = false;
    radarThread.join();

    for (Plane *plane : planeList)
    {
        delete plane; // clean the memory of any object and erase them
    }
    planeList.clear();

    // 5. cleaning
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
