#include <stdio.h>
#include "include/glad/glad.h"
#include "glad.c"
#include "GLFW/glfw3.h"
#include "glm/common.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

typedef unsigned int UI;
UI Pgm;

struct Data
{
    UI VAO;
    UI VBO;
};

Data BoxData()
{
    Data a = {};
    float box[]{
        0, 0,
        0, 1,
        1, 1,
        1, 1,
        1, 0,
        0, 0};
    glGenVertexArrays(1, &a.VAO);
    glBindVertexArray(a.VAO);
    glGenBuffers(1, &a.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, a.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 8, 0);
    return a;
};

void DrawBox(Data a, int x, int y, float colorX, float colorY, float colorZ, UI size)
{
    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(x, y, 0));
    Model = glm::scale(Model, glm::vec3(size, size, 0));
    glUniformMatrix4fv(glGetUniformLocation(Pgm, "Model"), 1, 0, glm::value_ptr(Model));
    glUniform3f(glGetUniformLocation(Pgm, "BoxColor"), colorX, colorY, colorZ);
    glBindVertexArray(a.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, a.VBO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(500, 500, "Tetris", 0, 0);
    glfwMakeContextCurrent(window);

    gladLoadGL();

    const char *Vsource = R"(
        #version 330 core
        layout (location = 0) in vec2 pos;
        uniform mat4 Proj;
        uniform mat4 Model;
        void main()
        {
            gl_Position = Proj * Model * vec4(pos,0,1);
        }
    )";

    const char *Fsource = R"(
        #version 330 core
        out vec4 Color;
        uniform vec3 BoxColor;
        
        void main()
        {
            Color = vec4(BoxColor,1);
        }
    )";

    UI vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &Vsource, 0);
    glCompileShader(vs);

    char info[1000];
    glGetShaderInfoLog(vs, 1000, 0, info);
    printf("%s", info);

    UI fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &Fsource, 0);
    glCompileShader(fs);
    glGetShaderInfoLog(fs, 1000, 0, info);
    printf("%s", info);

    Pgm = glCreateProgram();
    glAttachShader(Pgm, vs);
    glAttachShader(Pgm, fs);
    glLinkProgram(Pgm);
    glUseProgram(Pgm);
    glGetProgramInfoLog(Pgm, 1000, 0, info);
    printf("%s", info);

    glm::mat4 Proj(1.0f);
    Proj = glm::ortho(0.0f, 500.0f, 500.0f, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(Pgm, "Proj"), 1, 0, glm::value_ptr(Proj));

    Data Box = BoxData();
    glfwSwapInterval(9);
    int HeadX = 250;
    int HeadY = 250;
    int TailX[50] = {};
    int TailY[50] = {};
    int Taillength = 4;
    int DirX = 26;
    int DirY = 26;
    bool Move = true;
    bool GameStat = true;
    int FoodX[50];
    int FoodY[50];
    int FoodNo = 0;
    srand(5);
    bool FoodLoop = true;
    bool PosXDir = true;
    bool PosYDir = true;
    bool NegXDir = true;
    bool NegYDir = true;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 0, 0, 1);

        if (GameStat == true)
        {

            // TODO: Snake Movement
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && NegYDir == true)
            {
                DirY = -26;
                Move = false;
                PosYDir = false;
                PosXDir = true;
                NegXDir = true;
            }
            else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && PosYDir == true)
            {
                DirY = 26;
                Move = false;
                NegYDir = false;
                PosXDir = true;
                NegXDir = true;

            }
            else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && PosXDir == true)
            {
                DirX = 26;
                Move = true;
                NegXDir = false;
                PosYDir = true;
                NegYDir = true;
            }
            else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && NegXDir == true)
            {
                DirX = -26;
                Move = true;
                PosXDir = false;
                PosYDir = true;
                NegYDir = true;
            }

            if (Move == true)
            {
                HeadX += DirX;
            }
            else
            {
                HeadY += DirY;
            }

            int PrevX = HeadX;
            int PrevY = HeadY;

            for (int i = 0; i < Taillength; i++)
            {
                int Prev2X = TailX[i];
                int Prev2Y = TailY[i];

                TailX[i] = PrevX;
                TailY[i] = PrevY;

                PrevX = Prev2X;
                PrevY = Prev2Y;
            };

            // TODO: Collision with walls and tail

            if (HeadX <= 0 || HeadX >= 500 || HeadY <= 0 || HeadY >= 500)
            {
                GameStat = false;
            }

            for(int i = 1;i < Taillength;i++)
            {
                if(HeadX == TailX[i] && HeadY == TailY[i])
                {
                    GameStat = false;
                }
            }
        }

        //TODO: food System
        
        if(FoodLoop == true)
        {
            for(int i = 0; i < 50;i++)
            {
                FoodX[i] = rand()%450;
                FoodY[i] = rand()%450;
            }
            FoodLoop = false;
        };
        
        DrawBox(Box,FoodX[FoodNo],FoodY[FoodNo],0,1,0,35);

        if(HeadX >= (FoodX[FoodNo]) && HeadX <= (FoodX[FoodNo]+35) && HeadY >= (FoodY[FoodNo]) && HeadY <= (FoodY[FoodNo]+35))
        {
            FoodNo++;
            Taillength++;
        }
        //TODO: Restart
        if(GameStat == false)
        {
            if(glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS)
            {
                GameStat = true;
                Taillength = 4;
                FoodNo = 0;
                HeadX = 250;
                HeadY = 250;
            }
        }

        //TODO: Snake rendering 
        for (int i = 1; i < Taillength; i++)
        {
            DrawBox(Box,TailX[0],TailY[0],1,0,0,25);
            DrawBox(Box, TailX[i], TailY[i], 1, 1, 1, 25);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 07;
}