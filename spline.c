#include <GL/glut.h>
#include <stdio.h>

GLfloat cameraPoints[5][3] = {
    {0.0, 2.0, 10.0},   // P0
    {5.0, 3.0, 5.0},    // P1
    {0.0, 2.0, 0.0},    // P2
    {-5.0, 3.0, 5.0},   // P3
    {0.0, 2.0, 10.0}    // P4 (loop para voltar)
};

float t = 0.0f;     // parâmetro da spline
int segment = 1;    // segmento atual
float dt = 0.005f;  // velocidade da animação

// Função Catmull-Rom
void getCatmullRomPoint(float t, int i, GLfloat* pos) {
    float t2 = t * t;
    float t3 = t2 * t;

    pos[0] = 0.5f * ((2.0f * cameraPoints[i][0]) +
            (-cameraPoints[i-1][0] + cameraPoints[i+1][0]) * t +
            (2*cameraPoints[i-1][0] - 5*cameraPoints[i][0] + 4*cameraPoints[i+1][0] - cameraPoints[i+2][0]) * t2 +
            (-cameraPoints[i-1][0] + 3*cameraPoints[i][0] - 3*cameraPoints[i+1][0] + cameraPoints[i+2][0]) * t3);

    pos[1] = 0.5f * ((2.0f * cameraPoints[i][1]) +
            (-cameraPoints[i-1][1] + cameraPoints[i+1][1]) * t +
            (2*cameraPoints[i-1][1] - 5*cameraPoints[i][1] + 4*cameraPoints[i+1][1] - cameraPoints[i+2][1]) * t2 +
            (-cameraPoints[i-1][1] + 3*cameraPoints[i][1] - 3*cameraPoints[i+1][1] + cameraPoints[i+2][1]) * t3);

    pos[2] = 0.5f * ((2.0f * cameraPoints[i][2]) +
            (-cameraPoints[i-1][2] + cameraPoints[i+1][2]) * t +
            (2*cameraPoints[i-1][2] - 5*cameraPoints[i][2] + 4*cameraPoints[i+1][2] - cameraPoints[i+2][2]) * t2 +
            (-cameraPoints[i-1][2] + 3*cameraPoints[i][2] - 3*cameraPoints[i+1][2] + cameraPoints[i+2][2]) * t3);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    GLfloat camPos[3];
    getCatmullRomPoint(t, segment, camPos);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camPos[0], camPos[1], camPos[2],  // posição da câmera
              0.0, 0.0, 0.0,                   // ponto de interesse (olhar para o centro)
              0.0, 1.0, 0.0);                  // vetor "up"

    // Desenha um cubo no centro
    glColor3f(0.2f, 0.7f, 0.3f);
    glutSolidCube(2.0);

    glutSwapBuffers();

    // Atualiza t
    t += dt;
    if (t > 1.0f) {
        t = 0.0f;
        segment++;
        if (segment >= 3) segment = 1; // loop contínuo
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / (float)h, 1.0, 100.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Spline Camera Animation");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
