#include "raylib.h"
#include <stdio.h>
#include "dataHandling.h"
#include "recpp.h"
#include "boxPP.h"
#include "vector2pp.h"

#define GHOST (Color){130, 130, 130, 130}
#define LIGHTGHOST (Color){255, 255, 255, 130}

Rectangle correctRect(Rectangle rect){
    Rectangle temp = rect;
    if(temp.width < 0){
        temp.x += temp.width;
        temp.width = -temp.width;
    }
    if(temp.height < 0){
        temp.y += temp.height;
        temp.height = -temp.height;
    }
    return temp;
}

void DrawRectangleC(Rectangle rect, Color color){
    DrawRectangleRec(correctRect(rect), color);
}

BoxCollider2D rectToBox(BoxCollider2D box, Rectangle rect){
    BoxCollider2D temp = box;
    temp.x = rect.x;
    temp.y = rect.y;
    temp.sizeX = rect.width;
    temp.sizeY = rect.height;
    return temp;
}

PhysicsObject v2ToObj(PhysicsObject obj, Vector2 v2){
    PhysicsObject temp = obj;
    temp.position = v2;
    return temp;
}


int main(int argc, char* argv[]){
    int screenWidth = 1920;
    int screenHeight = 1080;
    int screenFPS = 60;

    //Get Window properties
    if(argc == 1){
        //Window properties prompt
        printf("Enter Resolution (recommended: 135, 270, 540, 1080): ");
        scanf("%d", &screenHeight);
        screenWidth = screenHeight / 9.00f * 16.00f;

        printf("Enter desired FPS: ");
        scanf("%d", &screenFPS);

    }else{
        printf("Getting arg1: ");
        screenHeight = parseInt(argv[1], 10);
        screenWidth = screenHeight / 9.00f * 16.00f;

        printf("Getting arg2: ");
        screenFPS = parseInt(argv[2], 10);

        if(screenHeight == 0){
            printf("ERROR: readArgs - screenHeight (argument 1) is not valid\n");
        }
        if(screenFPS == 0){
            printf("ERROR: readArgs - screenFPS (argument 2) is not valid\n");
        }

        if(screenHeight == 0 || screenFPS == 0){
            //Window properties prompt
            printf("Enter Resolution (recommended: 135, 270, 540, 1080): ");
            scanf("%d", &screenHeight);
            screenWidth = screenHeight / 9.00f * 16.00f;

            printf("Enter desired FPS: ");
            scanf("%d", &screenFPS);
        }

        /*if(argv[3] == "1" || argv[3] == "s"){

        }*/
    }

    char levelImagePath[64];    //
    char backgroundPath[64];    //
    Vector2 startingPos = {0,0};    //
    Vector2 startingPos2 = {0,0};   //

    //objects
    BoxCollider2D Col[64];  //
    TextBox levelText[2];   //
    PhysicsObject crate[8]; //
    BoxCollider2D ladderCol[16];    //

    //# of objs
    int colliderNum = 0;    //
    int textNum = 0;    //
    int ladderNum = 0;  //
    int crateNum = 0;   //

    //other level properties
    int leverNum = 0;   //
    int doorNum = 0;    //
    int portalNum = 0;  //
    //char levelPath[128];

    //debug variables
    //bool ColliderDebugMode = false;
    //int ShowCollider = 0;

    int isMultiplayer = true;
    int wallTags[16];
    int wallNum;
    uint16_t wallEnabled;
    BoxCollider2D goal;
    int scrollType = 0;
    uint64_t leverFlip;

    printf("Enter a level path:\n");
    char path[128];
    scanf("%s", path);

    readFileSF(path, levelImagePath, backgroundPath, &startingPos, &startingPos2, Col, ladderCol, levelText, crate, &textNum, &colliderNum, &ladderNum, &crateNum, &leverNum, &doorNum, &isMultiplayer, &portalNum, wallTags, &wallNum, &wallEnabled, &goal, &scrollType, &leverFlip);

    InitWindow(screenWidth, screenHeight, "SF-Editor");
    SetTargetFPS(screenFPS);
    Camera2D camera;
    camera.zoom = 1;
    camera.rotation = 0;
    camera.offset.x = screenWidth / 2;
    camera.offset.y = screenHeight / 2;
    float cameraSpeed = 100;
    float zoomSpeed = 3.0f;

    Rectangle colRects[64];
    Rectangle ladderRects[8];
    Vector2 cratePos[8];
    for(int i = 0; i < colliderNum + portalNum + leverNum + doorNum; i++){
        colRects[i] = boxToRec(Col[i]);
    }
    for(int i = 0; i < ladderNum; i++){
        ladderRects[i] = boxToRec(ladderCol[i]);
    }
    for(int i = 0; i < crateNum; i++){
        cratePos[i] = crate[i].position;
    }
    int rectSelect = 0;
    int renderMode = 0;
    int editMode = 0;
    Texture2D img = LoadTexture(levelImagePath);

    while(!WindowShouldClose()){
        if(IsKeyPressed(KEY_R)){
            readFileSF("temp.sf", levelImagePath, backgroundPath, &startingPos, &startingPos2, Col, ladderCol, levelText, crate, &textNum, &colliderNum, &ladderNum, &crateNum, &leverNum, &doorNum, &isMultiplayer, &portalNum, wallTags, &wallNum, &wallEnabled, &goal, &scrollType, &leverFlip);
            UnloadTexture(img);
            img = LoadTexture(levelImagePath);
            for(int i = 0; i < colliderNum + portalNum + leverNum + doorNum; i++){
                colRects[i] = boxToRec(Col[i]);
            }
            for(int i = 0; i < ladderNum; i++){
                ladderRects[i] = boxToRec(ladderCol[i]);
            }
            for(int i = 0; i < crateNum; i++){
                cratePos[i] = crate[i].position;
            }
            rectSelect = 0;
            renderMode = 0;
            editMode = 0;
        }
        camera.zoom *= 1 + GetMouseWheelMove() * GetFrameTime() * zoomSpeed;
        //printf("%f\n", camera.zoom);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) colRects[rectSelect] = (Rectangle){
            (int)((GetMouseX() - camera.offset.x) / camera.zoom + camera.target.x),
            (int)((GetMouseY() - camera.offset.y) / camera.zoom + camera.target.y),
            0,0
        };


        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            colRects[rectSelect].width = (int)((GetMouseX() - camera.offset.x) / camera.zoom + camera.target.x - colRects[rectSelect].x);
            colRects[rectSelect].height = (int)((GetMouseY() - camera.offset.y) / camera.zoom + camera.target.y - colRects[rectSelect].y);
        }


        if(IsKeyDown(KEY_W)) camera.target.y -= cameraSpeed * GetFrameTime() / camera.zoom;
        if(IsKeyDown(KEY_S)) camera.target.y += cameraSpeed * GetFrameTime() / camera.zoom;
        if(IsKeyDown(KEY_A)) camera.target.x -= cameraSpeed * GetFrameTime() / camera.zoom;
        if(IsKeyDown(KEY_D)) camera.target.x += cameraSpeed * GetFrameTime() / camera.zoom;
        if(IsKeyPressed(KEY_F11)) ToggleFullscreen();
        if(IsKeyPressed(KEY_TAB)){
            renderMode++;
            if(renderMode > 2) renderMode = 0;
            if(renderMode == 0){
                for(int i = 0; i < colliderNum + portalNum + leverNum + doorNum; i++){
                    Col[i] = rectToBox(Col[i], colRects[i]);
                }
            }
        }
        if(IsKeyPressed(KEY_SPACE)){
            rectSelect++;
            if(rectSelect >= colliderNum + portalNum + leverNum + doorNum) rectSelect = 0;
        }
        for(int i = KEY_ZERO; i < KEY_NINE + 1; i++){
            if(IsKeyPressed(i)){
                editMode = i - KEY_ZERO;
            }
        }
        if(IsKeyPressed(KEY_LEFT_CONTROL) && IsKeyDown(KEY_S)){
            printf("Saving...\n");
            for(int i = 0; i < colliderNum + portalNum + leverNum + doorNum; i++){
                    Col[i] = rectToBox(Col[i], colRects[i]);
            }
            writeFileSF("temp.sf", levelImagePath, backgroundPath, startingPos, startingPos2, Col, ladderCol, levelText, crate, textNum, colliderNum, ladderNum, crateNum, leverNum, doorNum, isMultiplayer, portalNum, wallTags, wallNum, wallEnabled, goal, scrollType, leverFlip);
            printf("Saved!\n");
        }


        BeginDrawing();
            BeginMode2D(camera);
                //if(IsKeyPressed(KEY_LEFT_CONTROL) && IsKeyDown(KEY_S)) ClearBackground(LIGHTGRAY);
                
                if(renderMode == 0){
                    ClearBackground(GRAY);
                    DrawTexture(img, 0,0,WHITE);
                    //DrawRectangleC(colRects[rectSelect], BLACK);
                    int colsToResize = colliderNum + leverNum + doorNum + portalNum;
                    for(int i = 0; i < colsToResize; i++){
                        int result = 0;
                        if(pointInBox(addVec2(resizeVec2(subVec2(GetMousePosition(), camera.offset), 1/camera.zoom), camera.target), Col[i])){
                            if(Col[i].trigger > 0){
                                result = 5;
                            }else if(Col[i].ladder == true){
                                result = 6;
                            }else{
                                result = 1;
                            }
                        }
                        switch(result){
                            case 0:
                                DrawRectangle(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, WHITE);
                                break;
                            case 1:
                                DrawRectangle(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, RED);
                                break;
                            case 5:
                                DrawRectangle(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, BROWN);
                                break;
                            case 6:
                                DrawRectangle(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, GRAY);
                                break;
                        }
                        //DrawRectangleLines(Col[i].x, Col[i].y, Col[i].sizeX, Col[i].sizeY, BLACK);
                    }
                    for(int i = 0; i < ladderNum; i++){
                        if(pointInBox(addVec2(resizeVec2(subVec2(GetMousePosition(), camera.offset), 1/camera.zoom), camera.target), ladderCol[i])){
                            DrawRectangle(ladderCol[i].x, ladderCol[i].y, ladderCol[i].sizeX, ladderCol[i].sizeY, SKYBLUE);
                        }else{
                            DrawRectangle(ladderCol[i].x, ladderCol[i].y, ladderCol[i].sizeX, ladderCol[i].sizeY, LIGHTGRAY);
                        }
                        //DrawRectangleLines(ladderCol[i].x, ladderCol[i].y, ladderCol[i].sizeX, ladderCol[i].sizeY, BLACK);
                    }
                    for(int i = 0; i < crateNum; i++){
                        DrawRectangle(crate[i].position.x, crate[i].position.y, crate[i].sizeX, crate[i].sizeY, MAROON);
                    }
                    DrawRectangle(goal.x, goal.y, goal.sizeX, goal.sizeY, DARKGREEN);
                    //DrawRectangleLines(goal.x, goal.y, goal.sizeX, goal.sizeY, BLACK);
                    for(int i = 0; i < textNum; i++){
                        DrawText(levelText[i].text, levelText[i].x, levelText[i].y, levelText[i].size, levelText[i].colour);
                    }
                }else{
                    ClearBackground(RAYWHITE);
                    for(int i = 0; i < colliderNum + portalNum + leverNum + doorNum; i++){
                        DrawRectangleC(colRects[i], rectSelect == i ? GREEN : BLUE);
                    }
                    if(renderMode == 2){
                        for(int i = 0; i < ladderNum; i++){
                            DrawRectangleC(ladderRects[i], BLACK);
                        }
                        for(int i = 0; i < crateNum; i++){
                            DrawRectangleV(cratePos[i], (Vector2){15,13}, MAROON);
                        }
                        //DrawRectangle(goal.x, goal.y, goal.sizeX, goal.sizeY, DARKGREEN);
                        for(int i = 0; i < textNum; i++){
                            DrawText(levelText[i].text, levelText[i].x, levelText[i].y, levelText[i].size, levelText[i].colour);
                        }
                    }
                    DrawTexture(img, 0, 0, LIGHTGHOST);
                    
                }
            EndMode2D();
            DrawText(TextFormat("tx: %f", camera.target.x), 10,10,20,BLUE);
            DrawText(TextFormat("ty: %f", camera.target.y), 10,40,20,BLUE);
            DrawText(TextFormat("rx: %f", colRects[rectSelect].x), 10,70,20,BLUE);
            DrawText(TextFormat("ry: %f", colRects[rectSelect].y), 10,100,20,BLUE);
            DrawText(TextFormat("rw: %f", colRects[rectSelect].width), 10,130,20,BLUE);
            DrawText(TextFormat("calc: %f", (GetMouseX() - camera.offset.x) / camera.zoom + camera.target.x - colRects[rectSelect].x), 160,130,20,BLUE);
            
            DrawText(TextFormat("rh: %f", colRects[rectSelect].height), 10,160,20,BLUE);
            DrawText(TextFormat("select: %d", rectSelect), 10,190,20,BLUE);
            DrawText(TextFormat("edit mode: %d", editMode), 10,220,20,BLUE);
            if(IsKeyPressed(KEY_LEFT_CONTROL) && IsKeyDown(KEY_S))
                DrawText("Saved!", 10, 250, 40, BLACK);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}