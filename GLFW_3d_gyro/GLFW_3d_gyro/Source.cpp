#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "OtherFunc.h"
#include "COM_controller.h"
#include <iostream>
#include <cstdlib>
#include "BLE_controller.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void DrawCube(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength);

GLfloat rotationX = 0.0f;
GLfloat rotationY = -90.0f;
GLfloat rotationZ = 0.0f;

bool dynamic_rotate = false;

int main(void) {
    std::vector<std::wstring> tmpvec;
    //COM_controller cc;
    //cc.get_available_com_potrs();
    //tmpvec = cc.get_available_com_potrs();
    //std::cout << "Available ports: \n----------------";
    //for (auto& elem : tmpvec) {
    //    std::wcout << "\n[" << elem << "]";
    //}
    //std::cout << "\n----------------";
    //std::cout << "\nSearch for the desired port...\n";
    std::string answer;
    int count = 10;
    LPCTSTR com = L"COM6";
    //cc.connect_to_esp32(com);
    std::wstring connected_port = com;
    // auto connect
    //connected_port = cc.auto_find_port_esp32();
    ///////////////
    //system("Gyro test");
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD curPos;
    Blinking(hStdOut, false);
    curPos = { 0, 0 };
    ///////////////
    /*if (!connected_port.empty()) {
        std::wcout << "Port found: [" << connected_port << "]" << std::endl << std::endl;
        cc.connect_to_esp32(connected_port.c_str());
        system("cls");
    }
    else {
        std::cout << "Failed search!";
    }*/

    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
    {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gyro test", NULL, NULL);

    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);


    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    glViewport(0.0f, 0.0f, screenWidth, screenHeight); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glMatrixMode(GL_PROJECTION); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glLoadIdentity(); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1000); // essentially set coordinate system
    glMatrixMode(GL_MODELVIEW); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    glLoadIdentity(); // same as above comment

    GLfloat halfScreenWidth = SCREEN_WIDTH / 2;
    GLfloat halfScreenHeight = SCREEN_HEIGHT / 2;


    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Render OpenGL here
        /////
        SetConsoleCursorPosition(hStdOut, curPos);
        //SetConsoleCursorPosition(hStdOut, curPos);
        int size = 0;
        std::string answer = "";
        try {
            answer = getBleString();
            //answer = wait_answer_from_esp32(cc);
            size = answer.size();
            std::cout << answer;
        }
        catch (std::exception e) {

        }
        SetConsoleCursorPosition(hStdOut, { (short)size, 0 });
        std::cout << std::string(50, ' ');
        bool is_X = false;
        bool is_Y = false;
        bool is_Z = false;
        int gyro_X = 0;
        int gyro_Y = 0;
        int gyro_Z = 0;
        std::string work_gyro_word_X = "";
        std::string work_gyro_word_Y = "";
        std::string work_gyro_word_Z = "";
        for (int i = 0; i < size; i++) {
            if (answer[i] == ' ') {
                continue;
            }
            if (answer[i] == 'X') {
                is_X = true;
                is_Y = false;
                is_Z = false;
                i++;
                continue;
            }
            else if (answer[i] == 'Y') {

                is_X = false;
                is_Y = true;
                is_Z = false;
                i++;
                continue;
            }
            else if (answer[i] == 'Z') {
                is_X = false;
                is_Y = false;
                is_Z = true;
                i++;
                continue;
            }
            if (is_X) {
                work_gyro_word_X += answer[i];
            }
            else if (is_Y) {
                work_gyro_word_Y += answer[i];
            }
            else if (is_Z) {
                work_gyro_word_Z += answer[i];
            }
        }
        try {
            gyro_X = std::stoi(work_gyro_word_X);
            gyro_Y = std::stoi(work_gyro_word_Y);
            gyro_Z = std::stoi(work_gyro_word_Z);
        }
        catch (std::exception e) {

        }
        std::cout << "\nGyro X: " << gyro_X << "                     \n";
        std::cout << "Gyro Y: " << gyro_Y << "                     \n";
        std::cout << "Gyro Z: " << gyro_Z << "                     \n";
        rotationX += ((gyro_X > 300 || gyro_X < -300) && dynamic_rotate ? gyro_X * 0.001 : 0);
        rotationY += ((gyro_Y > 300 || gyro_Y < -300) && dynamic_rotate ? gyro_Y * 0.001 : 0);
        rotationZ += ((gyro_Z > 300 || gyro_Z < -300) && dynamic_rotate ? gyro_Z * 0.001 : 0);

        //rotationX += (gyro_X > 300 || gyro_X < -300 ? gyro_X * 0.001 : 0);
        //rotationY += (gyro_Y > 300 || gyro_Y < -300 ? gyro_Y * 0.001 : 0);
        //rotationZ += (gyro_Z > 300 || gyro_Z < -300 ? gyro_Z * 0.001 : 0);
        /////
        glPushMatrix();
        glTranslatef(halfScreenWidth, halfScreenHeight, -500);
        glRotatef(rotationX, 1, 0, 0);
        glRotatef(rotationY, 0, 1, 0);
        glRotatef(rotationZ, 0, 0, 1);
        glTranslatef(-halfScreenWidth, -halfScreenHeight, 500);

        DrawCube(halfScreenWidth, halfScreenHeight, -500, 200);
        DrawCube(halfScreenWidth, halfScreenHeight, -500, 50);
        DrawCube(halfScreenWidth + 100, halfScreenHeight, -500, 40);
        DrawCube(halfScreenWidth - 100, halfScreenHeight, -500, 60);

        glPopMatrix();


        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}



void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //std::cout << key << std::endl;

    const GLfloat rotationSpeed = 10;

    // actions are GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_UP:
            rotationX -= rotationSpeed;
            break;
        case GLFW_KEY_DOWN:
            rotationX += rotationSpeed;
            break;
        case GLFW_KEY_RIGHT:
            rotationY += rotationSpeed;
            break;
        case GLFW_KEY_LEFT:
            rotationY -= rotationSpeed;
            break;
        case GLFW_KEY_F:
            dynamic_rotate = !dynamic_rotate;
            break;
        }
    }
}


void DrawCube(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength)
{
    GLfloat halfSideLength = edgeLength * 0.5f;

    GLfloat vertices[] =
    {
        // front face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

        // back face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom left

        // left face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

        // right face
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

        // top face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // bottom left

        // top face
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength  // bottom left
    };

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glColor3f( colour[0], colour[1], colour[2] );
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glDrawArrays(GL_QUADS, 0, 24);

    glDisableClientState(GL_VERTEX_ARRAY);
}