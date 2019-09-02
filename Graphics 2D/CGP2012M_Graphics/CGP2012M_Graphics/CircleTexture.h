#pragma once
#include <GL/glew.h>
#include "TextureClass.h"
#include <array>
#include <cmath>
// // - OpenGL Mathematics - https://glm.g-truc.net/
#define GLM_FORCE_RADIANS // force glm to use radians
// // NOTE: must do before including GLM headers
// // NOTE: GLSL uses radians, so will do the same, for consistency
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class CircleTexture
{
public:

	float centreX = 0.9f;
	float centreY = 0.9f;
	float bX_r = 4.0f - 0.01f;
	float bX_l = 0.0f + 0.01f;
	float bY_t = 2.25f - 0.01f;
	float bY_b = 0.75f + 0.01f;
	float bubbleSpeedx;
	float bubbleSpeedy;
	float radius;
	glm::mat4 circleTranslate;
	glm::mat4 circleScale;

	//set up vertex buffer object
	GLuint VBO;
	//set up vertex array object
	GLuint VAO;
	//set up index buffer object
	GLuint EBO;
	//set up texture
	Texture tex;
	//shaders
	Shader vSh, fSh;
	GLuint shaderProgram;
	//position
	float x=0, y=0;

	float speed = 0.01f;

	//set up vertex array
	GLfloat vertices[240];
	//set up index array
	GLuint indices[87] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		0, 7, 8,
		0, 8, 9,
		0, 9, 10,
		0, 10, 11,
		0, 11, 12,
		0, 12, 13,
		0, 13, 14,
		0, 14, 15,
		0, 15, 16,
		0, 16, 17,
		0, 17, 18,
		0, 18, 19,
		0, 19, 20,
		0, 20, 21,
		0, 21, 22,
		0, 22, 23,
		0, 23, 24,
		0, 24, 25,
		0, 25, 26,
		0, 26, 27,
		0, 27, 28,
		0, 28, 29
	};

	float rand_float(float max)
	{
		return ((rand() / (float)RAND_MAX) - 0.5) * max;
	}

	CircleTexture(float radius, float offsetX, float offsetY)
	{

		this->radius = radius;

		float speed = 0.01;

		bubbleSpeedx = rand_float(speed);
		bubbleSpeedy = rand_float(speed);

		circleTranslate = glm::mat4(1.0f);
		circleScale = glm::mat4(1.0f);
		circleScale = glm::scale(circleScale, glm::vec3(1.0f, 1.0f, 0.0f));

		float randValue = rand_float(1);
		float randValue2 = rand_float(1);

		x = centreX + randValue;
		y = centreY + randValue2;
		circleTranslate = glm::translate(circleTranslate, glm::vec3(centreX + randValue, centreY + randValue2, 0.0f));

		offsetX = offsetY = 0;

		//origin of circle at 0,0,0
		vertices[0] = 0.0f + offsetX;
		vertices[1] = 0.0f + offsetY;
		vertices[2] = 0.0f;
		//colour of origin vertex
		vertices[3] = 0.0f;
		vertices[4] = 0.0f;
		vertices[5] = 0.1f;
		//texture coordinates of centre
		vertices[6] = 0.5f;
		vertices[7] = 0.5f;

		GLfloat angle = 0.0f;

		//set remaining vertices based on radius
		for (int i = 8; i < 240; i += 8)
		{
			vertices[i] = (radius * cos(angle)) + offsetX;
			vertices[i + 1] = (radius * sin(angle)) + offsetY;
			vertices[i + 2] = 0.0f;
			//colour information
			vertices[i + 3] = 0.8f;
			vertices[i + 4] = 0.0f;
			vertices[i + 5] = 0.4f;
			//texture coordinate information
			vertices[i + 6] = ((radius * cos(angle) )*2.5f)+0.5f;
			vertices[i + 7] = ((radius * sin(angle) )*2.5f)+0.5f;
		
			//increase angle value in radians
			//(2*pi)/number of verts on circumference
			angle += (2*3.141)/28.0f;

		}

		//shaders
		vSh.shaderFileName("..//..//Assets//Shaders//shader_vColour_Projection.vert");
		fSh.shaderFileName("..//..//Assets//Shaders//shader_vColour_Projection.frag");

		vSh.getShader(1);
		fSh.getShader(2);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vSh.shaderID);
		glAttachShader(shaderProgram, fSh.shaderID);
		glLinkProgram(shaderProgram);

		glDeleteShader(vSh.shaderID);
		glDeleteShader(fSh.shaderID);

		//load the texture file
		//pic from http://pixelartmaker.com/art/b0d28fcc8a7f281
		//tex.load("..//..//Assets//Textures//pixelbubble.png");
		
		//if(tex.tex == nullptr)
		tex.load("..//..//Assets//Textures//PixelBubble.png");


	


	}

	void setBuffers()
	{
		//
		//OpenGL buffers
		//set up 1 for the triangle
		glGenBuffers(1, &VBO);
		// Initialization code using Vertex Array Object (VAO) (done once (unless the object frequently changes))
		glGenVertexArrays(1, &VAO);
		//initialise the index buffer
		glGenBuffers(1, &EBO);
		
		// Bind Vertex Array Object
		glBindVertexArray(VAO);
		// Copy our vertices array in a buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		//set up the EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// Then set our vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//colour information
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		//texture information
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		//Unbind the VAO
		glBindVertexArray(0);

		//texture buffers
		tex.setBuffers();
	}

	void render()
	{
		//draw the circle 
		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_2D, tex.texture);
		glPointSize(5.0f);
		glDrawElements(GL_TRIANGLES, 87, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}


	void updatePositions()	
	{
		float r = radius * 2;

		x += bubbleSpeedx;
		y += bubbleSpeedy;
		circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));

		if (x > (bX_r - r) || x < (bX_l + r))
			bubbleSpeedx *= -1.0f;

		if (y > (bY_t + r) || y < (bY_b - r))
			bubbleSpeedy *= -1.0f;


		/*if ((x - r) >= bX_r || (x + r) < bX_l)
		{
			bubbleSpeedx *= -1.0f;
			circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
			x += bubbleSpeedx;
		}
		else
		{
			circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
			x += bubbleSpeedx;
		}

		if ((y - r) >= bY_t || (y + r) <= bY_b)
		{
			bubbleSpeedy *= -1.0f;
			circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
			y += bubbleSpeedy;
		}
		else
		{
			circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
			y += bubbleSpeedy;
		}*/

		/*if (x >= bX_r || x + radius < bX_l)
		{
			bubbleSpeedx *= -1.0f;
			circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
			x += bubbleSpeedx;
		}



		if (x - radius <= bX_r || x + radius >= bX_l)
		{
			circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
			x += bubbleSpeedx;
		}

		if (y + 0.1 >= bY_t || y - 0.1 <= bY_b)
		{
			bubbleSpeedy *= -1.0f;
			circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
			y += bubbleSpeedy;
		}
		if (y + 0.1 <= bY_t || y - 0.1 >= bY_b)
		{
			circleTranslate = glm::translate(circleTranslate, glm::vec3(bubbleSpeedx, bubbleSpeedy, 0.0f));
			y += bubbleSpeedy;
		}*/

	
	
	
	}



};
