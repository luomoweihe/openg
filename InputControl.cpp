//
// Created by Alex L on 2022/11/30.
//
#include <GL/glut.h>
#include <iostream>
#include <iomanip>
#include "InputControl.h"
#include <cmath>
#define M_PI 3.14159265358979323846 // pi

using namespace std;

extern GLint winWidth, winHeight;
extern GLfloat objectXform[4][4];//对象变换矩阵
extern bool drawLines;
extern float TransformTime;

extern GLfloat xview, yview, zview,     // Viewing-coordinate origin.
               xref,  yref,  zref,      // Look-at point.
               Vx,    Vy,    Vz;        // View-up vector.

//摄像机局部坐标系的向右向量，其值由视线向量 ( xref – xview,
// yref -yview, zref – zview ) 和向上向量 ( Vx, Vy, Vz ) 的向量积求出。
GLfloat VRightx = (yref - yview) * Vz - (zref - zview) * Vy;
GLfloat VRighty = (zref - zview) * Vx - (xref - xview) * Vz;
GLfloat VRightz = (xref - xview) * Vy - (yref - yview) * Vx;

bool rotateCamera = false;

float angle, axis[3];
int lastX, lastY;
TransMode transModeSelected = Rotate;
int mainMenuEntries = sizeof(mainMenu) / sizeof(menuEntryStruct);

void startMotion(int x, int y);
void stopMotion(int x, int y);
void userEventAction(int key);
void quatRotate(GLfloat theta,
                GLfloat ux,  GLfloat uy,  GLfloat uz,
                GLfloat& px, GLfloat& py, GLfloat& pz);
void ResetObjectXform();

void mouseButton(int button, int state, int x, int y)
{
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            rotateCamera = false;
            break;
        case GLUT_RIGHT_BUTTON:
            rotateCamera = true;
            break;
        default:
            return;
    }
    switch(state)
    {
        case GLUT_DOWN:
            startMotion(x, y);
            break;
        case GLUT_UP:
            stopMotion(x, y);
            break;
    }
}

void mouseMotion(int x, int y)
{
    float dx = x - lastX;
    float dy = y - lastY;
    if(!dx && !dy) return;

    if(rotateCamera)
    {
        // 以参考点为中心，绕局部 X 轴旋转俯仰角，绕平行世界 Y 轴旋转环视；
        // 左乘局部向右向量(cross(v,vup))旋转，左乘世界 Y 轴向量(0,1,0)旋转
        VRightx = (yref - yview) * Vz - (zref - zview) * Vy;
        VRighty = (zref - zview) * Vx - (xref - xview) * Vz;
        VRightz = (xref - xview) * Vy - (yref - yview) * Vx;
        // 四元数旋转，旋转角由 dx 和 dy 确定
        GLfloat thetaX = -dy * (M_PI / winHeight);
        GLfloat thetaY = -dx * (M_PI / winWidth);
        GLfloat px = xref - xview;
        GLfloat py = yref - yview;
        GLfloat pz = zref - zview;
        quatRotate(thetaX, VRightx, VRighty, VRightz, px, py, pz);
        quatRotate(thetaY, 0.0, 1.0, 0.0, px, py, pz);

        xref = xview + px;
        yref = yview + py;
        zref = zview + pz;

        // 计算 vup 轴
        quatRotate(thetaX, VRightx, VRighty, VRightz, Vx, Vy, Vz);
        quatRotate(thetaY, 0.0, 1.0, 0.0, Vx, Vy, Vz);
    }
    else if(transModeSelected == Translate)
    {
        axis[0] = dx * (10.0f/winWidth);
        axis[1] = -dy * (10.0f/winHeight);
        axis[2] = 0;
    }
    else if (transModeSelected == Rotate)
    {
        angle = 3.0;
        axis[0] = dy * (360.0f/winHeight);
        axis[1] = dx * (360.0f/winWidth);
        axis[2] = 0;
    }
    else if (transModeSelected == Scale)
    {
        axis[0] = dx * (4.0f/winWidth);
        axis[1] = -dy * (4.0f/winHeight);
        axis[2] = 0; // 在调用时会 +1
    }

    lastX = x;
    lastY = y;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    userEventAction(key);
}

void startMotion(int x, int y)//鼠标按下动作储存初始坐标信息
{
    lastX = x;
    lastY = y;
}

void stopMotion(int x, int y)//鼠标弹起动作打印变换后的对象变换矩阵objectXform，数据会被打印到命令行窗口中
{
    angle = 0.0f;
    memset(axis, 0, sizeof(axis));
    cout<<"objectXform:"<<endl;
    for(int i = 0; i < 4; i++)
    {
        cout<<"\t";
        for(int j = 0; j < 4; j++)
        {
            cout<<setprecision(2)<<objectXform[i][j]<<"\t";
        }
        cout<<endl;
    }
}

void userEventAction(int key)
{
    switch (key)
    {
        case'0':        //重置
            angle = 0.0f;
            memset(axis, 0, sizeof(axis));
            ResetObjectXform();//重置变换矩阵
            //重置观察变换
            xview = 0.0; yview = 0.0; zview = 0.0;
            xref = 0.0; yref = 0.0; zref = -5.0;
            Vx = 0.0; Vy = 1.0; Vz = 0.0;
            transModeSelected = Rotate;
            break;
        case'r':        //旋转
            transModeSelected = Rotate;
            break;
        case't':        //平移
            transModeSelected = Translate;
            break;
        case's':        //缩放
            transModeSelected = Scale;
            break;
        case'1':        //线框图-多边形模式转换
            drawLines = !drawLines;
            break;
        case 'a':        //自动
            transModeSelected = Auto;
            ResetObjectXform();
            TransformTime = 0.0;
            break;
        case'2':        //正视图
            transModeSelected = ViewTransform;
            xview = 0.0; yview = 0.0; zview = 0.0;
            xref = 0.0; yref = 0.0; zref = -5.0;
            Vx = 0.0; Vy = 1.0; Vz = 0.0;
            break;
        case'4':        //左视图
            transModeSelected = ViewTransform;
            xview = -5.0; yview = 0.0; zview = -5.0;
            xref = 0.0; yref = 0.0; zref = -5.0;
            Vx = 0.0; Vy = 1.0; Vz = 0.0;
            break;
        case'6':        //右视图
            transModeSelected = ViewTransform;
            xview = 5.0; yview = 0.0; zview = -5.0;
            xref = 0.0; yref = 0.0; zref = -5.0;
            Vx = 0.0; Vy = 1.0; Vz = 0.0;
            break;
        case'8':        //后视图
            transModeSelected = ViewTransform;
            xview = 0.0; yview = 0.0; zview = -10.0;
            xref = 0.0; yref = 0.0; zref = -5.0;
            Vx = 0.0; Vy = 1.0; Vz = 0.0;
            break;
        case'5':        //顶视图
            transModeSelected = ViewTransform;
            xview = 0.0; yview = 5.0; zview = -5.0;
            xref = 0.0; yref = 0.0; zref = -5.0;
            Vx = 0.0; Vy = 0.0; Vz = -1.0;
            break;
        case 'y':
            xview += 0.15 * (xref - xview);
            xref += 0.15 * (xref - xview);
            yview += 0.15 * (yref - yview);
            yref += 0.15 * (yref - yview);
            zview += 0.15 * (zref - zview);
            zref += 0.15 * (zref - zview);
            break;
        case 'g':
            VRightx = (yref - yview) * Vz - (zref - zview) * Vy;
            VRighty = (zref - zview) * Vx - (xref - xview) * Vz;
            VRightz = (xref - xview) * Vy - (yref - yview) * Vx;
            xview -= 0.15 * VRightx;
            xref -= 0.15 * VRightx;
            yview -= 0.15 * VRighty;
            yref -= 0.15 * VRighty;
            zview -= 0.15 * VRightz;
            zref -= 0.15 * VRightz;
            break;
        case 'h':
            xview -= 0.15 * (xref - xview);
            xref -= 0.15 * (xref - xview);
            yview -= 0.15 * (yref - yview);
            yref -= 0.15 * (yref - yview);
            zview -= 0.15 * (zref - zview);
            zref -= 0.15 * (zref - zview);
            break;
        case 'j':
            VRightx = (yref - yview) * Vz - (zref - zview) * Vy;
            VRighty = (zref - zview) * Vx - (xref - xview) * Vz;
            VRightz = (xref - xview) * Vy - (yref - yview) * Vx;
            xview += 0.15 * VRightx;
            xref += 0.15 * VRightx;
            yview += 0.15 * VRighty;
            yref += 0.15 * VRighty;
            zview += 0.15 * VRightz;
            zref += 0.15 * VRightz;
            break;
        case 'q':
            yview += 0.1 * 1.0;
            yref += 0.1 * 1.0;
            break;
        case 'p':
            yview -= 0.1 * 1.0;
            yref -= 0.1 * 1.0;
            break;
        case 27:        //ESC键（ASCII：27）退出
            exit(0);
            break;
        default:
            break;
    }
    glutPostRedisplay();//重绘
}
void selectMain(int choice)      //调用通用动作解释函数
{
    userEventAction(mainMenu[choice].key);
}

void mouseWheel(int button, int dir, int x, int y)
{

        if(rotateCamera)
        {
            if(dir > 0) userEventAction('y');
            else if(dir < 0) userEventAction('h');
        }
        else if(transModeSelected == Scale)
        {
            axis[0] = dir * (4.0f/winWidth);
            axis[1] = -dir * (4.0f/winHeight);
            axis[2] = 0; // 在调用时会 +1
        }

    glutPostRedisplay();
}

/***************
四元数旋转函数 quatRotate 如下图所示。相关推导见教材 9.2.3 节。点坐标
( px, py, pz ) 三个参数使用引用传递，并在函数内部创建临时副本，根据旧坐标
计算得出旋转后的新坐标，并直接对引用赋值。
 **************/
void quatRotate(GLfloat theta,
                GLfloat ux,  GLfloat uy,  GLfloat uz,
                GLfloat& px, GLfloat& py, GLfloat& pz)
{
    GLfloat inpx = px;
    GLfloat inpy = py;
    GLfloat inpz = pz;
    px =
               ( ux * ux * (1 - cos(theta)) + cos(theta) ) * inpx +
               ( ux * uy * (1 - cos(theta)) - uz * sin(theta) ) * inpy +
               ( ux * uz * (1 - cos(theta)) + uy * sin(theta) ) * inpz ;
    py =
                ( uy * ux * (1 - cos(theta)) + uz * sin(theta) ) * inpx +
                ( uy * uy * (1 - cos(theta)) + cos(theta) ) * inpy +
                ( uy * uz * (1 - cos(theta)) - ux * sin(theta) ) * inpz ;
    pz =
                ( uz * ux * (1 - cos(theta)) - uy * sin(theta) ) * inpx +
                ( uz * uy * (1 - cos(theta)) + ux * sin(theta) ) * inpy +
                ( uz * uz * (1 - cos(theta)) + cos(theta) ) * inpz ;
}

/**
* 重置变换矩阵
*/
void ResetObjectXform()
{
    memset(objectXform, 0, sizeof(objectXform));//变换矩阵重置
    for(int i = 0; i <4; i++)
    {
        for(int j= 0; j < 4; j++)
        {
            if(i == j)objectXform[i][j] = 1.0f;
        }
    }
}