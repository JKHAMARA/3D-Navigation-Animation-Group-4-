#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>

// Camera Position
float camX = 0.0f, camY = 1.0f, camZ = 5.0f;
float angleY = 0.0f;
float speed = 0.1f;
float rotationAngle = 0.0f; // For keyframe animation

// Particle System
struct Particle {
    float x, y, z;
    float velocityX, velocityY, velocityZ;
    float life; // For fire particles
    float size; // Size of the particle
};

std::vector<Particle> rainParticles(2000); // Increased particle count for rain
std::vector<Particle> fireParticles(1000); // Increased particle count for fire
std::vector<Particle> snowParticles(1500); // Increased particle count for snow

void initParticles() {
    // Initialize rain particles
    for (auto &p : rainParticles) {
        p.x = (rand() % 200 - 100) / 10.0f;
        p.y = (rand() % 100) / 10.0f;
        p.z = (rand() % 200 - 100) / 10.0f;
        p.velocityY = -((rand() % 5 + 5) / 50.0f); // Faster falling
        p.velocityX = 0.0f;
        p.velocityZ = 0.0f;
        p.size = 2.0f; // Smaller size for rain
    }

    // Initialize fire particles
    for (auto &p : fireParticles) {
        p.x = (rand() % 20 - 10) / 10.0f;
        p.y = 0.0f;
        p.z = (rand() % 20 - 10) / 10.0f;
        p.velocityY = (rand() % 5 + 5) / 100.0f; // Slower upward movement
        p.velocityX = (rand() % 10 - 5) / 100.0f; // Slower horizontal movement
        p.velocityZ = (rand() % 10 - 5) / 100.0f; // Slower horizontal movement
        p.life = 1.0f;
        p.size = (rand() % 5 + 3); // Random size for fire particles
    }

    // Initialize snow particles
    for (auto &p : snowParticles) {
        p.x = (rand() % 200 - 100) / 10.0f;
        p.y = (rand() % 100) / 10.0f;
        p.z = (rand() % 200 - 100) / 10.0f;
        p.velocityY = -((rand() % 5) / 100.0f); // Slower falling
        p.velocityX = (rand() % 10 - 5) / 50.0f; // Slight horizontal movement
        p.velocityZ = (rand() % 10 - 5) / 50.0f; // Slight horizontal movement
        p.size = 3.0f; // Larger size for snow
    }
}

void updateRainParticles() {
    for (auto &p : rainParticles) {
        p.y += p.velocityY;
        if (p.y < -5) {
            p.y = 5;
            p.x = (rand() % 200 - 100) / 10.0f;
            p.z = (rand() % 200 - 100) / 10.0f;
        }
    }
}

void updateFireParticles() {
    for (auto &p : fireParticles) {
        p.x += p.velocityX;
        p.y += p.velocityY;
        p.z += p.velocityZ;
        p.life -= 0.01f; // Fade out over time

        // Reset particle if it fades out
        if (p.life <= 0.0f) {
            p.x = (rand() % 20 - 10) / 10.0f;
            p.y = 0.0f;
            p.z = (rand() % 20 - 10) / 10.0f;
            p.velocityY = (rand() % 5 + 5) / 100.0f; // Slower upward movement
            p.life = 1.0f;
            p.size = (rand() % 5 + 3); // Random size for fire particles
        }
    }
}

void updateSnowParticles() {
    for (auto &p : snowParticles) {
        p.y += p.velocityY;
        p.x += p.velocityX;
        p.z += p.velocityZ;

        if (p.y < -5) {
            p.y = 5;
            p.x = (rand() % 200 - 100) / 10.0f;
            p.z = (rand() % 200 - 100) / 10.0f;
        }
    }
}

// Keyboard controls for movement
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': camZ -= speed; break; // Move forward
        case 's': camZ += speed; break; // Move backward
        case 'a': camX -= speed; break; // Move left
        case 'd': camX += speed; break; // Move right
        case 27: exit(0); // Escape key exits
    }
    glutPostRedisplay();
}

// Mouse movement for rotation
void mouseMotion(int x, int y) {
    static bool warp = false;
    if (warp) { warp = false; return; }

    int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
    int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
    angleY += (x - centerX) * 0.005f;

    warp = true;
    glutWarpPointer(centerX, centerY);
    glutPostRedisplay();
}

// Display function (combines object rendering and particles)
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camX, camY, camZ, camX + sin(angleY), camY, camZ - cos(angleY), 0, 1, 0);

    // Render rotating cube (keyframe animation)
    glPushMatrix();
    glTranslatef(0, 0, -5);
    glRotatef(rotationAngle, 1, 1, 0);
    glColor3f(0, 1, 0);
    glutSolidCube(1);
    glPopMatrix();

    // Render rain particles
    glColor3f(0.5, 0.5, 1);
    glPointSize(2);
    glBegin(GL_POINTS);
    for (const auto &p : rainParticles)
        glVertex3f(p.x, p.y, p.z);
    glEnd();

    // Render fire particles
    glPointSize(4); // Larger size for fire
    glBegin(GL_POINTS);
    for (const auto &p : fireParticles) {
        glColor3f(1.0f, 0.5f, 0.0f); // Orange color
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();

    // Render snow particles
    glColor3f(1, 1, 1);
    glPointSize(3);
    glBegin(GL_POINTS);
    for (const auto &p : snowParticles)
        glVertex3f(p.x, p.y, p.z);
    glEnd();

    glutSwapBuffers();
}

// Timer function for animation
void timer(int value) {
    rotationAngle += 1.0f; // Rotate the cube
    if (rotationAngle > 360) rotationAngle -= 360;

    updateRainParticles();
    updateFireParticles();
    updateSnowParticles();

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    initParticles();

    // Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 800.0 / 600.0, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Navigation + Animation + Particles");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMotion);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
