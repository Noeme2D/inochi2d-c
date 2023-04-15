#define INOCHI2D_GLYES
#include "inochi2d.h"

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <stdio.h>

double timingFunc() {
    return glfwGetTime();
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

int main() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) 
        return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(480, 800, "Inochi2D App", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // A timing function that returns the current applications runtime in seconds and milliseconds is needed
    inInit(&timingFunc);

    // viewport size, which is the size of the scene
    uint sceneWidth; 
    uint sceneHeight;

    // It is highly recommended to change the viewport with
    // inSetViewport to match the viewport you want, otherwise it'll be 640x480
    inViewportSet(480, 800);
    inViewportGet(&sceneWidth, &sceneHeight);

    // Also many vtuber textures are pretty big so let's zoom out a bit.
    InCamera* camera = inCameraGetCurrent();
    inCameraSetZoom(camera, 0.2);
    inCameraSetPosition(camera, 0, 1000);

    // NOTE: If you want to implement camera switching (for eg camera presets) use
    // inSetCamera

    // NOTE: Loading API WIP, subject to change
    // You can get example models at https://github.com/Inochi2D/example-models
    InPuppet* myPuppet = inPuppetLoad("Aka.inx");

    struct {
        size_t len;
        InParameter **cont;
    } parameters;
    // let D allocate memory
    parameters.cont = NULL;
    inPuppetGetParameters(myPuppet, &parameters.cont, &parameters.len);
    for (size_t i = 0; i < parameters.len; i++) {
        char *name = inParameterGetName(parameters.cont[i]);
        bool isVec2 = inParameterIsVec2(parameters.cont[i]);
        printf("Parameter #%zu: %s is%s vec2.\n", i, name, isVec2 ? "" : " not");
    }

    inParameterSetValue(parameters.cont[1], -1, 0);

    while(!glfwWindowShouldClose(window)) {
        // NOTE: Inochi2D does not itself clear the main framebuffer
        // you have to do that your self.
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Run inUpdate first
        // This updates various submodules and time managment for animation
        inUpdate();

        // Imagine there's a lot of rendering code here
        // Maybe even some game logic or something

        // Begins drawing in to the Inochi2D scene
        // NOTE: You *need* to do this otherwise rendering may break
        inSceneBegin();
        
            // Draw and update myPuppet.
            // Convention for using Inochi2D in D is to put everything
            // in a scene block one indent in.
            inPuppetUpdate(myPuppet);
            inPuppetDraw(myPuppet);

        // Ends drawing in to the Inochi2D scene.
        inSceneEnd();
        
        // Draw the scene, background is transparent
        inSceneDraw(0, 0, sceneWidth, sceneHeight);
        
        // Do the buffer swapping and event polling last
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    inCleanup();
    glfwTerminate();
    return 0;
}