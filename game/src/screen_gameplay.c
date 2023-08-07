/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"
#include "raymath.h"

#pragma region Variables + Structs
static int framesCounter = 0;
static int finishScreen = 0;
const int paddleWidth = 16;
const int paddleHeight = 64;
const int ballSize = 16;
const float ballSpeed = 4.0f;
const float ballServeDuration = 2.0f;
const float ballScoreDuration = 2.0f;
const float maxDeflectAngle = 70.0f;

enum BallState { SERVING, MOVING, SCORED } ballState;

struct Paddle {
    float x, y;
    float moveSpeed;
    int score;
    char scoreStr[3];
} player1, player2;

struct Ball {
    float x, y;
    float velX, velY;
    float servingTime;
} ball;
#pragma endregion

#pragma region Gameplay Functions
// Get the deflection angle (in degrees) for the ball's position to the given paddle
float GetDeflectionAngle(struct Paddle *paddle)
{
    float ballY = ball.y - paddle->y;
    float angle = Lerp(-maxDeflectAngle, maxDeflectAngle, ballY / paddleHeight);
    
    if (ball.x < paddle->x)
    {
        angle = 180.0f - angle;
    }

    return angle;
}

// Set the ball's 2D velocity to move at the given angle (in degrees)
void SetBallVelocity(float angle)
{
    float rad = angle * PI / 180.0f;
    ball.velX = cos(rad) * ballSpeed;
    ball.velY = sin(rad) * ballSpeed;
}

// Update the ball's movement and velocity and check for collision
void UpdateBallMovement()
{
    // Move the ball
    ball.x += ball.velX;
    ball.y += ball.velY;

    // Bounce off the top/bottom of the screen
    if (ball.y <= 0.0f) {
        ball.velY = -ball.velY;
        ball.y = 1.0f;
    }
    else if (ball.y >= GetScreenHeight() - ballSize)
    {
        ball.velY = -ball.velY;
        ball.y = (float)GetScreenHeight() - (float)ballSize - 1.0f;
    }

    // If the ball goes off the side, score a point for the other player
    if (ball.x < ballSize - 20.0f) {
        ++player2.score;
        ball.servingTime = 0.0f;
        ball.x = (float)GetScreenWidth() * 0.5f;
        ball.y = (float)GetScreenHeight() * 0.5f;
        ballState = SCORED;
    }
    else if (ball.x > GetScreenWidth() + 20.0f)
    {
        ++player1.score;
        ball.servingTime = 0.0f;
        ball.x = (float)GetScreenWidth() * 0.5f;
        ball.y = (float)GetScreenHeight() * 0.5f;
        ballState = SCORED;
    }

    // Check for collision with Player 1's paddle
    if (ball.x + ballSize > player1.x && ball.x < player1.x + paddleWidth)
    {
        if (ball.y + ballSize > player1.y && ball.y < player1.y + paddleHeight)
        {
            SetBallVelocity(GetDeflectionAngle(&player1));
            ball.x = player1.x + (float)paddleWidth + 1;
        }
    }

    // Check for collision with Player 2's paddle
    if (ball.x + ballSize > player2.x && ball.x < player2.x + paddleWidth)
    {
        if (ball.y + ballSize > player2.y && ball.y < player2.y + paddleHeight)
        {
            SetBallVelocity(GetDeflectionAngle(&player2));
            ball.x = player2.x - (float)ballSize - 1.0f;
        }
    }
}
#pragma endregion

#pragma region Drawing Functions
// Put this into a function to expand on later
void DrawBackground()
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), DARKGRAY);
}

// Draw a paddle plus score text
void DrawPaddleAndScore(struct Paddle *paddle, int textX, int textY)
{
    DrawRectangle(paddle->x, paddle->y, paddleWidth, paddleHeight, WHITE);
    sprintf(paddle->scoreStr, "%d", paddle->score);
    DrawText(paddle->scoreStr, textX, textY, 40, GRAY);
}

// Put this into a function to expand on later
void DrawBall(struct Ball* ball)
{
    DrawRectangle(ball->x, ball->y, ballSize, ballSize, WHITE);
}
#pragma endregion

#pragma region Screen State Functions
// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    const float halfScreenHeight = ((float)GetScreenHeight() / 2.0f) - ((float)paddleHeight / 2.0f);

    // Place players starting positions
    player1.x = 20;
    player1.y = halfScreenHeight;
    player2.x = (float)GetScreenWidth() - (float)paddleWidth - 20.0f;
    player2.y = halfScreenHeight;
    player2.moveSpeed = 250.0f;

    // Set ball's starting position
    ball.x = (float)GetScreenWidth() / 2.0f;
    ball.y = (float)GetScreenHeight() / 2.0f;
    ballState = SERVING;

    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // Update Player 1 position
    if (IsCursorOnScreen())
    {
        player1.y = Clamp((float)GetMouseY() - (paddleHeight / 2), 0.0f, (float)GetScreenHeight() - paddleHeight);
    }

    // Update Player 2 position
    if (IsKeyDown(KEY_UP))
    {
        player2.y -= player2.moveSpeed * GetFrameTime();
        if (player2.y < 0.0f) player2.y = 0.0f;
    }
    else if (IsKeyDown(KEY_DOWN))
    {
        player2.y += player2.moveSpeed * GetFrameTime();
        if (player2.y > (float)GetScreenHeight() - paddleHeight) player2.y = (float)GetScreenHeight() - paddleHeight;
    }

    // Update Ball
    switch (ballState)
    {
    case SERVING:
        ball.servingTime += GetFrameTime();

        if (ball.servingTime >= ballServeDuration)
        {
            float angle = (float)GetRandomValue(-45, 45);
            if (GetRandomValue(0, 99) >= 50)
            {
                angle += 180.0f;
            }

            SetBallVelocity(angle);
            ballState = MOVING;
        }
        break;

    case MOVING:
        UpdateBallMovement();
        break;

    case SCORED:
        ball.servingTime += GetFrameTime();

        if (ball.servingTime >= ballScoreDuration)
        {
            ballState = SERVING;
            ball.servingTime = 0.0f;
        }
        break;
    }

    // Press enter or tap to change to ENDING screen
    /*
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
    */
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    DrawBackground();

    // Draw Players & Scores
    DrawPaddleAndScore(&player1, player1.x, 20);
    DrawPaddleAndScore(&player2, player2.x, 20);

    // Draw Ball
    DrawBall(&ball);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}
#pragma endregion