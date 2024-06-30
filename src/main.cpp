
/*
NOTES:

USE TRIANGLES INSTEAD OF POINTS TO DO TRANSFORMATIONS AND ROTATIONS

PROJECTION IS WARPED 

PROJECTION MULT MAY BE REVERSED SINCE VIDEO USES ROWS INSTEAD OF COLS
*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <iostream>
#include <vector>
#define SCREEN_WIDTH 800.0f
#define SCREEN_HEIGHT 800.0f
#define PI 3.1415926
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

//SHADERS COPY PASTED

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 0.02f, 1.0f);\n"
"}\n\0";


	struct matrix4x4
	{
		GLfloat matrix[4][4] = {0};
	};
	struct point
	{
		GLfloat x, y, z;
		void rotateX(GLfloat theta){
			float thetaRad = theta / 180.0f * PI;
			const GLfloat oldy = y;
			y = (cosf(thetaRad) * oldy) - (sinf(thetaRad) * z);
			z = (sinf(thetaRad) * oldy) + (cosf(thetaRad) * z);
		}
		void rotateY(GLfloat theta){
			float thetaRad = theta / 180.0f * PI;
			const GLfloat oldx = x;
			x = cosf(thetaRad) * oldx + sinf(thetaRad) * z;
			z = -sinf(thetaRad) * oldx + cosf(thetaRad) * z;
		}
		void rotateZ(GLfloat theta){
			//flip
			float thetaRad = theta / 180.0f * PI;
			const GLfloat oldx = x;
			x = cosf(thetaRad) * oldx - sinf(thetaRad) * y;
			y = sinf(thetaRad) * oldx + cosf(thetaRad) * y;
			//x = cosf(thetaRad) * x + sinf(thetaRad) * y;
			//y = -sinf(thetaRad) * x + cosf(thetaRad) * y;
			
			/*
			FOR 45 DEG
			x = cos(45) * x - sin(45) * y
			y = sin(45) * x + cos(45) * y
			
			[-0.5, -0.5] = (0, -√2/2)
			[-0.5, 0.5] = (-√2/2, 0)
			[0.5, 0.5] = (0, √2/2)
			[0.5, -0.5] = (√2/2, 0)
			*/
		}
		void projectPoint(matrix4x4 proj){
			GLfloat w = proj.matrix[0][3] * x + proj.matrix[1][3] * y + proj.matrix[2][3] * z + proj.matrix[3][3];
			x = proj.matrix[0][0] * x + proj.matrix[1][0] * y + proj.matrix[2][0] * z + proj.matrix[3][0];
			y = proj.matrix[0][1] * x + proj.matrix[1][1] * y + proj.matrix[2][1] * z + proj.matrix[3][1];
			z = proj.matrix[0][2] * x + proj.matrix[1][2] * y + proj.matrix[2][2] * z + proj.matrix[3][2];
			if(w != 0) {
				x /= w;
				y /= w;
				z /= w;
			}
		}
	};
	struct triangle
	{
		point points[3];
	};
	struct polygon
	{
		std::vector<triangle> triangles;
		//std::vector<point> pointsArray;
		//std::vector<point> rotatedPoints;
		//std::vector<point> projectedPoints;
		//Num points shouldn't be preset to 8, but will be set on initialization
		point pointsArray[8];
		point rotatedPoints[8];
		point projectedPoints[8];
		int numPoints;
		float rotation[3] = {0, 0, 0};
		polygon(){
			numPoints = 8;
		}
		polygon(int numPoints){
			this->numPoints = numPoints;	
			
		}
		void definePoints(point points[]){
			for(int i = 0; i < numPoints; i++){
				pointsArray[i] = points[i];
				rotatedPoints[i] = pointsArray[i];
			}
		}
		
		void rotatePoints(GLfloat xRot, GLfloat yRot, GLfloat zRot){
			
			for(int i = 0; i < numPoints; i++){
				rotatedPoints[i] = pointsArray[i];
				rotatedPoints[i].rotateX(rotation[0] + xRot);
				rotatedPoints[i].rotateY(rotation[1] + yRot);
				rotatedPoints[i].rotateZ(rotation[2] + zRot);
			}
			rotation[0] += xRot;
			rotation[1] += yRot;
			rotation[2] += zRot;
			for(int i = 0; i < 3; i++){
				while(rotation[i] >= 360.0f){
					rotation[i] -= 360.0f;
				}
				while(rotation[i] < 0.0f){
					rotation[i] += 360.0f;
				}
			}
			
		/*
		GLfloat xRad = (xRot + rotation[0]) / 180.0f * PI;
		GLfloat yRad = (yRot + rotation[1]) / 180.0f * PI;
		GLfloat zRad = (zRot + rotation[2]) / 180.0f * PI;
			for(int i = 0; i < numPoints; i++){
				rotatedPoints[i] = pointsArray[i];
				rotatedPoints[i].x = cosf(yRad) * cosf(zRad) * rotatedPoints[i].x + (sinf(xRad) * sinf(yRad) * cosf(zRad) - cosf(xRad) * sinf(zRad)) * rotatedPoints[i].y + (cosf(xRad) * sinf(yRad) * cosf(zRad) + sinf(xRad) * sinf(zRad)) * rotatedPoints[i].z;
				rotatedPoints[i].y = cosf(yRad) * sinf(zRad) * rotatedPoints[i].x + (sinf(xRad) * sinf(yRad) * sinf(zRad) + cosf(xRad) * cosf(zRad)) * rotatedPoints[i].y + (cosf(xRad) * sinf(yRad) * sinf(zRad) - sinf(xRad) * cosf(zRad)) * rotatedPoints[i].z;
				rotatedPoints[i].z = -sinf(yRad) * rotatedPoints[i].x + sinf(xRad) * cosf(yRad) * rotatedPoints[i].y + cosf(xRad) * cosf(yRad) * rotatedPoints[i].z;
			}
			rotation[0] += xRot;
			rotation[1] += yRot;
			rotation[2] += zRot;
			for(int i = 0; i < 3; i++){
				while(rotation[i] >= 360.0f){
					rotation[i] -= 360.0f;
				}
			}
		*/
		
			
			
		}
		void translatePoints(GLfloat dx, GLfloat dy, GLfloat dz){
			for(int i = 0; i < numPoints; i++){
				pointsArray[i].x += dx;
				pointsArray[i].y += dy;
				pointsArray[i].z += dz;
				rotatedPoints[i].x += dx;
				rotatedPoints[i].y += dy;
				rotatedPoints[i].z += dz;
			}
		}
		void projectPoints(matrix4x4 proj){
			for(int i = 0; i < numPoints; i++){
				projectedPoints[i] = rotatedPoints[i];
				projectedPoints[i].projectPoint(proj);
			}
		}
		void scaleToScreen(){
			//point * screen width / 2

			
		}
		void defineTriangles(){
			triangles.clear();
			triangles = {
			
			/*
			//SOUTH TOWARDS PERSPECTIVE

			//SOUTH
			{pointsArray[0], pointsArray[1], pointsArray[2]},
			{pointsArray[0], pointsArray[2], pointsArray[3]},
			//EAST
			{pointsArray[3], pointsArray[2], pointsArray[6]},
			{pointsArray[3], pointsArray[6], pointsArray[7]},
			//NORTH
			{pointsArray[7], pointsArray[6], pointsArray[5]},
			{pointsArray[7], pointsArray[5], pointsArray[4]},
			//WEST
			{pointsArray[4], pointsArray[5], pointsArray[1]},
			{pointsArray[4], pointsArray[1], pointsArray[0]},
			//TOP
			{pointsArray[1], pointsArray[5], pointsArray[6]},
			{pointsArray[1], pointsArray[6], pointsArray[2]},
			//BOTTOM
			{pointsArray[4], pointsArray[0], pointsArray[3]},
			{pointsArray[4], pointsArray[3], pointsArray[7]}
			*/
			
			{projectedPoints[0], projectedPoints[1], projectedPoints[2]},
			{projectedPoints[0], projectedPoints[2], projectedPoints[3]},
			
			{projectedPoints[3], projectedPoints[2], projectedPoints[6]},
			{projectedPoints[3], projectedPoints[6], projectedPoints[7]},
			
			{projectedPoints[7], projectedPoints[6], projectedPoints[5]},
			{projectedPoints[7], projectedPoints[5], projectedPoints[4]},
			
			{projectedPoints[4], projectedPoints[5], projectedPoints[1]},
			{projectedPoints[4], projectedPoints[1], projectedPoints[0]},
			
			{projectedPoints[1], projectedPoints[5], projectedPoints[6]},
			{projectedPoints[1], projectedPoints[6], projectedPoints[2]},
			
			{projectedPoints[4], projectedPoints[0], projectedPoints[3]},
			{projectedPoints[4], projectedPoints[3], projectedPoints[7]}
			

			};
			
		}
	};


	void processInput(GLFWwindow *window)
	{
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		
	}

	/*
	matrix4x4 multMatrix4x4(matrix4x4 A, matrix4x4 B){
		matrix4x4 result;
		int sum = 0;
		for(int i = 0; i < 4; i++){
			for(int j = 0; j < 4; j++){
				for(int k = 0; k < 4; k++){
					sum = sum + A.matrix[j][k] * B.matrix[k][i];
				}
				result.matrix[i][j] = sum;
				sum = 0;
			}
		}
		
		return result;
	}
	*/


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C++ 3D RayTracer", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
 
    glfwMakeContextCurrent(window);

    gladLoadGL();

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//DEFINE VARIABLES FOR PROJECTION MATRIX
	matrix4x4 projection;
	GLfloat aspect = SCREEN_HEIGHT / SCREEN_WIDTH;
	GLfloat fov = 90;
	GLfloat fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * PI);
	GLfloat zfar = 100.0f;
	GLfloat znear = 0.1f;
	GLfloat q = zfar/(zfar-znear);
	polygon cube;
	int transVal = 2.0f;

	projection.matrix[0][0] = aspect * fovRad;
	projection.matrix[1][1] = fovRad;
	projection.matrix[2][2] = zfar/(zfar-znear);
	projection.matrix[3][2] = (-zfar * znear)/(zfar-znear);
	
	projection.matrix[2][3] = 1.0f;
	
	point points[] = 
	{
		
		{-0.5f, -0.5f, 0.0f},
		{-0.5f, 0.5f, 0.0f},
		{0.5f, 0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{-0.5f, -0.5f, 1.0f},
		{-0.5f, 0.5f, 1.0f},
		{0.5f, 0.5f, 1.0f},
		{0.5f, -0.5f, 1.0f}
		
		
		/*
		{0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 1.0f},
		*/
		
	};

	cube.definePoints(points);
	//cube.rotatePoints(0.0f, 0.0f, 20.0f);
	cube.translatePoints(0.0f, 0.0f, transVal);
	
	cube.projectPoints(projection);
	cube.scaleToScreen();
	cube.defineTriangles();
	/*
	std::cout << "\nPrinting Projected Points \n\n";
	for(int i = 0; i < 8; i++){
		std::cout << "x: " << cube.projectedPoints[i].x << " y: " << cube.projectedPoints[i].y << " z: " << cube.projectedPoints[i].z << "\n";
	}
	*/

	//triangle meshArray[cube.triangles.size()];
	triangle meshArray[12];  
	
	copy(cube.triangles.begin(), cube.triangles.end(), meshArray);
	// Create reference containers for the Vartex Array Object and the Vertex Buffer Object
	GLuint VAO, VBO;

	// Generate the VAO and VBO with only 1 object each
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Make the VAO the current Vertex Array Object by binding it
	glBindVertexArray(VAO);

	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Introduce the vertices into the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(meshArray), meshArray, GL_STATIC_DRAW);

	// Configure the Vertex Attribute so that OpenGL knows how to read the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Enable the Vertex Attribute so that OpenGL knows to use it
	glEnableVertexAttribArray(0);

	// Bind both the VBO and VAO to 0 so that we don't accidentally modify the VAO and VBO we created
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glMatrixMode(GL_PROJECTION);
	// Variables to be changed in the ImGUI window
	float size = 1.0f;
	float color[4] = { 0.8f, 0.3f, 0.02f, 1.0f };

	// Exporting variables to shaders
	glUseProgram(shaderProgram);
	glUniform1f(glGetUniformLocation(shaderProgram, "size"), size);
	glUniform4f(glGetUniformLocation(shaderProgram, "color"), color[0], color[1], color[2], color[3]);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		//TAKE THIS OUTSIDE IF STATEMENTS AND PUT AT BOTTOM
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		// Export variables to shader
		glUseProgram(shaderProgram);
		glUniform1f(glGetUniformLocation(shaderProgram, "size"), size);
		glUniform4f(glGetUniformLocation(shaderProgram, "color"), color[0], color[1], color[2], color[3]);
		
		/*
		std::cout << "\nPrinting Projected Points \n\n";
			for(int i = 0; i < 8; i++){
				std::cout << "x: " << cube.projectedPoints[i].x << " y: " << cube.projectedPoints[i].y << " z: " << cube.projectedPoints[i].z << "\n";
			}
		*/
		
		//Find way to convert to switch case
		if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
			cube.translatePoints(0.0f, 0.0f, -transVal);
			cube.rotatePoints(0.0f, -1.0f, 0.0f);
			cube.translatePoints(0.0f, 0.0f, transVal);
				
		}
		else if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
			cube.translatePoints(0.0f, 0.0f, -transVal);
			cube.rotatePoints(0.0f, 1.0f, 0.0f);
			cube.translatePoints(0.0f, 0.0f, transVal);
		}
		if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
			cube.translatePoints(0.0f, 0.0f, -transVal);
			cube.rotatePoints(1.0f, 0.0f, 0.0f);
			cube.translatePoints(0.0f, 0.0f, transVal);
		}
		else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
			cube.translatePoints(0.0f, 0.0f, -transVal);
			cube.rotatePoints(-1.0f, 0.0f, 0.0f);
			cube.translatePoints(0.0f, 0.0f, transVal);
		}
		if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
			cube.translatePoints(0.0f, 0.0f, -transVal);
			cube.rotatePoints(0.0f, 0.0f, -1.0f);
			cube.translatePoints(0.0f, 0.0f, transVal);	
		}	
		else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
			cube.translatePoints(0.0f, 0.0f, -transVal);
			cube.rotatePoints(0.0f, 0.0f, 1.0f);
			cube.translatePoints(0.0f, 0.0f, transVal);
		}

		cube.projectPoints(projection);
		cube.defineTriangles();
		copy(cube.triangles.begin(), cube.triangles.end(), meshArray);
		glClearColor(0.07f, 0.13f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(meshArray), meshArray, GL_STATIC_DRAW);
		for(int i = 0; i < sizeof(meshArray); i+=3){
			glDrawArrays(GL_LINE_LOOP, i, 3);
		}

		processInput(window);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
