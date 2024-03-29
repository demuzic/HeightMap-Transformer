#define RAYGUI_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include "raylib.h"
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <raymath.h>
#include <string>
#include "GLOBAL.h"
#include <direct.h>
#include "raygui.h"

//VARIABLES
//system

int user_gizmo = 1;
int quick_move = 1;
int editor_gizmo = 0;

//fuctional
int KeyDown = 0;
float distance;
float preDistance;

float l1Mesh = 16;
float l2Mesh = 16;
float hMesh = 8;
std::string ImagePath = "";
Image image;
Texture2D texture;
Mesh mesh;
Model model;
Vector3 mapPosition = { -8.0f, 0.0f, -8.0f };
Rectangle EditorArea;
Vector2 EditorAreaDistance = { 50,20 };
int editor()
{

    float EditorSize = 25;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "HeightMapGen");
    SetTargetFPS(60);

    Camera camera = { 0 };
    camera.position = Vector3{ 18.0f, 21.0f, 18.0f };
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;


    while (!WindowShouldClose())
    {
       
        EditorArea = {EditorAreaDistance.x,EditorAreaDistance.y,GetScreenWidth()- EditorAreaDistance.x , GetScreenHeight()- EditorAreaDistance.y }; //Editor Area
        EditorAreaDistance.x = GetScreenWidth() * 0.15;
        EditorAreaDistance.y = 40;

        UpdateCamera(&camera); // CAMERA NAV
        SetCameraMode(camera, 1);
        BeginDrawing();
        ClearBackground(Color{ 20,20,20,255 });
        BeginMode3D(camera);
        //EDITOR
        DrawGrid(EditorSize + 2, 1.0f);
        int edGrid = (EditorSize / 2) + 2;

        if (editor_gizmo == 1)
        {
            DrawCylinderEx(Vector3{ -(float)edGrid,0,-(float)edGrid }, Vector3{ -(float)edGrid + 3,0,-(float)edGrid }, 0.1, 0.1, 15, RED);
            DrawCylinderEx(Vector3{ -(float)edGrid + 4,0,-(float)edGrid }, Vector3{ -(float)edGrid + 3,0,-(float)edGrid }, 0.01, 0.2, 15, RED); //GIZMO
            DrawCylinderEx(Vector3{ -(float)edGrid,0,-(float)edGrid }, Vector3{ -(float)edGrid,0,-(float)edGrid + 3 }, 0.1, 0.1, 15, BLUE);
            DrawCylinderEx(Vector3{ -(float)edGrid,0,-(float)edGrid + 3 }, Vector3{ -(float)edGrid,0,-(float)edGrid + 4 }, 0.2, 0.01, 15, BLUE);
            DrawCylinderEx(Vector3{ -(float)edGrid,0,-(float)edGrid }, Vector3{ -(float)edGrid,3,-(float)edGrid }, 0.1, 0.1, 15, GREEN);
            DrawCylinderEx(Vector3{ -(float)edGrid,4,-(float)edGrid }, Vector3{ -(float)edGrid,3,-(float)edGrid }, 0.01, 0.2, 15, GREEN);
        }



        
        //DrawHM
        DrawModel(model, mapPosition, 1, WHITE);
        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (droppedFiles.count == 1)
            {
                if (IsFileExtension(droppedFiles.paths[0], ".png") ||
                    IsFileExtension(droppedFiles.paths[0], ".jpeg") ||
                    IsFileExtension(droppedFiles.paths[0], ".jpg"))
                {
                    
                    image = LoadImage(droppedFiles.paths[0]);
                    texture = LoadTextureFromImage(image);
                    mesh = GenMeshHeightmap(image, Vector3{ l1Mesh,hMesh,l2Mesh });
                    model = LoadModelFromMesh(mesh);
                    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
                    UnloadDroppedFiles(droppedFiles);

                }
            }


        }


        if (IsKeyPressed(KEY_F1))
        {
            camera.projection = CAMERA_PERSPECTIVE;
        }
        if (IsKeyPressed(KEY_F2))
        {
            camera.projection = CAMERA_ORTHOGRAPHIC;
        }

        EndMode3D();
        //UI
        if (user_gizmo == 1)
        {
            int overXZ = 0;
            int overYZ = 0;
            int overYX = 0;
            Vector3 a = { camera.position.x - camera.target.x,camera.position.y - camera.target.y,camera.position.z - camera.target.z };
            float b = sqrt(pow(a.x, 2) + pow(a.y, 2));
            float c = sqrt(pow(b, 2) + pow(a.z, 2));
            distance = c;
            Vector2 stPos = GetWorldToScreen(camera.target, camera);
            Vector2 XendPos = GetWorldToScreen(Vector3{ (camera.target.x) + c / 30,camera.target.y,camera.target.z }, camera);
            Vector2 YendPos = GetWorldToScreen(Vector3{ camera.target.x ,camera.target.y + c / 30,camera.target.z }, camera);
            Vector2 ZendPos = GetWorldToScreen(Vector3{ camera.target.x ,camera.target.y,camera.target.z + c / 30 }, camera);

            Vector2 offset = { GetScreenWidth() / 2 - 50,-GetScreenHeight() / 2 + 90 };
            Color colArea = { 50,50,50,175 };

            Vector2 nstPos = Vector2{ stPos.x + offset.x,stPos.y + offset.y };
            Vector2 nXendPos = Vector2{ XendPos.x + offset.x,XendPos.y + offset.y };
            Vector2 nYendPos = Vector2{ YendPos.x + offset.x,YendPos.y + offset.y };
            Vector2 nZendPos = Vector2{ ZendPos.x + offset.x,ZendPos.y + offset.y };

            Vector2 XZendPos = GetWorldToScreen(Vector3{ camera.target.x + c / 30 ,camera.target.y,camera.target.z + c / 30 }, camera);
            Vector2 nXZendPos = Vector2{ XZendPos.x + offset.x, XZendPos.y + offset.y };
            int overarea = 0;
            int gizmopos = 0;
            //COLLISION
            if (nZendPos.y < nstPos.y && nXendPos.y > nstPos.y && nstPos.x < nZendPos.x && nstPos.x < nXendPos.x) {
                gizmopos = 1;
                //YZ   
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nZendPos.x, nZendPos.y - i }, 3)) {
                        overYZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 2;
                        }
                    }

                }
                //XZ
                for (float i = 0; i < Vector2Distance(nXendPos, nstPos); i++) {
                    float PROP = i / Vector2Distance(nXendPos, nstPos);
                    Vector2 fPoint = { nXendPos.x - (nXendPos.x - nstPos.x) * PROP,nXendPos.y - (nXendPos.y - nstPos.y) * PROP };

                    if (CheckCollisionPointLine(GetMousePosition(), fPoint, Vector2{ fPoint.x - (nXendPos.x - nXZendPos.x)  , fPoint.y - (nXendPos.y - nXZendPos.y) }, 3))
                    {
                        overXZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 3;
                        }
                    }

                }
                //YX
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nXendPos.x, nXendPos.y - i }, 3))
                    {
                        overYX = 1;
                        if (overarea == 0)
                        {
                            overarea = 1;
                        }
                    }
                }

                

                

            }
            if (nXendPos.y < nstPos.y && nZendPos.y > nstPos.y && nstPos.x > nZendPos.x && nstPos.x > nXendPos.x) {
                gizmopos = 2;
                //YX
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nXendPos.x, nXendPos.y - i }, 3))
                    {
                        overYX = 1;
                        if (overarea == 0)
                        {
                            overarea = 1;
                        }
                    }
                }
                //XZ
                for (float i = 0; i < Vector2Distance(nXendPos, nstPos); i++) {
                    float PROP = i / Vector2Distance(nXendPos, nstPos);
                    Vector2 fPoint = { nXendPos.x - (nXendPos.x - nstPos.x) * PROP,nXendPos.y - (nXendPos.y - nstPos.y) * PROP };

                    if (CheckCollisionPointLine(GetMousePosition(), fPoint, Vector2{ fPoint.x - (nXendPos.x - nXZendPos.x)  , fPoint.y - (nXendPos.y - nXZendPos.y) }, 3))
                    {
                        overXZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 3;
                        }
                    }

                }
                //YZ   
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nZendPos.x, nZendPos.y - i }, 3)) {
                        overYZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 2;
                        }
                    }

                }

                

            }
            if (nXendPos.y > nstPos.y && nZendPos.y > nstPos.y && nstPos.x < nZendPos.x && nstPos.x > nXendPos.x) {
                gizmopos = 3;
                //YX
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nXendPos.x, nXendPos.y - i }, 3))
                    {
                        overYX = 1;
                        if (overarea == 0)
                        {
                            overarea = 1;
                        }
                    }
                }

                //XZ
                for (float i = 0; i < Vector2Distance(nXendPos, nstPos); i++) {
                    float PROP = i / Vector2Distance(nXendPos, nstPos);
                    Vector2 fPoint = { nXendPos.x - (nXendPos.x - nstPos.x) * PROP,nXendPos.y - (nXendPos.y - nstPos.y) * PROP };

                    if (CheckCollisionPointLine(GetMousePosition(), fPoint, Vector2{ fPoint.x - (nXendPos.x - nXZendPos.x)  , fPoint.y - (nXendPos.y - nXZendPos.y) }, 3))
                    {
                        overXZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 3;
                        }
                    }

                }
                //YZ   
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nZendPos.x, nZendPos.y - i }, 3)) {
                        overYZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 2;
                        }
                    }

                }



            }
            if (nXendPos.y < nstPos.y && nZendPos.y < nstPos.y && nstPos.x > nZendPos.x && nstPos.x < nXendPos.x) {
                gizmopos = 4;
                //XZ
                for (float i = 0; i < Vector2Distance(nXendPos, nstPos); i++) {
                    float PROP = i / Vector2Distance(nXendPos, nstPos);
                    Vector2 fPoint = { nXendPos.x - (nXendPos.x - nstPos.x) * PROP,nXendPos.y - (nXendPos.y - nstPos.y) * PROP };

                    if (CheckCollisionPointLine(GetMousePosition(), fPoint, Vector2{ fPoint.x - (nXendPos.x - nXZendPos.x)  , fPoint.y - (nXendPos.y - nXZendPos.y) }, 3))
                    {
                        overXZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 3;
                        }
                    }

                }
                //YX
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nXendPos.x, nXendPos.y - i }, 3))
                    {
                        overYX = 1;
                        if (overarea == 0)
                        {
                            overarea = 1;
                        }
                    }
                }
                //YZ   
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nZendPos.x, nZendPos.y - i }, 3)) {
                        overYZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 2;
                        }
                    }

                }



            }
            if (nXendPos.y > nstPos.y && nZendPos.y > nstPos.y && nstPos.x > nZendPos.x && nstPos.x < nXendPos.x) {
                gizmopos = 5;
                //XZ
                for (float i = 0; i < Vector2Distance(nXendPos, nstPos); i++) {
                    float PROP = i / Vector2Distance(nXendPos, nstPos);
                    Vector2 fPoint = { nXendPos.x - (nXendPos.x - nstPos.x) * PROP,nXendPos.y - (nXendPos.y - nstPos.y) * PROP };

                    if (CheckCollisionPointLine(GetMousePosition(), fPoint, Vector2{ fPoint.x - (nXendPos.x - nXZendPos.x)  , fPoint.y - (nXendPos.y - nXZendPos.y) }, 3))
                    {
                        overXZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 3;
                        }
                    }

                }
                //YX
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nXendPos.x, nXendPos.y - i }, 3))
                    {
                        overYX = 1;
                        if (overarea == 0)
                        {
                            overarea = 1;
                        }
                    }
                }
                //YZ   
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nZendPos.x, nZendPos.y - i }, 3)) {
                        overYZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 2;
                        }
                    }

                }



            }
            if (nXendPos.y > nstPos.y && nZendPos.y < nstPos.y && nstPos.x > nZendPos.x && nstPos.x > nXendPos.x) {
                gizmopos = 6;
                //YX
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nXendPos.x, nXendPos.y - i }, 3))
                    {
                        overYX = 1;
                        if (overarea == 0)
                        {
                            overarea = 1;
                        }
                    }
                }
                //XZ
                for (float i = 0; i < Vector2Distance(nXendPos, nstPos); i++) {
                    float PROP = i / Vector2Distance(nXendPos, nstPos);
                    Vector2 fPoint = { nXendPos.x - (nXendPos.x - nstPos.x) * PROP,nXendPos.y - (nXendPos.y - nstPos.y) * PROP };

                    if (CheckCollisionPointLine(GetMousePosition(), fPoint, Vector2{ fPoint.x - (nXendPos.x - nXZendPos.x)  , fPoint.y - (nXendPos.y - nXZendPos.y) }, 3))
                    {
                        overXZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 3;
                        }
                    }

                }
                //YZ   
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nZendPos.x, nZendPos.y - i }, 3)) {
                        overYZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 2;
                        }
                    }

                }



            }
            if (nXendPos.y < nstPos.y && nZendPos.y < nstPos.y && nstPos.x < nZendPos.x && nstPos.x > nXendPos.x) {
                gizmopos = 7;
                //YZ
                    for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                        if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nZendPos.x, nZendPos.y - i }, 3)) {
                            overYZ = 1;
                            if (overarea == 0)
                            {
                                overarea = 2;
                            }
                        }

                    }
                //YX
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nXendPos.x, nXendPos.y - i }, 3))
                    {
                        overYX = 1;
                        if (overarea == 0)
                        {
                            overarea = 1;
                        }
                    }
                }
                //XZ
                for (float i = 0; i < Vector2Distance(nXendPos, nstPos); i++) {
                    float PROP = i / Vector2Distance(nXendPos, nstPos);
                    Vector2 fPoint = { nXendPos.x - (nXendPos.x - nstPos.x) * PROP,nXendPos.y - (nXendPos.y - nstPos.y) * PROP };

                    if (CheckCollisionPointLine(GetMousePosition(), fPoint, Vector2{ fPoint.x - (nXendPos.x - nXZendPos.x)  , fPoint.y - (nXendPos.y - nXZendPos.y) }, 3))
                    {
                        overXZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 3;
                        }
                    }

                }

            }
            if (nXendPos.y < nstPos.y && nZendPos.y > nstPos.y && nstPos.x < nZendPos.x && nstPos.x < nXendPos.x) {
                gizmopos = 8;
                //YZ
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nZendPos.x, nZendPos.y - i }, 3)) {
                        overYZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 2;
                        }
                    }

                }
                //YX
                for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                    if (CheckCollisionPointLine(GetMousePosition(), Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nXendPos.x, nXendPos.y - i }, 3))
                    {
                        overYX = 1;
                        if (overarea == 0)
                        {
                            overarea = 1;
                        }
                    }
                }
                //XZ
                for (float i = 0; i < Vector2Distance(nXendPos, nstPos); i++) {
                    float PROP = i / Vector2Distance(nXendPos, nstPos);
                    Vector2 fPoint = { nXendPos.x - (nXendPos.x - nstPos.x) * PROP,nXendPos.y - (nXendPos.y - nstPos.y) * PROP };

                    if (CheckCollisionPointLine(GetMousePosition(), fPoint, Vector2{ fPoint.x - (nXendPos.x - nXZendPos.x)  , fPoint.y - (nXendPos.y - nXZendPos.y) }, 3))
                    {
                        overXZ = 1;
                        if (overarea == 0)
                        {
                            overarea = 3;
                        }
                    }

                }

            }
            //CHANGE CAMERA
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (overarea == 1)
                {
                    if (gizmopos == 2 || gizmopos == 4 || gizmopos == 5 || gizmopos == 8 ) {
                        KeyDown = 1;
                    }
                    if (gizmopos == 1 || gizmopos == 3 || gizmopos == 6 || gizmopos == 7) {
                        KeyDown = 2;
                    }
                }
                if (overarea == 2)
                {
                    if (gizmopos == 2 || gizmopos == 3 || gizmopos == 7 || gizmopos == 8) {
                        KeyDown = 6;
                    }
                    if (gizmopos == 1 || gizmopos == 4 || gizmopos == 5 || gizmopos == 6) {
                        KeyDown = 5;
                    }
                }
                if (overarea == 3)
                {
                    if (gizmopos == 7 || gizmopos == 6 || gizmopos == 5 || gizmopos == 8) {
                        KeyDown = 3;

                    }
                    if (gizmopos == 1 || gizmopos == 2 || gizmopos == 4 || gizmopos == 3) {
                        KeyDown = 4;

                    }
                }
            }
            
            
            //DRAW
            //YX
            for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {
                if (overYX && overarea == 1)
                {
                    DrawLineEx(Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nXendPos.x, nXendPos.y - i }, 3, colArea);
                }
            }
            //YZ
            for (int i = 0; i < Vector2Distance(nstPos, nYendPos); i++) {

                if (overYZ && overarea == 2)
                {
                    DrawLineEx(Vector2{ nstPos.x, nstPos.y - i }, Vector2{ nZendPos.x, nZendPos.y - i }, 3, colArea);
                }
            }
            //XZ
            for (float i = 0; i < Vector2Distance(nXendPos, nstPos); i++) {
                float PROP = i / Vector2Distance(nXendPos, nstPos);
                Vector2 fPoint = { nXendPos.x - (nXendPos.x - nstPos.x) * PROP,nXendPos.y - (nXendPos.y - nstPos.y) * PROP };
                if (overXZ == 1 && overarea == 3)
                {
                    DrawLineEx(fPoint, Vector2{ fPoint.x - (nXendPos.x - nXZendPos.x)  , fPoint.y - (nXendPos.y - nXZendPos.y) }, 3, colArea);
                }

            }
            
            //LINE
            DrawLineEx(Vector2{ stPos.x + offset.x,stPos.y + offset.y }, Vector2{ XendPos.x + offset.x,XendPos.y + offset.y }, 3, RED);
            DrawLineEx(Vector2{ stPos.x + offset.x,stPos.y + offset.y }, Vector2{ YendPos.x + offset.x,YendPos.y + offset.y }, 3, GREEN);
            DrawLineEx(Vector2{ stPos.x + offset.x,stPos.y + offset.y }, Vector2{ ZendPos.x + offset.x,ZendPos.y + offset.y }, 3, BLUE);
            std::cout << overarea << std::endl;
        }

        if (quick_move == 1)
        {

            if (IsKeyPressed(320))
            {
                KeyDown = 1;
            }
            if (IsKeyPressed(325))
            {
                KeyDown = 2;
            }
            if (IsKeyPressed(328))
            {
                KeyDown = 3;
            }
            if (IsKeyPressed(322))
            {
                KeyDown = 4;
            }
            if (IsKeyPressed(324))
            {
                KeyDown = 5;
            }
            if (IsKeyPressed(326))
            {
                KeyDown = 6;
            }
            if (IsKeyPressed(KEY_LEFT_ALT) || IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
            {
                KeyDown = 0;
            }
            if (KeyDown == 1)
            {
                UpdateCamera(&camera);
                SetCameraMode(camera, 0);
                camera.position = Vector3{ Lerp(camera.position.x,camera.target.x,0.1),Lerp(camera.position.y,camera.target.y,0.1),Lerp(camera.position.z,preDistance,0.1) };


            }
            if (KeyDown == 2)
            {
                UpdateCamera(&camera);
                SetCameraMode(camera, 0);
                camera.position = Vector3{ Lerp(camera.position.x,camera.target.x,0.1),Lerp(camera.position.y,camera.target.y,0.1),Lerp(camera.position.z,-preDistance,0.1) };

            }
            if (KeyDown == 3)
            {
                UpdateCamera(&camera);
                SetCameraMode(camera, 0);
                camera.position = Vector3{ Lerp(camera.position.x,camera.target.x,0.1),Lerp(camera.position.y,preDistance,0.1),Lerp(camera.position.z,camera.target.z,0.1) };

            }
            if (KeyDown == 4)
            {
                UpdateCamera(&camera);
                SetCameraMode(camera, 0);
                camera.position = Vector3{ Lerp(camera.position.x,camera.target.x,0.1),Lerp(camera.position.y,-preDistance,0.1),Lerp(camera.position.z,camera.target.z,0.1) };

            }
            if (KeyDown == 5)
            {
                UpdateCamera(&camera);
                SetCameraMode(camera, 0);
                camera.position = Vector3{ Lerp(camera.position.x,preDistance,0.1),Lerp(camera.position.y,camera.target.y,0.1),Lerp(camera.position.z,camera.target.z,0.1) };

            }
            if (KeyDown == 6)
            {
                UpdateCamera(&camera);
                SetCameraMode(camera, 0);
                camera.position = Vector3{ Lerp(camera.position.x,-preDistance,0.1),Lerp(camera.position.y,camera.target.y,0.1),Lerp(camera.position.z,camera.target.z,0.1) };

            }

            if (KeyDown == 0)
            {
                preDistance = distance;
                UpdateCamera(&camera);
                SetCameraMode(camera, 1);
            }

        }

        

        //TOOLBAR
        Rectangle ToolbarArea = { 0,0,EditorArea.x,GetScreenHeight() };
        DrawRectangleRec(ToolbarArea, GRAY);
        

        
        EndDrawing();
    }
    CloseWindow();
    return 0;

}
