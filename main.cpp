#include "flappy.hh"

int main(int argc, char *argv[]) {

    glutInit(&argc, argv);

    srand(static_cast<unsigned int>(time(nullptr)));

    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Flappy Bird", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        glfwTerminate();
        return -1;
    }

    GLuint textureID;

    textureID = SOIL_load_OGL_texture("images/jofa.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

    GLuint pipeUpTextureID = SOIL_load_OGL_texture("images/pipe_up.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint pipeDownTextureID = SOIL_load_OGL_texture("images/pipe_down.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);



// Vérifier si le chargement de la texture a réussi
    if (textureID == 0) {
        std::cerr << "SOIL loading error: '" << SOIL_last_result() << "'" << std::endl;
        return 1;
    }



    // Initialize the bird's starting position
//    birdY = 0.0f;

    // Start the game loop
    while (!glfwWindowShouldClose(window)) {
        update();
        render(textureID, pipeUpTextureID, pipeDownTextureID);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
