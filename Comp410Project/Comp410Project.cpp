
#define GLM_ENABLE_EXPERIMENTAL
#include "Angel.h"
#include "ObjectHolder.h"

#include <glm.hpp>
#include <gtx/rotate_vector.hpp>

#include <ctime>
#include <sstream>
#include <string>
#include <fstream>
#include <memory>



typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

using std::cout;
using std::endl;

ObjectHolder objectHolder;

int ANIMATION_TIME_INTERVAL = 10;


WorldBounds* world;
Ball* ball;
Crosshair* crosshair;

GLuint vao[1];
GLuint buffer;
GLuint program;

GLint projectionLoc;//Location for projection matrix attribute of vshader.
GLint viewLoc;//Location for view matrix attribute of vshader.
GLint modelLoc;//Location for model matrix attribute of vshader.
GLint colorLoc;//Location for color.
mat4   perspectiveProjection;
mat4   orthogonalProjection;
mat4   view;
mat4   model;

enum cameraMode{ballCamera = 0,freeCamera = 1};
int cameraMode = ballCamera;

vec4 eye = vec4(0.0, 0.0, 0.0, 1.0f); //The position the camera stands on.
vec4 viewDirection = vec4(0.0f, 0.0f, -1.0f, 0.0f); // the direction vector that the camera looks towards.
vec4 up = vec4(0.0, 1.0, 0.0, 0.0); //The up vector of eye.
vec2 previousMousePosition = vec2(0, 0);

const double deltaAngle = 0.5;
const double maximumAngle = 80.0;

void loadBallFromObj();
void globalMoveForwardBackward(float amt);
void globalMoveLeftRight(float amt);
void updateViewMatrixAccordingToBall();

//working lookAt implementation
mat4 lookAtTest(vec4 eye, vec4 at, vec4 upDir) {

	vec4 forward = normalize(eye - at);
	vec4 left = normalize(cross(upDir, forward));
	vec4 up = cross(forward, left);
	mat4 mat = mat4(1.0f);

	mat[0][0] = left.x;
	mat[1][0] = left.y;
	mat[2][0] = left.z;
	mat[0][1] = up.x;
	mat[1][1] = up.y;
	mat[2][1] = up.z;
	mat[0][2] = forward.x;
	mat[1][2] = forward.y;
	mat[2][2] = forward.z;

	// set translation part
	mat[3][0] = -left.x * eye.x - left.y * eye.y - left.z * eye.z;
	mat[3][1] = -up.x * eye.x - up.y * eye.y - up.z * eye.z;
	mat[3][2] = -forward.x * eye.x - forward.y * eye.y - forward.z * eye.z;

	return mat;

}

//initialize game values.
void initGame() {
	//Create a ball.
	float ballRadius = 0.10f;
	ball = new Ball(ballRadius);
	loadBallFromObj();
	ball->scaleModel(ballRadius / 0.25f);
	ball->color = color4(0.5f, 0.5f, 0.5f, 1.0f);

	//Create the object holder.
	objectHolder = ObjectHolder();
	objectHolder.program = program;
	objectHolder.newTurnAnimation = true;

	//Create the world
	world = new WorldBounds();

	//Add world to the object holder.
	objectHolder.addObject(world);
	objectHolder.addCollidableGameObject(world);
	objectHolder.worldBounds = world;

	//Create and add crosshair to object holder.
	crosshair = new Crosshair();
	objectHolder.addObject(crosshair);

	objectHolder.addObject(ball);
	objectHolder.startingBall = ball;

	//Create the helper balls, initialize game values.
	objectHolder.generateHelperBalls(objectHolder.shootingVector * 2);
	globalMoveForwardBackward(+1.0f);
	objectHolder.addNewShootingBalls();
	objectHolder.generateSquaresOnNewTurn();
	objectHolder.setLightAsPoint(false);
	objectHolder.showHelperBalls(false);
	objectHolder.moveStartingBall(0.25, 0);
	objectHolder.moveStartingBall(-0.25, 0);
	updateViewMatrixAccordingToBall();


}
void
init()
{
	
	// Load shaders and use the resulting shader program
	program = InitShader("vshader32.glsl", "fshader32.glsl");
	glUseProgram(program);

	srand(time(NULL));

	model = mat4(1.0f);
	view = mat4(1.0f);

	// Kept globally as they will be referenced in other methods.
	projectionLoc = glGetUniformLocation(program, "projection");
	viewLoc = glGetUniformLocation(program, "view");
	modelLoc = glGetUniformLocation(program, "model");
	colorLoc = glGetUniformLocation(program, "ucolor");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	mat4 view = lookAtTest(eye, eye + viewDirection, up);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);


	point4 light_position(0.0, 0.0, -1.0, 1.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float  material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
		1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
		1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
		1, specular_product);

	glUniform4fv(glGetUniformLocation(program, "LightPosition"),
		1, light_position);

	glUniform1f(glGetUniformLocation(program, "Shininess"),
		material_shininess);

	glEnable(GL_LINE_SMOOTH);

	initGame();
}

//----------------------------------------------------------------------------

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw objects
	objectHolder.drawObjects(modelLoc, viewLoc, projectionLoc, colorLoc);

	
	glutSwapBuffers();
}

void updateViewMatrixAccordingToBall() {
	//Displace ball and update view matrix
	eye.x = objectHolder.startingBall->x;
	eye.y = objectHolder.startingBall->y;
	eye.z = objectHolder.startingBall->z;
	mat4 view = lookAtTest(eye, eye + viewDirection, up);
	objectHolder.view = view;
}

//The following two functions globalMoveForwardBackward(float amt) and globalMoveLeftRight(float amt) are referenced from Jamie King's OpenGL tutorial:
//https://www.youtube.com/watch?v=7oNLw9Bct1k&index=58&list=PLRwVmtr-pp06qT6ckboaOhnm9FxmzHpbY
void globalMoveForwardBackward(float amt) {
	eye += amt * -viewDirection;
	mat4 view = lookAtTest(eye, eye + viewDirection, up);
	objectHolder.view = view;

}

void globalMoveLeftRight(float amt) {
	eye += amt * cross(viewDirection, up);
	mat4 view = lookAtTest(eye, eye + viewDirection, up);
	objectHolder.view = view;

}


void special(int key, int x, int y) {
	
	//Shooting direction has changed, update it accordingly
	switch (key) {
	case GLUT_KEY_RIGHT:
		objectHolder.yRotationByDegrees += deltaAngle;
		if (objectHolder.yRotationByDegrees > maximumAngle)
			objectHolder.yRotationByDegrees = maximumAngle;
		break;
	case GLUT_KEY_LEFT:
		objectHolder.yRotationByDegrees -= deltaAngle;
		if (objectHolder.yRotationByDegrees < -maximumAngle)
			objectHolder.yRotationByDegrees = -maximumAngle;
		break;
	case GLUT_KEY_DOWN:
		objectHolder.xRotationByDegrees += deltaAngle;
		if (objectHolder.xRotationByDegrees > maximumAngle)
			objectHolder.xRotationByDegrees = maximumAngle;
		break;
	case GLUT_KEY_UP:
		objectHolder.xRotationByDegrees -= deltaAngle;
		if (objectHolder.xRotationByDegrees < -maximumAngle)
			objectHolder.xRotationByDegrees = -maximumAngle;
		break;
	}

	//create the new rotation matrix using angles.
	//update the shooting vector helper balls with the new rotation.
	mat4 rotationMatrix = transpose(Translate(-objectHolder.startingBall->x, -objectHolder.startingBall->y, -objectHolder.startingBall->z) * 
		RotateY(objectHolder.yRotationByDegrees) * RotateX(objectHolder.xRotationByDegrees)
		* Translate (objectHolder.startingBall->x,objectHolder.startingBall->y,objectHolder.startingBall->z));
	objectHolder.shootingVector = rotationMatrix * vec4(0.0f,0.0f,-0.5f,0.0f);
	objectHolder.rotateHelperBalls(-objectHolder.xRotationByDegrees,-objectHolder.yRotationByDegrees);
	
	if(cameraMode == ballCamera)
		updateViewMatrixAccordingToBall();
}


void
keyboard(unsigned char key, int x, int y)
{

	//Keyboard controls

	switch (key) {
	case 033:
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 's': case'S': //based on camera mode, update view matrix/move ball
		if (cameraMode == freeCamera) {
			globalMoveForwardBackward(0.25);
		}
		else if (cameraMode == ballCamera) {
			if (objectHolder.allowShoot) {
				objectHolder.moveStartingBall(0, -0.25);
				updateViewMatrixAccordingToBall();

			}		
		}
		break;
	case 'w': case 'W'://based on camera mode, update view matrix/move ball
		if (cameraMode == freeCamera) {
			globalMoveForwardBackward(-0.25);
		}
		else if (cameraMode == ballCamera) {
			if (objectHolder.allowShoot) {
				objectHolder.moveStartingBall(0, 0.25);
				updateViewMatrixAccordingToBall();

			}
		}
		break;
	case 'a': case 'A'://based on camera mode, update view matrix/move ball
		if (cameraMode == freeCamera) {
			globalMoveLeftRight(-0.25);
		}
		else if (cameraMode == ballCamera) {
			if (objectHolder.allowShoot) {
				objectHolder.moveStartingBall(-0.25, 0);
				updateViewMatrixAccordingToBall();

			}
		}
		break;
	case 'd': case 'D'://based on camera mode, update view matrix/move ball
		if (cameraMode == freeCamera) {
			globalMoveLeftRight(0.25);
		}
		else if (cameraMode == ballCamera) {
			if (objectHolder.allowShoot) {
				objectHolder.moveStartingBall(0.25, 0);
				updateViewMatrixAccordingToBall();

			}
		}
		break;
	case 'r': case 'R': //reset the game if it is over.
		if (objectHolder.gameOver) {
			cout << "Restarting game!" << endl;
			objectHolder.resetGame();
			initGame();
		}
		else {
			cout << "Game has not ended!" << endl;
		}
		break;
	case 32: //shoot the ball if able to.
		if (!objectHolder.gameOver && objectHolder.allowShoot) {
			ball->setVelocity(objectHolder.shootingVector.x, objectHolder.shootingVector.y, objectHolder.shootingVector.z);
			objectHolder.showHelperBalls(false);
		}
		break;
	case 'x': case 'X': //SWITCH CAMERA MODE
		if (cameraMode == ballCamera && objectHolder.allowShoot) {
			cameraMode = freeCamera;
			objectHolder.setLightAsPoint(true);
			objectHolder.showHelperBalls(true);
			objectHolder.shootingVector = vec4(0.0f, 0.0f, -0.5f, 0.0f);

		}
		else if(cameraMode == freeCamera && objectHolder.allowShoot) {
			cameraMode = ballCamera;
			updateViewMatrixAccordingToBall();
			objectHolder.setLightAsPoint(false);
			objectHolder.showHelperBalls(false);

		}
		break;

	}
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	if (w <= h)
		objectHolder.orthogonalProjection = Ortho(-1.0, 1.0, -1.0 * (GLfloat)h / (GLfloat)w,
			1.0 * (GLfloat)h / (GLfloat)w, -1.0, 1.0);
	else  objectHolder.orthogonalProjection = Ortho(-1.0* (GLfloat)w / (GLfloat)h, 1.0 *
		(GLfloat)w / (GLfloat)h, -1.0, 1.0, -1.0, 1.0);
	// Set projection matrix
	objectHolder.perspectiveProjection = Perspective(45.0f, (GLfloat)w / (GLfloat)h, 0.1f, 1000.0f);

}

void main_menu(int id) {


}

//The following function mouse_movement(int x,int y) is referenced from Jamie King's OpenGL tutorial:
//https://www.youtube.com/watch?v=7oNLw9Bct1k&index=58&list=PLRwVmtr-pp06qT6ckboaOhnm9FxmzHpbY

void mouse_movement(int x, int y) {
	//Based on the displacement of mouse, change the view direction of the camera.

	vec2 currentMousePosition = vec2(x, y);
	vec2 mouseDisplacement = currentMousePosition - previousMousePosition;
	if (length(mouseDisplacement) > 50.0f) {
		previousMousePosition = currentMousePosition;
		return;
	}

	glm::vec3 vector = glm::vec3(viewDirection.x, viewDirection.y, viewDirection.z);
	vector = glm::mat3(glm::rotate(-mouseDisplacement.x * (GLfloat)0.006, glm::vec3(up.x, up.y, up.z))) * vector;

	glm::vec3 rotateAround = glm::cross(vector, glm::vec3(up.x, up.y, up.z));
	vector = glm::mat3(glm::rotate(-mouseDisplacement.y * (GLfloat)0.006, rotateAround)) * vector;
	viewDirection = vec4(vector.x, vector.y, vector.z, 0.0f);
	previousMousePosition = currentMousePosition;

	if (cameraMode == ballCamera) {
		objectHolder.shootingVector = viewDirection * 0.25 / length(viewDirection);
		if (viewDirection.z > 0)
			viewDirection.z = 0;
	}
	
	
	mat4 view = lookAtTest(eye, eye + viewDirection, up);
	objectHolder.view = view;
}


void animationTimer(int arguement) {
	glutPostRedisplay();
	objectHolder.update();
	glutTimerFunc(ANIMATION_TIME_INTERVAL, animationTimer, 1);

}


int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Cube");

	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMotionFunc(mouse_movement);
	glutReshapeFunc(reshape);
	glutTimerFunc(ANIMATION_TIME_INTERVAL, animationTimer, 1);


	glutMainLoop();
	return 0;
}

void loadBallFromObj() {
	std::ifstream infile;
	infile.open("ball.obj");
	std::string line;
	int lineIndex = 1;
	int vertexIndexCounter = 0;
	int indexIndexCounter = 0;
	const int numberOfVertices = 642;
	const int numberOfTriangles = 1280;
	point4 vertices[numberOfVertices];
	vec3 normals[numberOfVertices];
	int indices[numberOfTriangles * 3];

	//Read data off the file

	while (std::getline(infile, line)) {
		std::istringstream stream(line);

		if (lineIndex >= 12 && lineIndex <= 1295) {
			std::string dummy;
			float x, y, z;
			stream >> dummy >> x >> y >> z;
			if (dummy == "vn") {
				normals[vertexIndexCounter] = vec3(x, y, z);
			}
			else if (dummy == "v") {
				vertices[vertexIndexCounter++] = point4(x, y, z,1.0f);
			}
		}
		else if (lineIndex >= 1298 && lineIndex <= 2577) {
			std::string dummy, first, second, third;
			stream >> dummy >> first >> second >> third;
			indices[indexIndexCounter++] = stoi(first.substr(0, first.find('/')));
			indices[indexIndexCounter++] = stoi(second.substr(0, second.find('/')));
			indices[indexIndexCounter++] = stoi(third.substr(0, third.find('/')));
		}
		lineIndex++;
	}

	for (int i = 0; i < numberOfTriangles * 3; i++) {
		ball->vertices.push_back(vertices[indices[i] - 1]);
		ball->normals.push_back(normals[indices[i] - 1]);
	}
	infile.close();
}