#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>
#include <ctime>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float FLOOR_POSITION = -0.9f;
const float OBJECT_SPEED = 0.0f;
const float JUMP_FORCE = 0.6f;
const float GRAVITY = 0.01f;
const float PIPE_WIDTH = 40.0f;
const float PIPE_HEIGHT = 250.0f;
const float PIPE_GAP = 150.0f;  // Reduced gap size
const float PIPE_SPEED = 0.8f;

float birdX = -WINDOW_WIDTH / 4;
float birdY = 0.0f;
float birdVelocity = 0.0f;

std::vector<float> pipePositions;
float lastPipeSpawnTime = 0.0f;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        birdVelocity = JUMP_FORCE;
    }
}

void updateBird()
{
    birdY += birdVelocity;
    birdVelocity -= GRAVITY;
}

void updatePipes()
{
    // Spawn new pipe pair periodically
    float currentTime = glfwGetTime();
    if (currentTime - lastPipeSpawnTime >= 2.5f)
    {
        float minPipeHeight = 1000.0f;
        float maxPipeHeight = 0.65f * WINDOW_HEIGHT;
        float pipeHeight = minPipeHeight + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxPipeHeight - minPipeHeight)));

        float pipeY = WINDOW_HEIGHT / 2 - (PIPE_GAP / 2.0f) - pipeHeight / 2.0f;  // Spawn position of bottom pipe adjusted
        pipePositions.push_back(WINDOW_WIDTH / 2);
        pipePositions.push_back(pipeY);
        lastPipeSpawnTime = currentTime;
    }

    // Update pipe positions
    for (size_t i = 0; i < pipePositions.size(); i += 2)
    {
        pipePositions[i] -= PIPE_SPEED;
    }

    // Remove pipes that have moved off the screen
    if (!pipePositions.empty() && pipePositions.front() + PIPE_WIDTH < -WINDOW_WIDTH / 2)
    {
        pipePositions.erase(pipePositions.begin(), pipePositions.begin() + 2);
    }
}

void update()
{
    updateBird();
    updatePipes();
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw the background
    glColor3f(0.2f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-WINDOW_WIDTH / 2, FLOOR_POSITION * WINDOW_HEIGHT / 2);
    glVertex2f(WINDOW_WIDTH / 2, FLOOR_POSITION * WINDOW_HEIGHT / 2);
    glVertex2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    glVertex2f(-WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    glEnd();

    // Draw the floor
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2);
    glVertex2f(WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2);
    glVertex2f(WINDOW_WIDTH / 2, FLOOR_POSITION * WINDOW_HEIGHT / 2);
    glVertex2f(-WINDOW_WIDTH / 2, FLOOR_POSITION * WINDOW_HEIGHT / 2);
    glEnd();

    // Draw the bird
    glPushMatrix();
    glTranslatef(birdX, birdY, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-20.0f, -20.0f);
    glVertex2f(20.0f, -20.0f);
    glVertex2f(20.0f, 20.0f);
    glVertex2f(-20.0f, 20.0f);
    glEnd();
    glPopMatrix();

    // Draw pipes
    glColor3f(0.5f, 0.5f, 0.5f);
    for (size_t i = 0; i < pipePositions.size(); i += 2)
    {
        float pipeX = pipePositions[i];
        float pipeY = pipePositions[i + 1];

        // Draw bottom half of the pipe
        glBegin(GL_QUADS);
        glVertex2f(pipeX, -WINDOW_HEIGHT / 2);
        glVertex2f(pipeX + PIPE_WIDTH, -WINDOW_HEIGHT / 2);
        glVertex2f(pipeX + PIPE_WIDTH, pipeY);
        glVertex2f(pipeX, pipeY);
        glEnd();

        // Draw top half of the pipe
        glBegin(GL_QUADS);
        glVertex2f(pipeX, pipeY + PIPE_GAP);
        glVertex2f(pipeX + PIPE_WIDTH, pipeY + PIPE_GAP);
        glVertex2f(pipeX + PIPE_WIDTH, WINDOW_HEIGHT / 2);
        glVertex2f(pipeX, WINDOW_HEIGHT / 2);
        glEnd();
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    if (!glfwInit())
    {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Flappy Bird", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);

    // Initialize the bird's starting position
    birdY = 0.0f;

    // Start the game loop
    while (!glfwWindowShouldClose(window))
    {
        update();
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
