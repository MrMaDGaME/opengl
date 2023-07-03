#include "flappy.hh"

int score = 0;
int nextPipeToCross = 0;
float birdX = -WINDOW_WIDTH / 4;
float birdY = 0.0f;
float birdVelocity = 0.0f;
std::vector<float> pipePositions;
float lastPipeSpawnTime = 0.0f;
bool gameOver = false;
int highScore = 0;

std::string loadShaderSource(const char* filepath) {
    std::ifstream shaderFile;
    shaderFile.open(filepath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
}


void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (gameOver && key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        // Restart the game
        birdY = 0.0f;
        birdVelocity = 0.0f;
        pipePositions.clear();
        lastPipeSpawnTime = 0.0f;
        score = 0;
        nextPipeToCross = 0;
        gameOver = false;
    } else if (!gameOver && key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        birdVelocity = JUMP_FORCE;
    }
}

bool checkCollision(float birdX, float birdY, float pipeX, float pipeY) {
    // Define bird's radius
    const float BIRD_RADIUS = 20.0f;

    // Define rectangle edges
    float leftEdge = pipeX;
    float rightEdge = pipeX + PIPE_WIDTH;
    float topEdge = pipeY;
    float bottomEdge = pipeY + PIPE_HEIGHT;

    // Check if the bird is inside the pipe
    if (birdX + BIRD_RADIUS > leftEdge && birdX - BIRD_RADIUS < rightEdge &&
        birdY + BIRD_RADIUS > bottomEdge && birdY - BIRD_RADIUS < topEdge) {
        return true;
    }

    // Check if bird is intersecting with pipe's sides
    float closestX = std::max(leftEdge, std::min(birdX, rightEdge));
    float closestY = std::max(bottomEdge, std::min(birdY, topEdge));

    float dx = birdX - closestX;
    float dy = birdY - closestY;

    return (dx * dx + dy * dy) < (BIRD_RADIUS * BIRD_RADIUS);
}


void updateBird() {
    if (gameOver) {
        return; // Do not update bird if game is over
    }
    birdY += birdVelocity;
    birdVelocity -= GRAVITY;

    // Check collision with pipes
    for (size_t i = 0; i < pipePositions.size(); i += 2) {
        float pipeX = pipePositions[i];
        float pipeY = pipePositions[i + 1];

        // Check if bird's position is within the pipe's boundaries
        if (birdX + 20.0f >= pipeX && birdX - 20.0f <= pipeX + PIPE_WIDTH) {
            // Check if the bird is outside the gap
            if (birdY + 20.0f > pipeY + PIPE_GAP || birdY - 20.0f < pipeY) {
                gameOver = true; // Collision occurred
                break;
            }
        }
    }

    // Check collision with floor and ceiling
    if (birdY + 20.0f >= WINDOW_HEIGHT / 2 || birdY - 20.0f <= -WINDOW_HEIGHT / 2) {
        gameOver = true;
    }
}

void updatePipes() {
    if (gameOver) {
        return; // Do not update pipes if game is over
    }
    // Spawn new pipe pair periodically
    float currentTime = glfwGetTime();
    if (currentTime - lastPipeSpawnTime >= 1.0f && !gameOver) {
        float minPipeHeight = 1000.0f;
        float maxPipeHeight = 0.65f * WINDOW_HEIGHT;
        float pipeHeight = minPipeHeight + static_cast<float>(rand()) /
                                           (static_cast<float>(RAND_MAX / (maxPipeHeight - minPipeHeight)));
        float pipeY = WINDOW_HEIGHT / 2 - (PIPE_GAP / 2.0f) - pipeHeight / 2.0f;
        pipePositions.push_back(WINDOW_WIDTH / 2);
        pipePositions.push_back(pipeY);
        lastPipeSpawnTime = currentTime;
    }

    // Update pipe positions
    for (size_t i = 0; i < pipePositions.size(); i += 2) {
        pipePositions[i] -= PIPE_SPEED;
    }

    // Remove pipes that have moved off the screen
    if (!pipePositions.empty() && pipePositions.front() + PIPE_WIDTH < -WINDOW_WIDTH / 2) {
        pipePositions.erase(pipePositions.begin(), pipePositions.begin() + 2);
        nextPipeToCross -= 2;  // Adjust nextPipeToCross index
    }
}

void renderBitmapString(float x, float y, void *font, const char *string) {
    const char *c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void render(GLuint textureID, GLuint pipeUpTextureID, GLuint pipeDownTextureID) {
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

    glPushMatrix();
    glTranslatef(birdX, birdY, 0.0f);

// Activer la texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Activer le mélange alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-25.0f, -25.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(25.0f, -25.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(25.0f, 25.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-25.0f, 25.0f);
    glEnd();

// Désactiver la texture après utilisation
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    for (size_t i = 0; i < pipePositions.size(); i += 2) {
        float pipeX = pipePositions[i];
        float pipeY = pipePositions[i + 1];

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, pipeDownTextureID);  // Use the bottom pipe texture
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw bottom half of the pipe
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(pipeX, -WINDOW_HEIGHT / 2);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(pipeX + PIPE_WIDTH, -WINDOW_HEIGHT / 2);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(pipeX + PIPE_WIDTH, pipeY);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(pipeX, pipeY);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, pipeUpTextureID);  // Use the top pipe texture

        // Draw top half of the pipe
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(pipeX, pipeY + PIPE_GAP);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(pipeX + PIPE_WIDTH, pipeY + PIPE_GAP);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(pipeX + PIPE_WIDTH, WINDOW_HEIGHT / 2);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(pipeX, WINDOW_HEIGHT / 2);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }

    // Draw the score
    char scoreString[15];
    sprintf(scoreString, "Score: %d", score);
    glColor3f(1.0f, 1.0f, 1.0f);  // Set text color
    renderBitmapString(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 50, GLUT_BITMAP_HELVETICA_18, scoreString);

    // Draw the high score
    char highScoreString[25];
    sprintf(highScoreString, "High Score: %d", highScore);
    renderBitmapString(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 70, GLUT_BITMAP_HELVETICA_18, highScoreString);

    if (gameOver) {
        glColor3f(1.0f, 1.0f, 1.0f);  // Set text color
        renderBitmapString(-50.0f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Game Over");

        char finalScoreString[25];
        sprintf(finalScoreString, "Final Score: %d", score);
        renderBitmapString(-80.0f, -25.0f, GLUT_BITMAP_HELVETICA_18, finalScoreString);  // Display final score

        renderBitmapString(-80.0f, -50.0f, GLUT_BITMAP_HELVETICA_18, "Press Space to Replay");
    }
    glFlush();
}

void update() {
    updateBird();
    updatePipes();

    // Check if the bird has crossed the next pipe
    if (nextPipeToCross < pipePositions.size() && birdX > pipePositions[nextPipeToCross]) {
        score++;
        nextPipeToCross += 2;
    }

    if (score > highScore) {
        highScore = score;
    }
}
