#include <GL/glut.h>
#include <cmath>
#include "InputControl.h"
#include <Gl/freeglut_ext.h>

float mynear = 2.0;
float myfar = 7.0;
float viewxform_z = -5.0;
float pi = 3.14;
GLint winWidth = 1000;
GLint winHeight = 1000;

//观察变换所需的值s
GLfloat xview = 0.0, yview = 0.0, zview = 0.0; // Viewing-coordinate origin.
GLfloat xref = 0.0, yref = 0.0, zref = -1.0; // Look-at point.
GLfloat Vx = 0.0, Vy = 1.0, Vz = 0.0; // View-up vector.

extern float angle, axis[3];
extern int lastX, lastY;
extern TransMode transModeSelected;
extern int mainMenuEntries;

void init(void)
{
    glLineWidth(2.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_BLEND);//启用混合，使能反走样、阿尔法通道
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);//反走样
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    //glEnable(GL_LIGHT0);//启动一号灯
    //glEnable(GL_LIGHTING);//开启光照

    glutCreateMenu(selectMain); // 使用 selectMain 作为菜单调用函数
    for (int i = 0; i < mainMenuEntries; i++)
    {
        glutAddMenuEntry(mainMenu[i].label, i);
    }
    glutAttachMenu(GLUT_MIDDLE_BUTTON); // 菜单绑定右键

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //gluLookAt(2, 2, 2, 0, 0, 0, -1, -1, -1);
}

void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    winHeight = h;
    winWidth = w;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(w<=h)
        glFrustum(-2.5, 2.5, -2.5 * (float)h / (float)w, 2.5 * (float)h / (float)w, mynear, myfar);
    else
        glFrustum(-2.5 * (float)w / (float)h, 2.5 * (float)w / (float)h, -2.5, 2.5, mynear, myfar);
    /*if(w<=h)
        glOrtho(-5.0, 5.0, -5.0 * (float)h / (float)w, 5.0 * (float)h / (float)w, mynear, myfar);
    else
        glOrtho(-5.0 * (float)w / (float)h, 5.0 * (float)w / (float)h, -5.0, 5.0, mynear, myfar);
*/

    viewxform_z = -5.0;
    glMatrixMode(GL_MODELVIEW);
    /*glLoadIdentity();
    gluLookAt(2, 2, 2, 0, 0, 0, -1, -1, -1);*/

}

GLfloat vertices[8][3] = {
        {-1.0,-1.0,-1.0}, {1.0,-1.0,-1.0}, {1.0,1.0,-1.0}, {-1.0,1.0,-1.0},
        {-1.0,-1.0,1.0}, {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}
};

GLfloat colors[8][4] = {
        {0.0,0.0,0.0, 0.5}, {1.0,0.0,0.0, 0.5}, {1.0,1.0,0.0, 0.5}, {0.0,1.0,0.0, 0.5},
        {0.0,0.0,1.0, 0.5}, {1.0,0.0,1.0, 0.5}, {1.0,1.0,1.0, 0.5}, {0.0,1.0,1.0, 0.5}
};

GLfloat Top_Bottom[2][3] = {
        {0.0, 2.0, 0.0}, {0.0, -2.0, 0.0}
};

GLfloat TBColor[2][4] = {
        {123, 0, 123, 0.5}, {0, 123, 123, 0.5}
};

GLfloat objectXform[4][4] = {
        {1.0, 0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
};

bool drawLines = false;

void polygon(int a, int b, int c, int d)
{
    /* draw a polygon via list of vertices */
    if(drawLines){
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
            glVertex3fv(vertices[a]);
            glVertex3fv(vertices[b]);
            glVertex3fv(vertices[c]);
            glVertex3fv(vertices[d]);
        glEnd();
    }
    else{
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_POLYGON);//逆时针绘制面
            glColor4fv(colors[a]);
            glVertex3fv(vertices[a]);

            glColor4fv(colors[b]);
            glVertex3fv(vertices[b]);

            glColor4fv(colors[c]);
            glVertex3fv(vertices[c]);

            glColor4fv(colors[d]);
            glVertex3fv(vertices[d]);

        glEnd();
    }

}

void colorcube(void)
{
/* map vertices to faces */
    polygon(1, 0, 3, 2);
    polygon(3, 7, 6, 2);
    polygon(7, 3, 0, 4);
    polygon(2, 6, 5, 1);
    polygon(4, 5, 6, 7);
    polygon(5, 4, 0, 1);
}

void triangle(int a, int b, int c, int judge)
{
    if (judge == 0){
        glColor3f(0.0, 0.0, 0.0);

        glBegin(GL_TRIANGLES);

        glColor4fv(TBColor[a]);
        glVertex3fv(Top_Bottom[a]);

        glColor4fv(colors[b]);
        glVertex3fv(vertices[b]);

        glColor4fv(colors[c]);
        glVertex3fv(vertices[c]);
    }
    else if (judge == 1){
        glColor3ub(255.0, 255.0, 255.0);

        glBegin(GL_LINE_LOOP);

        glVertex3fv(Top_Bottom[a]);

        glVertex3fv(vertices[b]);

        glVertex3fv(vertices[c]);
    }

    glEnd();
}

void drawtriangle(void)
{
    //top
    triangle(0, 7, 6, 0);
    triangle(0, 6, 2, 0);
    triangle(0, 2, 3, 0);
    triangle(0, 3, 7, 0);
    //bottom
    triangle(1, 5, 4, 0);
    triangle(1, 1, 5, 0);
    triangle(1, 1, 0, 0);
    triangle(1, 0, 4, 0);

    //line
    //top
    triangle(0, 7, 6, 1);
    triangle(0, 6, 2, 1);
    triangle(0, 2, 3, 1);
    triangle(0, 3, 7, 1);
    //bottom
    triangle(1, 5, 4, 1);
    triangle(1, 1, 5, 1);
    triangle(1, 1, 0, 1);
    triangle(1, 0, 4, 1);
}
float TransformTime = 0.0;
float deltaTime = 0.01;

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-2.5, 2.5, -2.5, 2.5, mynear, myfar);
     */
    if(transModeSelected == ViewTransform){
        glPushMatrix();
        glTranslatef(0.0, -2.1, 0.0);
        glScalef(5.0, 0.1, 5.0);
        glColor4f(0.5, 0.5, 0.5, 0.5);
        glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, 0.5, -2);
        glScalef(5.0, 5, 0.1);
        glColor4f(0.5, 0.5, 0.5, 0.5);
        glutSolidCube(1.0);
        glPopMatrix();
    }


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(xview, yview, zview, xref, yref, zref, Vx, Vy, Vz);
    glTranslatef(0.0, 0.0, viewxform_z);

    glDepthMask(GL_FALSE);

    if(transModeSelected == Translate)
    {
        //平移
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(axis[0], axis[1], axis[2]);
        glMultMatrixf((GLfloat *)objectXform);
        glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);
        glPopMatrix();
        //重置axis
        axis[0] = 0.0;
        axis[1] = 0.0;
        axis[2] = 0.0;
    }
    else if(transModeSelected == Rotate)
    {
        //旋转
        glPushMatrix();//保存当前矩阵至栈中
        glLoadIdentity();
        glRotatef((GLfloat)angle, axis[0], axis[1], axis[2]);
        glMultMatrixf((GLfloat *)objectXform);//将当前矩阵右乘到栈顶
        glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);//取出GL_MODELVIEW_MATRIX存于objectXform
        glPopMatrix();
        //重置angle
        angle = 0.0f;
    }
    else if(transModeSelected == Scale)
    {
        //缩放
        glPushMatrix();//保存当前矩阵至栈中
        glLoadIdentity();
        glScalef(axis[0]+1,axis[1]+1,axis[2]+1);
        glMultMatrixf((GLfloat *)objectXform);//将当前矩阵右乘到栈顶
        glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);//取出GL_MODELVIEW_MATRIX存于objectXform
        glPopMatrix();
        //重置axis
        axis[0] = 0.0;
        axis[1] = 0.0;
        axis[2] = 0.0;
    }
    else if(transModeSelected == Auto)
    {
        /***********平移***********/
        glPushMatrix();//保存当前矩阵至栈中
        glLoadIdentity();
        GLfloat deltaHeight = (GLfloat) (sin(TransformTime + deltaTime ) - sin(TransformTime) ) * 2;
        glTranslatef(0.0, deltaHeight, 0.0);
        glMultMatrixf((GLfloat *)objectXform);//将当前矩阵右乘到栈顶
        glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);//取出GL_MODELVIEW_MATRIX存于objectXform
        glPopMatrix();//从栈顶取出矩阵

        /***********旋转***********/
        glPushMatrix();//保存当前矩阵至栈中
        glLoadIdentity();
        GLfloat deltaAngle = (GLfloat) (sin(TransformTime + deltaTime + pi / 2)) * pi;
        glRotatef(deltaAngle,0,1,0);
        glMultMatrixf((GLfloat *)objectXform);//将当前矩阵右乘到栈顶
        glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);//取出GL_MODELVIEW_MATRIX存于objectXform
        glPopMatrix();//从栈顶取出矩阵

        /***********缩放************/
        glPushMatrix();//保存当前矩阵至栈中
        glLoadIdentity();
        GLfloat deltaScale = (GLfloat) (sin(TransformTime + deltaTime) - sin(TransformTime) + 1);//1.005;//(1+abs(0.2*(sin(TransformTime + deltaTime))));
        glScaled(deltaScale,deltaScale,deltaScale);
        glMultMatrixf((GLfloat *)objectXform);//将当前矩阵右乘到栈顶
        glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)objectXform);//取出GL_MODELVIEW_MATRIX存于objectXform
        glPopMatrix();//从栈顶取出矩阵

        TransformTime += deltaTime;
    }

    glPushMatrix();
    glMultMatrixf((GLfloat *)objectXform);


    //绘制
    bool k = drawLines;
    if(!drawLines)
    {
        colorcube();
        drawLines = true;
        colorcube();
    }
    else colorcube();
    drawLines = k;
    drawtriangle();

    glPopMatrix();

    glDepthMask(GL_TRUE);


    glFlush();
    glutSwapBuffers();

    Sleep(deltaTime * 1000);

    glutPostRedisplay();
}
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    //glutInitWindowPosition(50, 100);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Example");

    init();
    /*******************************/
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
    glutMouseWheelFunc(mouseWheel);
    /*******************************/
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
