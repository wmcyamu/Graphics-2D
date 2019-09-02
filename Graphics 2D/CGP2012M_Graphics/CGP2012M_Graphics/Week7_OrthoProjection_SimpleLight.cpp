#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "Windows.h"

//include shape, shader header files
#include "GLerror.h"
#include "SDL_Start.h"
#include "Triangle_T.h"
#include "CircleTexture.h"
#include "Square.h"
#ifndef TEXTURECLASS_H
#define TEXTURECLASS_H
#ifndef SHADERCLASS_H
#define SHADERCLASS_H

// // GLEW - OpenGL Extension Wrangler - http://glew.sourceforge.net/
// // NOTE: include before SDL.h
#ifndef GLEW_H
#define GLEW_H
//#include <GL/glew.h>


// SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#ifndef SDL_H
#define SDL_H
#include "SDL.h"
#include "SDL_image.h"
//#include "SDL_mixer.h"
//#include "SDL_ttf.h"

// // - OpenGL Mathematics - https://glm.g-truc.net/
#define GLM_FORCE_RADIANS // force glm to use radians
// // NOTE: must do before including GLM headers
// // NOTE: GLSL uses radians, so will do the same, for consistency
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//***************
const Uint8 *keyState = SDL_GetKeyboardState(NULL);
//variables
//bool wu = false;
//bool a = false;
//bool s = false;
//bool d = false;
//bool up = false;
//bool lefto = false;
//bool down = false;
//bool right = false;

float playerx;
float playery;
SDL_Event event;
SDL_Window *win;
bool windowOpen = true;
bool isFullScreen = false;

//screen boundaries for collision tests
float bX_r = 3.8f;
float bX_l = 0.2f;
float bY_t = 2.8f;
float bY_b = 0.2f;
int bubbleNum = 1;
float PRLx;
float PRLy;
unsigned int LT = 0, CT = 0;
//screen centre

//window aspect/width/height
int w;
int h;
float aspect;
int left;
int newwidth;
int newheight;

//transform matrices
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

glm::mat4 translate;

glm::mat4 rotate;
glm::mat4 scale;
glm::mat4 backgroundTranslate;
glm::mat4 backgroundScale;
glm::vec3 b_scaleFactor;
float angle = 0;

std::vector<CircleTexture*> circles;

glm::vec3 lightCol;
float ambientIntensity;

//**************
//function prototypes

bool playerDead = false;

void handleInput();
void checkForKeyPresses(void);
void HPC()
{
	for (int i = 0; i < bubbleNum; i++)
	{
		CircleTexture* crc = circles[i];
		glm::vec3 pp = translate[3];
		glm::vec3 cp = crc->circleTranslate[3];

		float dist = glm::distance(pp, cp);

		if (dist < crc->radius)
		{
			//they've collided
			//crc->bubbleSpeedx = crc->bubbleSpeedy = 0;
			playerDead = true;
		}
	}
}

int main(int argc, char *argv[]) {
	//start and initialise SDL
	SDL_Start sdl;
	SDL_GLContext context = sdl.Init();
	win = sdl.win;

	SDL_GetWindowSize(win, &w, &h);
	glViewport(0, 0, w, h);
	aspect = (float)w / (float)h;

	//error class
	GLerror glerr;
	int errorLabel;

	//GLEW initialise
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	//register debug callback
	if (glDebugMessageCallback)
	{
		std::cout << "Registering OpenGL Debug callback function" << std::endl;
		glDebugMessageCallback(glerr.openglCallbackFunction, &errorLabel);
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			NULL,
			true);
	}

	//*****************************************************
	//OpenGL specific data
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//create triangle
	Triangle_T tri_T;
	Square Player;
	//create background square
	Square background;

	//create circles
	float randValue, randValue2;
	srand(time(0));
	

	PRLx = rand() % 3 + 0.4;
	PRLy = rand() % 2 + 0.3;

	/*bubbleSpeedx = rand() % 1/30 + 0.01;
	bubbleSpeedy = rand() % 1/30 + 0.01;*/

	for (int i = 0; i < 99; i++)
	{
		//circles.push_back(CircleTexture(radius, (randValue-0.5), (randValue2 -0.5f)));
		circles.push_back(new CircleTexture(0.2, 0.0f, 0.0f));
	}

	errorLabel = 0;

	//*********************
	//create texture collection
	//create textures - space for 4, but only using 1
	Texture texArray[4];
	//background texture
	//got texture from http://www.pngall.com/space-png/download/14520
	
	texArray[0].load("..//..//Assets//Textures//newbg.png");

	// texture from https://www.pngkey.com/maxpic/u2w7r5o0o0q8i1a9/
	texArray[1].load("..//..//Assets//Textures//gO.png");
	texArray[0].setBuffers();
	texArray[1].setBuffers();

	errorLabel = 2;

	//OpenGL buffers
	tri_T.setBuffers();
	background.setBuffers();
	//set buffers for the circles
	for (int q = 0; q < 99; q++)
	{
		circles[q]->setBuffers();
	}

	errorLabel = 3;
	//*****************************************
	//set uniform variables
	int transformLocation;
	int modelLocation;
	int viewLocation;
	int projectionLocation;
	int circleTransformLocation;
	int backgroundColourLocation;
	int ambientIntensityLocation;

	//light colour initial setting
	lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
	//light distance setting
	ambientIntensity = 1.0f;

	//initialise transform matrices 
	//orthographic (2D) projection
	projectionMatrix = glm::ortho(0.0f, 4.0f, 0.0f, 3.0f, -1.0f, 100.0f);
	//initialise view matrix to '1'
	viewMatrix = glm::mat4(1.0f);

	
	translate = glm::mat4(1.0f);
	rotate = glm::mat4(1.0f);
	scale = glm::mat4(1.0f);
	backgroundScale = glm::mat4(1.0f);
	backgroundTranslate = glm::mat4(1.0f);

	//once only scale to background
	b_scaleFactor = { 20.0f, 15.0f, 1.0f };
	backgroundScale = glm::scale(backgroundScale, glm::vec3(b_scaleFactor));
	backgroundTranslate = glm::translate(backgroundTranslate, glm::vec3(2.0f, 1.5f, 0.0f));

	//once only scale to triangle
	playerx = 0.2f;
	playery = 0.2f;
	scale = glm::scale(scale, glm::vec3(playerx, playery, 0.0f));

	

	//once only translate - translate triangle into centre of screen
	glUseProgram(tri_T.shaderProgram1);
	translate = glm::translate(translate, glm::vec3(2.0f, 1.5f, 0.0f));
	modelLocation = glGetUniformLocation(tri_T.shaderProgram1, "uModel");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(translate*rotate*scale));
	//once only translate - translate circle into centre of screen
	

	errorLabel = 4;



	///time logic 
	//render the circles
	unsigned int lastTime = 0, currentTime;

	/*int q = 0;
	int qn = 1;*/
	

	//*****************************
	//'game' loop
	while (windowOpen)
	{
		//*************************
		
		//process inputs
		
		handleInput();


		HPC();
		

		/*if (translate[3].x < (bX_r - translate[3].x))
		{
			glm::translate(translate, glm::vec3(0.3f, translate[3].y, 0.0f));
		}*/

		/*if (translate[3].x <= bX_l)
		{
			glm::translate(translate, glm::vec3(0.3f,translate[3].y, 0.0f));
		}*/

		/*if (translate[3].x + 0.02f == bX_r)
		{
			glm::translate(translate, glm::vec3((float)cos(angle)*-0.05f, (float)sin(angle)*-0.05f, 0.0f));
		}

		if (translate[3].y >= bY_t)
		{
			glm::translate(translate, glm::vec3((float)cos(angle)*-0.05f, (float)sin(angle)*-0.05f, 0.0f));
		}

		if (translate[3].y <= bY_b)
		{
			glm::translate(translate, glm::vec3((float)cos(angle)*-0.05f, (float)sin(angle)*-0.05f, 0.0f));
		}*/
		
		//****************************
		// OpenGL calls.
		
		glClearColor(1.0f, 1.0f, 1.0f, 1);
		glClear(GL_COLOR_BUFFER_BIT); 

		errorLabel = 5;
		//background
		glUseProgram(background.shaderProgram);

		//set background lighting
		backgroundColourLocation = glGetUniformLocation(background.shaderProgram, "uLightColour");
		glProgramUniform3fv(background.shaderProgram, backgroundColourLocation, 1, glm::value_ptr(lightCol));
		//light distance
		ambientIntensityLocation = glGetUniformLocation(background.shaderProgram, "uAmbientIntensity");
		glProgramUniform1f(background.shaderProgram, ambientIntensityLocation, ambientIntensity);

		//set background image
		modelLocation = glGetUniformLocation(background.shaderProgram, "uModel");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(backgroundTranslate*backgroundScale));
		viewLocation = glGetUniformLocation(background.shaderProgram, "uView");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		projectionLocation = glGetUniformLocation(background.shaderProgram, "uProjection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		if (!playerDead)
			glBindTexture(GL_TEXTURE_2D, texArray[0].texture);
		else
			glBindTexture(GL_TEXTURE_2D, texArray[1].texture);

		background.render();

		//specify shader program to use
		//to allow transform uniform to be passed in
		glUseProgram(tri_T.shaderProgram1);

		//set projection matrix uniform and other triangle values
		projectionLocation = glGetUniformLocation(tri_T.shaderProgram1, "uProjection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		modelLocation = glGetUniformLocation(tri_T.shaderProgram1, "uModel");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(translate*rotate*scale));
		viewLocation = glGetUniformLocation(tri_T.shaderProgram1, "uView");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		
		//draw the triangle with a shader and texture
		if(!playerDead)
			tri_T.render();

		//set to wireframe so we can see the circles
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				std::cout << CT << " CurrentTime"<< std::endl;
				std::cout << LT << "lastTime"<< std::endl;
		if (keyState[SDL_SCANCODE_S])
		{
			if (CT > LT + 7000)
			{


				translate[3].x = PRLx;
				translate[3].y = PRLy;
				LT = CT;
			}
		}


		if (keyState[SDL_SCANCODE_DOWN])
		{
			if (CT > LT + 7000)
			{
				translate[3].x = PRLx;
				translate[3].y = PRLy;
				LT = CT;
			}
		}
	

		//render
		currentTime = SDL_GetTicks();
		CT = SDL_GetTicks();
		///Timer before spawn
		if (currentTime > lastTime + 1500)
		{
			
			bubbleNum++;
			lastTime = currentTime;


		}


		///spawn bubble
		for ( int q =0; q < bubbleNum; q++)
		{
			circles[q]->updatePositions();

			glUseProgram(circles[q]->shaderProgram);
			//set projection matrix uniform
			projectionLocation = glGetUniformLocation(circles[q]->shaderProgram, "uProjection");
			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			modelLocation = glGetUniformLocation(circles[q]->shaderProgram, "uModel");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(circles[q]->circleTranslate*circles[q]->circleScale));
			viewLocation = glGetUniformLocation(circles[q]->shaderProgram, "uView");
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glBindVertexArray(circles[q]->VAO);
			circles[q]->render();
		}
		

		//for (int q = 0; q < 1; q++)
		//{
		//	circles[q] = updatePositions(circles[q]);

		//	glUseProgram(circles[q].shaderProgram);
		//	//set projection matrix uniform
		//	projectionLocation = glGetUniformLocation(circles[q].shaderProgram, "uProjection");
		//	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//	modelLocation = glGetUniformLocation(circles[q].shaderProgram, "uModel");
		//	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(circleTranslate*circleScale));
		//	viewLocation = glGetUniformLocation(circles[q].shaderProgram, "uView");
		//	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));


		//	glBindVertexArray(circles[q].VAO);
		//	circles[q].render();
		//}

		SDL_GL_SwapWindow(sdl.win);

	}//end loop

	//****************************
	// Once finished with OpenGL functions, the SDL_GLContext can be deleted.
	SDL_GL_DeleteContext(context);

	SDL_Quit();
	return 0;
}




//CircleTexture updatePositions(CircleTexture c)
//{
//
//	//check collison and update position for asteroids
//
//	std::cout << c.x << std::endl;
//
//	if (circleTranslate[3].x  >= bX_r || circleTranslate[3].x + radius < bX_l)
//	{

//		bubbleSpeedx *= -1.0f;
//		circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//		c.x += bubbleSpeedx;
//	}
//
//
//
//	if (circleTranslate[3].x - radius <= bX_r || circleTranslate[3].x + radius >= bX_l)
//	{
//		circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//		c.x += bubbleSpeedx;
//	}
//
//	if (circleTranslate[3].y + 0.1 >= bY_t || circleTranslate[3].y - 0.1 <= bY_b)
//	{
//		bubbleSpeedy *= -1.0f;
//		circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//		c.y += bubbleSpeedy;
//	}
//	if (circleTranslate[3].y + 0.1 <= bY_t || circleTranslate[3].y - 0.1 >= bY_b)
//	{
//		circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//		c.y += bubbleSpeedy;
//	}
//
//
//
//	//if (circleTranslate[3].x - radius >= bX_r || circleTranslate[3].x + radius < bX_l)
//	//{
//	//	bubbleSpeedx *= -1.0f;
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//	//	c.x += bubbleSpeedx;
//	//}
//	//if (circleTranslate[3].x - radius <= bX_r || circleTranslate[3].x + radius >= bX_l)
//	//{
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//	//	c.x += bubbleSpeedx;
//	//}
//
//	//if (circleTranslate[3].y + 0.1 >= bY_t || circleTranslate[3].y - 0.1 <= bY_b)
//	//{
//	//	bubbleSpeedy *= -1.0f;
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//	//	c.y += bubbleSpeedy;
//	//}
//	//if (circleTranslate[3].y + 0.1 <= bY_t || circleTranslate[3].y - 0.1 >= bY_b)
//	//{
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//	//	c.y += bubbleSpeedy;
//	//}
//
//
//
//
//
//
//
//
//
//	/*if (circleTranslate[3].x - 0.1 < bX_l)
//	{
//	bubbleSpeedx *= -1.0f;
//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//	c.x += bubbleSpeedx;
//	}*/
//	/*if (circleTranslate[3].x + radius >= bX_l)
//	{
//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, 0, 0.0f));
//	c.x += bubbleSpeedx;
//	}*/
//	/*if (circleTranslate[3].y + radius <= bY_b)
//	{
//		bubbleSpeedy *= -1.0f;
//		circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//		c.y += bubbleSpeedy;
//	}*/
//
//	
//	////update positions of the bubbles x
//	//if (c.x + (radius/2) < bX_r&(bubbleSpeedx > 0))
//	//{
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx,0, 0.0f));
//	//	c.x += bubbleSpeedx;													 
//	//}																			 
//	//if (c.x + (radius / 2) < bX_r&(bubbleSpeedx > 0))
//	//{																			 
//	//	bubbleSpeedx *= -1.0f;													 
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx,0, 0.0f));
//	//	c.x += bubbleSpeedx;													 
//	//																			 
//	//}																			 
//	//if (c.x + (radius / 2) < bX_r & c.x - (radius / 2)> bX_l &(bubbleSpeedx < 0))
//	//{																			 
//	//																			 
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx,0, 0.0f));
//	//	c.x += bubbleSpeedx;													
//	//																			
//	//}																			
//	//if (c.x - (radius / 2) < bX_l &(bubbleSpeedx < 0))
//	//{																			
//	//	bubbleSpeedx *= -1.0f;													
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx,0, 0.0f));
//	//	c.x += bubbleSpeedx;													
//	//	//std::cout << c.x << std::endl;										
//	//}																			
//	//if (c.x + (radius / 2) < bX_r& c.x - (radius / 2) > bX_l & (bubbleSpeedx > 0))
//	//{																			
//	//																			
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx,0, 0.0f));
//	//	c.x += bubbleSpeedx;													 
//	//}
//
//	/*std::cout << std::to_string(c.y) << std::to_string(c.x) << std::endl;*/
//
//	/*if (!c.y + radius <= bY_t)
//	{
//		
//
//		circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//		c.y += bubbleSpeedy;
//
//	}*/
//
//	////update positions of the bubbles y
//	//if (c.y < (bY_t - (radius * 2))&(bubbleSpeedy > 0))
//	//{
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//	//	c.y += bubbleSpeedy;
//	//	return c;
//	//}
//	//if (translate[3].x + 0.02f >= bY_t)
//	//{
//	//	glm::translate(translate, glm::vec3((float)cos(angle)*-0.05f, (float)sin(angle)*-0.05f, 0.0f));
//	//}
//
//	//if (c.y + radius > bY_t & (bubbleSpeedy > 0))
//	//{
//	//	bubbleSpeedy *= -1.0f;
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//	//	c.y += bubbleSpeedy;
//
//	//}
//	//if (c.y + radius > bY_t& c.y - radius > bY_b & (bubbleSpeedy < 0))
//	//{
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//	//	c.y += bubbleSpeedy;
//
//	//}
//	//if (c.y - radius < bY_b &(bubbleSpeedy < 0))
//	//{
//	//	bubbleSpeedy *= -1.0f;
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//	//	c.y += bubbleSpeedy;
//	//	//std::cout << c.y << std::endl;
//
//	//}
//	//if (c.y + radius < bY_t &c.y - radius > bY_b & (bubbleSpeedy > 0))
//	//{
//	//	circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
//	//	c.y += bubbleSpeedy;
//
//	//}
//
//
//	return c;
//	
//}

void handleInput()
{
	//*****************************
		//SDL handled input
		//Any input to the program is done here

	
	while (SDL_PollEvent(&event))
	{




		if (event.type == SDL_QUIT)
		{
			windowOpen = false;
		}
		if (event.type == SDL_WINDOWEVENT)
		{
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				std::cout << "Window resized w:" << w << " h:" << h << std::endl;
				SDL_GetWindowSize(win, &w, &h);
				newwidth = h * aspect;
				left = (w - newwidth) / 2;
				glViewport(left, 0, newwidth, h);
				break;
			
			/*case SDL_WINDOW_FULLSCREEN;
				std::cout << "Window resized w:" << w << " h:" << h << std::endl;
				SDL_GetWindowSize(win, &w, &h);
				newwidth = h * aspect;
				left = (w - newwidth) / 2;
				glViewport(left, 0, newwidth, h);
				break;*/

			default:
				break;
			}
		}
	
		/*if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			
			case SDLK_UP:
				translate = glm::translate(translate, glm::vec3((float)cos(angle)*0.02f, (float)sin(angle)*0.02f, 0.0f));
				break;

			case SDLK_DOWN:
				translate = glm::translate(translate, glm::vec3((float)cos(angle)*-0.02f, (float)sin(angle)*-0.02f, 0.0f));
				break;

			case SDLK_a:
				angle += glm::radians(10.0f);
				rotate = glm::rotate(rotate,glm::radians(10.0f), glm::vec3(0, 0, 1));
				break;
			case SDLK_d:
				angle -= glm::radians(10.0f);
				rotate = glm::rotate(rotate, glm::radians(-10.0f) , glm::vec3(0, 0, 1));
				break;
			case SDLK_f:
				
				break;
			case SDLK_1:
				lightCol = glm::vec3(0.0f, 0.0f, 1.0f);
				break;
			case SDLK_2:
				lightCol = glm::vec3(0.0f, 1.0f, 0.0f);
				break;
			case SDLK_0:
				lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
				break;
			
			}
			
		}*/
		/*int sym = event.key.keysym.sym;
		switch (event.type)
		{
		

		case SDL_KEYDOWN:
			wu = (sym == SDLK_w);
			a = (sym == SDLK_a);
			s = (sym == SDLK_s);
			d = (sym == SDLK_d);
			up = (sym == SDLK_UP);
			lefto = (sym == SDLK_LEFT);
			down = (sym == SDLK_DOWN);
			right = (sym == SDLK_RIGHT);
			break;

			
		case SDL_KEYUP:

			if (sym == SDLK_w) wu = false;
			if (sym == SDLK_a) a = false;
			if (sym == SDLK_s) s = false;
			if (sym == SDLK_d) d = false;
			if (sym == SDLK_UP) up       = false;
			if (sym == SDLK_LEFT) lefto  = false;
			if (sym == SDLK_DOWN) down   = false;
			if (sym == SDLK_RIGHT) right = false;
			break;

		}*/
		
	}

	checkForKeyPresses();
	
}


void checkForKeyPresses()
{

	/*if (playerx < bX_l)
	{
		translate = glm::translate(translate, glm::vec3(0.3f,playery,0));
	}

	if (playerx > bX_r)
	{
		translate = glm::translate(translate, glm::vec3(3.7f, playery, 0));
	}

	if (playery > bY_t)
	{
		translate = glm::translate(translate, glm::vec3(playerx, 2.7f, 0));
	}
	if (playery < bY_b)
	{
		translate = glm::translate(translate, glm::vec3(playerx, 0.03f, 0));
	}*/

	/*float bX_r = 3.8f;
	float bX_l = 0.2f;
	float bY_t = 2.8f;
	float bY_b = 0.02f;
*/


	if (keyState[SDL_SCANCODE_W]) 
	{ 

		if (translate[3].x + 0.1f >= bX_r  /* || translate[3].x - 0.2f< bX_l || translate[3].y + 0.2f > bY_t || translate[3].y - 0.2f < bY_b  */)
		{
			translate[3].x = 3.5;
			translate[3].y = translate[3].y;
			/*glm::translate(translate, glm::vec3((float)cos(angle)*-0.05f, (float)sin(angle)*-0.05f, 0.0f));*/
			//glm::translate(translate, glm::vec3(0.3f, translate[3].y, 0.0f));
		}

		else if (translate[3].x -0.1f < bX_l)
		{
			translate[3].x = 0.5;
			translate[3].y = translate[3].y;
		}
		else if (translate[3].y +0.1f > bY_t)
		{
			translate[3].x = translate[3].x;
			translate[3].y = 2.5;
		}

		else if (translate[3].y - 0.1f < bY_b)
		{
			translate[3].x = translate[3].x;
			translate[3].y = 0.5;
		}
	
		else
		{
			translate = glm::translate(translate, glm::vec3((float)cos(angle)*0.02f, (float)sin(angle)*0.02f, 0.0f));
		}
	}
	if (keyState[SDL_SCANCODE_UP])
	{
		if (translate[3].x + 0.1f >= bX_r)
		{
			translate[3].x = 3.5;
			translate[3].y = translate[3].y;
		}

		else if (translate[3].x - 0.1f < bX_l)
		{
			translate[3].x = 0.5;
			translate[3].y = translate[3].y;
		}
		else if (translate[3].y + 0.1f > bY_t)
		{
			translate[3].x = translate[3].x;
			translate[3].y = 2.5;
		}

		else if (translate[3].y - 0.1f < bY_b)
		{
			translate[3].x = translate[3].x;
			translate[3].y = 0.5;
		}

		else
		{
			translate = glm::translate(translate, glm::vec3((float)cos(angle)*0.02f, (float)sin(angle)*0.02f, 0.0f));
		}
	}



	if (keyState[SDL_SCANCODE_A]) {
		angle += glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(10.0f), glm::vec3(0, 0, 1));
	}
	if (keyState[SDL_SCANCODE_LEFT]) {
		angle += glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(10.0f), glm::vec3(0, 0, 1));
	}
	if (keyState[SDL_SCANCODE_D]) {
		angle -= glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(-10.0f), glm::vec3(0, 0, 1));
	}
	if (keyState[SDL_SCANCODE_RIGHT]) {
		angle -= glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(-10.0f), glm::vec3(0, 0, 1));
	}

	/*if (keyState[SDL_SCANCODE_W] & keyState[SDL_SCANCODE_A]) {
		translate = glm::translate(translate, glm::vec3((float)cos(angle)*0.02f, (float)sin(angle)*0.02f, 0.0f));
		angle += glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(10.0f), glm::vec3(0, 0, 1));
	}

	if (keyState[SDL_SCANCODE_UP] & keyState[SDL_SCANCODE_LEFT]) {
		translate = glm::translate(translate, glm::vec3((float)cos(angle)*0.02f, (float)sin(angle)*0.02f, 0.0f));
		angle += glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(10.0f), glm::vec3(0, 0, 1));
	}

	if (keyState[SDL_SCANCODE_W] & keyState[SDL_SCANCODE_D]) {
		translate = glm::translate(translate, glm::vec3((float)cos(angle)*0.02f, (float)sin(angle)*0.02f, 0.0f));
		angle -= glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(-10.0f), glm::vec3(0, 0, 1));
	}

	if (keyState[SDL_SCANCODE_UP] & keyState[SDL_SCANCODE_RIGHT]) {
		translate = glm::translate(translate, glm::vec3((float)cos(angle)*0.02f, (float)sin(angle)*0.02f, 0.0f));
		angle -= glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(-10.0f), glm::vec3(0, 0, 1));
	}

	if (keyState[SDL_SCANCODE_S] & keyState[SDL_SCANCODE_A]) {
		translate = glm::translate(translate, glm::vec3((float)cos(angle)*-0.02f, (float)sin(angle)*-0.02f, 0.0f));
		angle += glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(10.0f), glm::vec3(0, 0, 1));
	}

	if (keyState[SDL_SCANCODE_DOWN] & keyState[SDL_SCANCODE_LEFT]) {
		translate = glm::translate(translate, glm::vec3((float)cos(angle)*-0.02f, (float)sin(angle)*-0.02f, 0.0f));
		angle += glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(10.0f), glm::vec3(0, 0, 1));
	}

	if (keyState[SDL_SCANCODE_S] & keyState[SDL_SCANCODE_D]) {
		translate = glm::translate(translate, glm::vec3((float)cos(angle)*-0.02f, (float)sin(angle)*-0.02f, 0.0f));
		angle -= glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(-10.0f), glm::vec3(0, 0, 1));
	}

	if (keyState[SDL_SCANCODE_DOWN] & keyState[SDL_SCANCODE_RIGHT]) {
		translate = glm::translate(translate, glm::vec3((float)cos(angle)*-0.02f, (float)sin(angle)*-0.02f, 0.0f));
		angle -= glm::radians(10.0f);
		rotate = glm::rotate(rotate, glm::radians(-10.0f), glm::vec3(0, 0, 1));
	}*/
}

#endif
#endif
#endif
#endif