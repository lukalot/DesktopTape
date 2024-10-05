#include "glad/glad.h"  // Move this to the top, before any other OpenGL-related includes

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include "window_utils.h"  // Make sure this line is present

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "menu_bar_controller.h"  // Add this line

// Add this near the top of the file, after the includes
extern "C" {
    void launchNewInstance();
}

#include <filesystem>
#include <mach-o/dyld.h>

const size_t MAX_INPUT_LENGTH = 4096;
char inputBuffer[MAX_INPUT_LENGTH] = "New tape...";
float fontSize = 14.0f;
const float MIN_FONT_SIZE = 10.0f;
const float MAX_FONT_SIZE = 60.0f;
bool fontSizeChanged = false;
ImFont* currentFont = nullptr;

int currentWidth = 100;
int currentHeight = 100;

GLuint VBO, VAO;

// Add these global variables at the top of the file
int targetWidth = 100;
int targetHeight = 100;
bool needsResize = false;

double lastScaleTime = 0.0;
const double scaleInterval = 0.1; // Scale every 100ms when key is held down

// Add this global variable to track window focus
static bool g_WindowFocused = true;

// Add this function to handle window focus callback
void windowFocusCallback(GLFWwindow* window, int focused)
{
    g_WindowFocused = focused != 0;
}

// Add this near the top of the file with other global variables
bool isFirstFrame = true;

void createRectangleVBO() {
    float vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    uniform mat4 transform;
    void main() {
        gl_Position = transform * vec4(aPos.x, aPos.y, 0.0, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 color;
    void main() {
        FragColor = color;
    }
)";

GLuint shaderProgram;

void compileShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void updateImGuiFont(ImGuiIO& io) {
    io.Fonts->Clear();
    ImFontConfig font_config;
    font_config.OversampleH = 4;
    font_config.OversampleV = 4;
    font_config.PixelSnapH = true;
    font_config.SizePixels = fontSize;
    font_config.RasterizerMultiply = 1.5f;
    font_config.GlyphOffset.y = 0.5f;
    font_config.GlyphExtraSpacing.x = 0.5f;
    currentFont = io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/SFNS.ttf", fontSize * 2.0f, &font_config);
    io.Fonts->Build();
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_CreateFontsTexture();
}

// Modify the updateWindowSize function
void updateWindowSize(GLFWwindow* window, const char* text) {
    ImGui::PushFont(currentFont);
    ImVec2 text_size = ImGui::CalcTextSize(text);
    float line_height = ImGui::GetTextLineHeight();
    ImGui::PopFont();

    // Count the number of newlines
    int newlines = 1; // Start with 1 to account for the first line
    for (const char* c = text; *c; ++c) {
        if (*c == '\n') newlines++;
    }

    // Calculate height based on newlines and actual text size
    float text_height = std::max(text_size.y, newlines * line_height);
    
    targetWidth = static_cast<int>(text_size.x) + 40;
    targetHeight = static_cast<int>(text_height) + 26;
    
    if (targetWidth != currentWidth || targetHeight != currentHeight) {
        glfwSetWindowSize(window, targetWidth, targetHeight);
        currentWidth = targetWidth;
        currentHeight = targetHeight;
    }
}

void errorCallback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void drawRectangleLines(float x, float y, float width, float height, float r, float g, float b, float a) {
    glUseProgram(shaderProgram);

    glm::mat4 transform = glm::ortho(0.0f, (float)currentWidth, (float)currentHeight, 0.0f, -1.0f, 1.0f);
    transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
    transform = glm::scale(transform, glm::vec3(width, height, 1.0f));

    GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    GLint colorLoc = glGetUniformLocation(shaderProgram, "color");
    glUniform4f(colorLoc, r, g, b, a);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

// Add this function near the top of your file, after the includes
void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // Suppress the Escape key by not forwarding it to ImGui
        return;
    }
    
    // Check for Command+N key combination
    if (key == GLFW_KEY_N && action == GLFW_PRESS && (mods & GLFW_MOD_SUPER)) {
        launchNewInstance();
        return;
    }
    
    // Forward all other key events to ImGui
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

// Add this function definition somewhere in the file, outside of main()
extern "C" void launchNewInstance();

int main(int, char**)
{
    // Set the working directory to the Resources folder of the .app bundle
    uint32_t bufsize = 1024;
    char path[1024];
    if (_NSGetExecutablePath(path, &bufsize) == 0) {
        std::filesystem::path executablePath(path);
        std::filesystem::path resourcesPath = executablePath.parent_path().parent_path() / "Resources";
        std::filesystem::current_path(resourcesPath);
    } else {
        std::cerr << "Failed to get executable path" << std::endl;
        return 1;
    }

    initializeCocoaApp();

    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Modify the window hints before creating the window
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // Make the window initially invisible

    GLFWwindow* window = glfwCreateWindow(400, 100, "Desktop Tape", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Add this line
    setWindowLevelForAllSpaces(glfwGetCocoaWindow(window));

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    createRectangleVBO();
    compileShaders();

    setWindowLevelBelowNormal(glfwGetCocoaWindow(window));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5f, 0.5f, 0.5f, 0.3f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.6f, 0.6f, 0.6f, 0.3f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.7f, 0.7f, 0.7f, 0.3f);

    updateImGuiFont(io);

    double dragOffsetX = 0.0, dragOffsetY = 0.0;
    bool isDragging = false;

    // After creating the window and before the main loop, add:
    glfwSetKeyCallback(window, glfwKeyCallback);

    // Set up window focus callback
    glfwSetWindowFocusCallback(window, windowFocusCallback);

    setupMenuBar();

    while (!glfwWindowShouldClose(window)) {
        processCocoaEvents();
        glfwPollEvents();

        double currentTime = glfwGetTime();

        // Check for continuous scaling
        if (io.KeyCtrl) { // This checks for Command on Mac or Ctrl on Windows/Linux
            if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) { // '+' key
                if (currentTime - lastScaleTime >= scaleInterval) {
                    fontSize = std::min(fontSize + 2.0f, MAX_FONT_SIZE);
                    fontSizeChanged = true;
                    lastScaleTime = currentTime;
                }
            } else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) { // '-' key
                if (currentTime - lastScaleTime >= scaleInterval) {
                    fontSize = std::max(fontSize - 2.0f, MIN_FONT_SIZE);
                    fontSizeChanged = true;
                    lastScaleTime = currentTime;
                }
            }
        }

        if (fontSizeChanged) {
            updateImGuiFont(io);
            fontSizeChanged = false;
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (!isDragging) {
                dragOffsetX = mouseX;
                dragOffsetY = mouseY;
                isDragging = true;
            } else {
                int windowX, windowY;
                glfwGetWindowPos(window, &windowX, &windowY);
                glfwSetWindowPos(window, 
                    windowX + (mouseX - dragOffsetX),
                    windowY + (mouseY - dragOffsetY));
            }
        } else {
            isDragging = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Update ImGui configuration based on window focus
        // ImGui::GetIO().ConfigInputTextCursorBlink = g_WindowFocused;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(currentWidth, currentHeight));
        ImGui::Begin("Text Input", nullptr, 
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoScrollWithMouse); // Add this flag

        ImGui::PushFont(currentFont);
        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_NoHorizontalScroll;
        if (ImGui::InputTextMultiline("##input", inputBuffer, MAX_INPUT_LENGTH,
            ImVec2(currentWidth - 10, currentHeight - 10), flags, NULL, NULL, !g_WindowFocused)) {
            updateWindowSize(window, inputBuffer);
        }
        ImGui::PopFont();

        // Update window size on every frame
        updateWindowSize(window, inputBuffer);

        ImGui::End();

        // Render the frame
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Draw the rectangle outline
        if (glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
            drawRectangleLines(0, 0, currentWidth, currentHeight, 1.0f, 1.0f, 1.0f, 0.5f);
        }

        // After the first frame, make the window visible
        if (isFirstFrame) {
            glfwShowWindow(window);
            isFirstFrame = false;
        }

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    runCocoaApp();

    return 0;
}