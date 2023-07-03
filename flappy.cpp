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


void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // On check si le jeu recommence
    if (gameOver && key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
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

void updateBird() {
    // Si le jeu est fini l'oiseau bouge plus
    if (gameOver) {
        return;
    }
    birdY += birdVelocity;
    birdVelocity -= GRAVITY;

    // Colision
    for (size_t i = 0; i < pipePositions.size(); i += 2) {
        float pipeX = pipePositions[i];
        float pipeY = pipePositions[i + 1];

        if (birdX + 20.0f >= pipeX && birdX - 20.0f <= pipeX + PIPE_WIDTH) {
            if (birdY + 20.0f > pipeY + PIPE_GAP || birdY - 20.0f < pipeY) {
                gameOver = true; // On trouve une collision
                break;
            }
        }
    }

    // Collision sol et plafond
    if (birdY + 20.0f >= WINDOW_HEIGHT / 2 || birdY - 20.0f <= -WINDOW_HEIGHT / 2) {
        gameOver = true;
    }
}

void updatePipes() {
    // Jeu fini pas de update de pipes
    if (gameOver) {
        return;
    }
    // On rajoute des tuyaux
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

    // Position des pipe change
    for (size_t i = 0; i < pipePositions.size(); i += 2) {
        pipePositions[i] -= PIPE_SPEED;
    }

    // Pipe a gauche de l'ecran qui sont enlever
    if (!pipePositions.empty() && pipePositions.front() + PIPE_WIDTH < -WINDOW_WIDTH / 2) {
        pipePositions.erase(pipePositions.begin(), pipePositions.begin() + 2);
        nextPipeToCross -= 2;
    }
}

// Render pour pouvoir ecrire le score
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

    // Background
    glColor3f(0.2f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-WINDOW_WIDTH / 2, FLOOR_POSITION * WINDOW_HEIGHT / 2);
    glVertex2f(WINDOW_WIDTH / 2, FLOOR_POSITION * WINDOW_HEIGHT / 2);
    glVertex2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    glVertex2f(-WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    glEnd();

    // Sol
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
        // Activation de texture
        glBindTexture(GL_TEXTURE_2D, pipeDownTextureID);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        // Pipes du bas
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(pipeX, -WINDOW_HEIGHT / 2);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(pipeX + PIPE_WIDTH, -WINDOW_HEIGHT / 2);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(pipeX + PIPE_WIDTH, pipeY);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(pipeX, pipeY);
        glEnd();

        // Activation de texture
        glBindTexture(GL_TEXTURE_2D, pipeUpTextureID);

        // Pipes du haut
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(pipeX, pipeY + PIPE_GAP);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(pipeX + PIPE_WIDTH, pipeY + PIPE_GAP);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(pipeX + PIPE_WIDTH, WINDOW_HEIGHT / 2);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(pipeX, WINDOW_HEIGHT / 2);
        glEnd();

        // enlever les texture
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }

    // Score
    char scoreString[15];
    sprintf(scoreString, "Score: %d", score);
    glColor3f(1.0f, 1.0f, 1.0f);  // Set text color
    renderBitmapString(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 50, GLUT_BITMAP_HELVETICA_18, scoreString);

    // High score
    char highScoreString[25];
    sprintf(highScoreString, "High Score: %d", highScore);
    renderBitmapString(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 70, GLUT_BITMAP_HELVETICA_18, highScoreString);

    // Screen de fin de jeu
    if (gameOver) {
        glColor3f(1.0f, 1.0f, 1.0f);
        renderBitmapString(-50.0f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Game Over");

        char finalScoreString[25];
        sprintf(finalScoreString, "Final Score: %d", score);
        renderBitmapString(-80.0f, -25.0f, GLUT_BITMAP_HELVETICA_18, finalScoreString);

        renderBitmapString(-80.0f, -50.0f, GLUT_BITMAP_HELVETICA_18, "Press Space to Replay");
    }
    glFlush();
}

// La fonction update qui est lancer chaque boucle pour update les objets.
void update() {
    updateBird();
    updatePipes();

    if (nextPipeToCross < pipePositions.size() && birdX > pipePositions[nextPipeToCross]) {
        score++;
        nextPipeToCross += 2;
    }

    if (score > highScore) {
        highScore = score;
    }
}
