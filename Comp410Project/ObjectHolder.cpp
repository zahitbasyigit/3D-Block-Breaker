#include "ObjectHolder.h"
#include <string>
#include <time.h>
#include <stdlib.h>
#include <memory>
#include <algorithm> 
#include <random>
#include <chrono>


unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine e(seed);

ObjectHolder::ObjectHolder()
{
	srand(time(NULL));
	ballCount = WorldBounds::worldSizeByCubesY;
	currentTurnBallCount = ballCount;
	currentTurnCount = 0;
}


ObjectHolder::~ObjectHolder()
{
}

void ObjectHolder::addObject(Object* object)
{
	//add the object to vector for drawing later
	//set the objects VAO and buffer its VBO with vertices and normals.

	objects.push_back(object);
	object->setVectorIndex(objects.size() - 1);

	vector<point4> vertices = object->getVertices();
	vector<vec3> normals = object->getNormals();

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	GLuint vNormal = glGetAttribLocation(program, "vNormal");

	glGenVertexArrays(1, &object->vao);

	// Bind to Cube buffer
	glBindVertexArray(object->vao);

	// Cube buffer
	glGenBuffers(1, &object->buffer);
	glBindBuffer(GL_ARRAY_BUFFER, object->buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(point4) + normals.size() * sizeof(vec3),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(point4), &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(point4), normals.size() * sizeof(vec3), &normals[0]);

	// Cube vertex arrays
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(vertices.size() * sizeof(point4)));
}

void ObjectHolder::addCollidableGameObject(GameObject* gameObject) {
	collidableGameObjects.push_back(gameObject);
	gameObject->collidableVectorIndex = collidableGameObjects.size() - 1;
}

void ObjectHolder::addPowerUpSphere(Ball * ball)
{
	powerUpSpheres.push_back(ball);
	ball->powerUpIndex = powerUpSpheres.size() - 1;
}


void ObjectHolder::removeObject(Object* object, bool deletePointer)
{

	//delete the object from the vector and free its memory.

	if (object->getVectorIndex() < 0) {
		cout << "Invalid attempt at deleting object with index " << object->getVectorIndex() << endl;
	}
	else {
		//Fix vector indices of next elements
		for (int i = object->getVectorIndex() + 1; i < objects.size(); i++) {
			Object* o = objects.at(i);
			o->setVectorIndex(o->getVectorIndex() - 1);
		}

		objects.erase(objects.begin() + object->getVectorIndex());

		//TODO
		//delete object  ? 

		if (deletePointer) {
			cout << "Deleting pointer." << endl;
			delete object;
			object = NULL;
		}
	}

}

void ObjectHolder::removeCollidableGameObject(GameObject* object, bool deletePointer) {
		if (object->collidableVectorIndex < 0) {
		cout << "Invalid attempt at deleting collidable object with index " << object->collidableVectorIndex << endl;
	}
	else {
		//Fix vector indices of next elements
		for (int i = object->collidableVectorIndex + 1; i < collidableGameObjects.size(); i++) {
			GameObject* o = collidableGameObjects.at(i);
			o->collidableVectorIndex = o->collidableVectorIndex - 1;
		}

		collidableGameObjects.erase(collidableGameObjects.begin() + object->collidableVectorIndex);

		if (deletePointer) {
			cout << "Deleting pointer." << endl;
			delete object;
			object = NULL;
		}
	}
}

void ObjectHolder::removePowerUpShere(Ball * object, bool deletePointer)
{	if (object->powerUpIndex < 0) {
		cout << "Invalid attempt at deleting sphere with index " << object->powerUpIndex << endl;
	}
	else {
		//Fix vector indices of next elements
		for (int i = object->powerUpIndex + 1; i < powerUpSpheres.size(); i++) {
			Ball* o = powerUpSpheres.at(i);
			o->powerUpIndex = o->powerUpIndex - 1;
		}

		powerUpSpheres.erase(powerUpSpheres.begin() + object->powerUpIndex);

		if (deletePointer) {
			cout << "Deleting pointer." << endl;
			delete object;
			object = NULL;
		}
	}
}
 
void ObjectHolder::drawObjects(GLint modelLoc, GLint viewLoc, GLint projectionLoc, GLint colorLoc)
{

	//Draw each object with different view/projection matrices depending on the type (UI or game object).
	for (int i = 0; i < objects.size(); i++) {
		Object* o = objects.at(i);
		mat4 projection = o->perspectiveProjection ? perspectiveProjection : orthogonalProjection;
		glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection);

		if (!o->perspectiveProjection) {
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, mat4(1.0f));
		}
		else {
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);
		}

		o->draw(modelLoc, colorLoc);
	}
}

void ObjectHolder::moveBall(Ball* ball)
{

	//ball's x,y,z values are taken both before and after moving to detect collision 
	//if the bounds are between previous and next values then there exists a collision.
	
	float previousX = ball->x;
	float previousY = ball->y;
	float previousZ = ball->z;

	ball->move();
	
	float currentX = ball->x;
	float currentY = ball->y;
	float currentZ = ball->z;

	for (int i = collidableGameObjects.size() - 1; i >= 0; i--) {
		GameObject* gameObject = collidableGameObjects.at(i);
		if (gameObject->collisionType == gameObject->cubeCollision) { //CUBES TO DESTROY
			bool collision = false;

			if (previousX + ball->radius <= gameObject->xCollisionLeftBound && currentX + ball->radius >= gameObject->xCollisionLeftBound && 
				currentY >= gameObject->yCollisionBottomBound - ball->radius && currentY <= gameObject->yCollisionTopBound + ball->radius &&
				currentZ >= gameObject->zCollisionFrontBound - ball->radius && currentZ <= gameObject->zCollisionBackBound + ball->radius)
			{
				collision = true;
				ball->dx = -abs(ball->dx);
				cout << "Left collision occured." << endl;
			}

			if (previousX - ball->radius >= gameObject->xCollisionRightBound && currentX - ball->radius <= gameObject->xCollisionRightBound &&
				currentY >= gameObject->yCollisionBottomBound - ball->radius && currentY <= gameObject->yCollisionTopBound + ball->radius &&
				currentZ >= gameObject->zCollisionFrontBound - ball->radius && currentZ <= gameObject->zCollisionBackBound + ball->radius)
			{
				collision = true;
				ball->dx = abs(ball->dx);
				cout << "Right collision occured." << endl;
			}

			if (previousY + ball->radius <= gameObject->yCollisionBottomBound && currentY + ball->radius >= gameObject->yCollisionBottomBound &&
				currentX >= gameObject->xCollisionLeftBound - ball->radius && currentX <= gameObject->xCollisionRightBound + ball->radius &&
				currentZ >= gameObject->zCollisionFrontBound - ball->radius && currentZ <= gameObject->zCollisionBackBound + ball->radius)
			{
				collision = true;
				ball->dy = -abs(ball->dy);
				cout << "Bottom collision occured." << endl;
			}

			if (previousY - ball->radius >= gameObject->yCollisionTopBound && currentY - ball->radius <= gameObject->yCollisionTopBound &&
				currentX >= gameObject->xCollisionLeftBound - ball->radius && currentX <= gameObject->xCollisionRightBound + ball->radius &&
				currentZ >= gameObject->zCollisionFrontBound - ball->radius && currentZ <= gameObject->zCollisionBackBound + ball->radius)
			{
				collision = true;
				ball->dy = abs(ball->dy);
				cout << "Top collision occured." << endl;
			}


			if (previousZ + ball->radius <= gameObject->zCollisionFrontBound && currentZ + ball->radius >= gameObject->zCollisionFrontBound &&
				currentX >= gameObject->xCollisionLeftBound - ball->radius && currentX <= gameObject->xCollisionRightBound + ball->radius &&
				currentY >= gameObject->yCollisionBottomBound - ball->radius && currentY <= gameObject->yCollisionTopBound + ball->radius)
			{
				collision = true;
				ball->dz = -abs(ball->dz);
				cout << "Front collision occured." << endl;
			}

			if (previousZ - ball->radius >= gameObject->zCollisionBackBound && currentZ - ball->radius <= gameObject->zCollisionBackBound &&
				currentX >= gameObject->xCollisionLeftBound - ball->radius && currentX <= gameObject->xCollisionRightBound + ball->radius &&
				currentY >= gameObject->yCollisionBottomBound - ball->radius && currentY <= gameObject->yCollisionTopBound + ball->radius)
			{
				collision = true;
				ball->dz = abs(ball->dz);
				cout << "Back collision occured." << endl;
			}

			
			if (collision) {
				//collision occured, decrement health by one.
				gameObject->health--;
				if (gameObject->health == 0) { // health is 0, cube should be destroyed.
					cout << gameObject->tag() << " has been destroyed." << endl;
					removeObject(gameObject, false);
					removeCollidableGameObject(gameObject, true);
				}
				else {
					//if the cube is damaged and not destroyed, update its color.
					gameObject->color = color4(0.0f, 1.0f * gameObject->health / gameObject->maxHealth, 0.0f, 1.0f);
				}
			}
		}
		else if (gameObject->collisionType == gameObject->worldBoundsCollision){ // WORLD BOUND

			//WORLD BOUNDS 
			if (ball->x - ball->radius <= gameObject->xCollisionLeftBound || ball->x + ball->radius >= gameObject->xCollisionRightBound) {
				ball->dx = -ball->dx;
			}

			if (ball->y - ball->radius <= gameObject->yCollisionBottomBound || ball->y + ball->radius >= gameObject->yCollisionTopBound) {
				ball->dy = -ball->dy;
			}

			if (ball->z - ball->radius <= gameObject->zCollisionBackBound) {
				ball->dz = -ball->dz;
			}

			//ball is returned
			if (ball->z - ball->radius * 4 >= gameObject->zCollisionFrontBound) {
				
				ball->returned = true;
				bool endTurn = true;

				//check if every ball is returned
				for (int i = 0; i < shootingBalls.size(); i++) {
					if (!shootingBalls.at(i)->returned) {
						if (lightBall == ball) {
							lightBall = shootingBalls.at(i);
						}

						endTurn = false;
						break;
					}
				}

				if (endTurn) {
					// every ball is returned, a new turn should start. reset properties.
					generateSquaresOnNewTurn();
					startingBall->isVisible = true;
					startingBall->reset();
					startingBall->updateMatrices();
					lightBall = startingBall;
					newTurnAnimation = true;
					this->rotateHelperBalls(0, 0);
					this->translateHelperBallsToPosition(startingBall->x, startingBall->y);

					//reset lights and helper balls depending on camera mode.
					if (lightAsPoint) {
						this->showHelperBalls(true);
						this->shootingVector = vec4(0.0f, 0.0f, -0.5f, 0.0f);
						this->xRotationByDegrees = 0;
						this->yRotationByDegrees = 0;
					}
					else {
						this->showHelperBalls(false);

					}
					for (int i = 0; i < shootingBalls.size(); i++) {
						//reset shooting balls.
						shootingBalls.at(i)->isThrown = false;
						shootingBalls.at(i)->returned = false;
						shootingBalls.at(i)->x = 0;
						shootingBalls.at(i)->y = 0;
						shootingBalls.at(i)->z = 1000;
						shootingBalls.at(i)->updateMatrices();

					}

					currentTurnBallCount = ballCount;

				}

			}
		}

	}	

	for (int i = powerUpSpheres.size() - 1; i >= 0; i--) { // extra ball collision

		Ball* powerUpSphere = powerUpSpheres.at(i);
		vec3 ballPosition = vec3(ball->x, ball->y, ball->z);
		vec3 powerUpSpherePosition = vec3(powerUpSphere->x, powerUpSphere->y, powerUpSphere->z);
		float distanceBetweenOrigins = distanceBtwPoints(ballPosition, powerUpSpherePosition);
		float totalRadius = ball->radius + powerUpSphere->radius;

		if (distanceBetweenOrigins <= totalRadius) { //simple ball->ball collision detection.
			removeObject(powerUpSphere, false);
			removePowerUpShere(powerUpSphere, true);
			ballCount += WorldBounds::worldSizeByCubesY;
			addNewShootingBalls();
			cout << "New total ball count : " << ballCount << endl;
		}



	}
	

}

void ObjectHolder::createRandomBox(float leftBound, float rightBound, float bottomBound, float topBound, float frontBound, float backBound, float boxSize) {
	//Create a random box between the bounds, for testing.
	point4 randomPoint = point4(0, 0, 0, 1.0f);
	randomPoint.x = randomFloat(leftBound, rightBound - boxSize);
	randomPoint.y = randomFloat(bottomBound, topBound - boxSize);
	randomPoint.z = randomFloat(frontBound, backBound - boxSize);

	Cube* cube = new Cube(randomPoint, boxSize);

	addObject(cube);
	addCollidableGameObject(cube);

}

float ObjectHolder::randomFloat(float min, float max) {
	//return a float between min and max values.
	float random = ((float)rand()) / (float)RAND_MAX;
	return min + random * (max - min);
}

void ObjectHolder::generateHelperBalls(vec4 vector)
{

	//Helper balls to visualize the direction of the ball are generated at the start of the game.

	vec3 currentPosition = vec3(vector.x,vector.y,vector.z);
	float xSquared = pow((int) (worldBounds->xCollisionLeftBound - worldBounds->xCollisionRightBound), 2);
	float ySquared = pow((int)(worldBounds->yCollisionBottomBound - worldBounds->yCollisionTopBound), 2);
	float zSquared = pow((int)(worldBounds->zCollisionBackBound - worldBounds->zCollisionFrontBound), 2);

	float diagonalLength = sqrt(xSquared + ySquared + zSquared); //Diagonal is the longest distance from 2 vertices in a 3D rectangle.
	int amountOfHelperBalls = diagonalLength / length(vector); //the amount of balls to generate is calculated via diagonals length.
	for (int i = 0; i < amountOfHelperBalls; i++) {
		Ball* ball = generateBall(0.05f, currentPosition);
		addObject(ball);
		helperBalls.push_back(ball);

		currentPosition = currentPosition + vec3(vector.x, vector.y, vector.z);
	}
	
}

void ObjectHolder::rotateHelperBalls(float xRot, float yRot)
{
	//Rotate each helper ball
	for (int i = 0; i < helperBalls.size(); i++) {
		Ball* ball = helperBalls.at(i);
		ball->rotateAround = vec3(this->startingBall->x, this->startingBall->y, 0.0f);
		ball->xRotation = xRot;
		ball->yRotation = yRot;
		ball->updateMatrices();
	}
}

void ObjectHolder::translateHelperBallsToPosition(float x, float y)
{
	//Translate each helper ball
	for (int i = 0; i < helperBalls.size(); i++) {
		Ball* ball = helperBalls.at(i);
		ball->x = x;
		ball->y = y;
		ball->updateMatrices();
	}
}

void ObjectHolder::showHelperBalls(bool show) {

	//Show/Hide helper balls
	for (int i = 0; i < helperBalls.size(); i++) {
		Ball* ball = helperBalls.at(i);
		ball->isVisible = show;
	}
}

Ball* ObjectHolder::generateBall(float radius, vec3 position)
{
	//Generate a ball of certain radius and certain center.

	vector<point4> ballVertices = startingBall->getVertices();
	vector<vec3> ballNormals = startingBall->getNormals();

	Ball* ball = new Ball(radius);
	ball->vertices = ballVertices;
	ball->normals = ballNormals;
	ball->color = color4(1.0f, 1.0f, 1.0f, 1.0f);
	
	ball->scaleModel(radius / 0.25f);
	ball->displaceModel(position.x, position.y, position.z);
	
	return ball;
}

void ObjectHolder::generateSquaresOnNewTurn()
{

	//incremenet turn
	currentTurnCount++;

	float cubeSize = WorldBounds::cubeSize;
	vector<point4> bottomLeftCorners = generateRandomBottomLeftCorners(cubeSize);
	const int generatedSquareCount = WorldBounds::worldSizeByCubesX * WorldBounds::worldSizeByCubesY / 8;
	
	//generate new cubes to destroy.
	for (int i = 0; i < generatedSquareCount; i++) {
		Cube* cube = new Cube(bottomLeftCorners.at(i),cubeSize,currentTurnCount, currentTurnCount);
		addObject(cube);
		addCollidableGameObject(cube);
	}

	//generate power-up ball
	point4 bottomLeftCorner = bottomLeftCorners.at(generatedSquareCount);
	vec3 center = vec3(bottomLeftCorner.x + cubeSize / 2, bottomLeftCorner.y + cubeSize / 2, bottomLeftCorner.z - cubeSize / 2);
	float radius = cubeSize / 4;

	Ball* ball = generateBall(radius,center);
	ball->color = color4(0.25f, 0.25f, 1.0f, 1.0f);
	addObject(ball);
	addPowerUpSphere(ball);

}

vector<point4> ObjectHolder::generateRandomBottomLeftCorners(float cubeSize)
{

	//the following generates all possible bottom left corners and returns a shuffled version
	//this is done in order to generate unique, random bottom left corners of amount we want from all possible values.
	vector<point4> vector;
	for (int i = 0; i < WorldBounds::worldSizeByCubesX; i++) {
		for (int j = 0; j < WorldBounds::worldSizeByCubesY; j++) {
			point4 point = point4(-WorldBounds::worldSizeByCubesX / 2 * cubeSize + i*cubeSize, -WorldBounds::worldSizeByCubesY / 2 * cubeSize + j*cubeSize, 
				-WorldBounds::worldSizeByCubesZ * cubeSize + cubeSize * 1,1.0f);
			vector.push_back(point);
		}
	}

	std::shuffle(std::begin(vector), std::end(vector), e);
	return vector;
}

void ObjectHolder::bringCubesCloser(float dz) {
	//Cubes and power  up spheres are brought closer by a cube size in small amounts dz at the end of the turn.
	for (int i = collidableGameObjects.size() - 1; i >= 0; i--) {
		GameObject* gameObject = collidableGameObjects.at(i);
		if (gameObject->destroyOnImpact) { //Object is a cube
			gameObject->displaceModel(0,0,dz);
			gameObject->zCollisionBackBound += dz;
			gameObject->zCollisionFrontBound += dz;

		}
	}

	for (int i = powerUpSpheres.size() - 1; i >= 0; i--) {
		Ball* powerUpSphere = powerUpSpheres.at(i);
		powerUpSphere->displaceModel(0, 0, dz);
	}


}

void ObjectHolder::newTurn() {
	//New turn

	newTurnAnimation = false;
	newTurnTotalDisplacement = 0;
	allowShoot = true;
	setLightAsPoint(this->lightAsPoint);
	
	for (int i = collidableGameObjects.size() - 1; i >= 0; i--) {
		GameObject* gameObject = collidableGameObjects.at(i);
		if (gameObject->destroyOnImpact) { //Object is a cube
			gameObject->numberOfTurnsExisted++; //increment the turns existed
			if (gameObject->numberOfTurnsExisted + 2> WorldBounds::worldSizeByCubesZ) {
				//The cube existed for too many turns, it reached the z=0 plane, game is over.
				gameOver = true;
				break;
			}
		}
	}

	for (int i = powerUpSpheres.size() - 1; i >= 0; i--) {
		Ball* powerUpSphere = powerUpSpheres.at(i);
		powerUpSphere->numberOfTurnsExisted++;//increment the turns existed
		if (powerUpSphere->numberOfTurnsExisted + 2 > WorldBounds::worldSizeByCubesZ) {
			//The extra sphere existed for too many turns, it reached the z=0 plane, sphere is removed the game.
			removePowerUpShere(powerUpSphere, false);
			removeObject(powerUpSphere, true);
		}

	}

	if (gameOver) {
		cout << "Game Over!" << endl;
	}


	
}

void ObjectHolder::resetGame() {
	//reset game
	for (int i = collidableGameObjects.size() - 1; i >= 0; i--) {
		GameObject* o = collidableGameObjects.at(i);
		removeObject(o,false);
		removeCollidableGameObject(o, true);
	}
	
	for (int i = objects.size() - 1; i >= 0; i--) {
		Object* o = objects.at(i);
		removeObject(o, true);
	}
	
	objects.clear();
	collidableGameObjects.clear();
	gameOver = false;

}

void ObjectHolder::update()
{
	//game's logic loop updates values such as balls position depending on velocity,
	//bringing cubes closer if the turn had ended.

	if (newTurnAnimation) {
		//Bring cubes closer with small amounts for animation
		newTurnTotalDisplacement += 0.06f;
		bringCubesCloser(0.06f);
		if (newTurnTotalDisplacement >= WorldBounds::cubeSize) {
			//in case at the end of bringing cubes together, it overshoots, redo the overshoot displacement.
			float overshoot = WorldBounds::cubeSize - newTurnTotalDisplacement;
			bringCubesCloser(overshoot);
			newTurn();
		}
		
	}
	else { 
		if (startingBall->dx == 0 && startingBall->dy == 0 & startingBall->dz == 0) {
			//balls are not thrown.
			lightBall = startingBall;
			updateLight();
			return;
		}


		//Since the function is not returned, the balls are thrown, so check if the first one is moving yet,
		//if not, initiate its motion
		Ball* firstBall = shootingBalls.at(0);
		if (!firstBall->isThrown) {
			allowShoot = false;
			startingBall->isVisible = false;
			firstBall->isThrown = true;
			firstBall->x = startingBall->x;
			firstBall->y = startingBall->y;
			firstBall->z = startingBall->z;
			firstBall->setVelocity(startingBall->dx, startingBall->dy, startingBall->dz);
			lightBall = firstBall;
		}
		
		
		// for each ball after the first ball, check if the previous ball has moved enough so that the new ball should initiate its movement.
		for (int i = 1; i < currentTurnBallCount; i++) {
			Ball* previousBall = shootingBalls.at(i - 1);
			Ball* currentBall = shootingBalls.at(i);

			
			vec3 startingBallCenter = vec3(startingBall->x, startingBall->y, startingBall->z);
			vec3 previousBallCenter = vec3(previousBall->x, previousBall->y, previousBall->z);


			if (previousBall->isThrown && distanceBtwPoints(startingBallCenter, previousBallCenter) > startingBall->radius * 40) {
				//The previous ball has moved enough so that the new ball should initiate its movement.
				if (!currentBall->isThrown) {
					currentBall->isThrown = true;
					currentBall->x = startingBall->x;
					currentBall->y = startingBall->y;
					currentBall->z = startingBall->z;
					currentBall->setVelocity(startingBall->dx, startingBall->dy, startingBall->dz);

				}
			}
		}

		//Displace all the balls that initiated its movement.
		for (int i = 0; i < currentTurnBallCount; i++) {
			if (shootingBalls.at(i)->isThrown) {
				this->moveBall(shootingBalls.at(i));
			}
		}

		//update light's position with respect to the light ball.
		glUniform4fv(glGetUniformLocation(program, "LightPosition"),
			1, vec4(this->lightBall->x, this->lightBall->y, this->lightBall->z, 1.0f));
	}
}

float ObjectHolder::distanceBtwPoints(vec3 p1, vec3 p2)
{
	//return distance between two points.
	float xs = pow(p1.x - p2.x, 2);
	float ys = pow(p1.y - p2.y, 2);
	float zs = pow(p1.z - p2.z, 2);

	return sqrt(xs+ys+zs);
}

void ObjectHolder::addNewShootingBalls() {
	//Extra ball has been picked up, generate new shooting balls at a far away distance for future.
	for (int i = 0; i < WorldBounds::worldSizeByCubesY; i++) {
		Ball* ball = generateBall(startingBall->radius,vec3(0.0f,0.0f,1000.0f));
		ball->color = startingBall->color;
		ball->isThrown = false;
		ball->returned = true;
		addObject(ball);
		shootingBalls.push_back(ball);
	}
}

void ObjectHolder::moveStartingBall(float dx, float dy)
{
	//Displace the starting ball, which is the ball where shooting balls start being thrown from.
	startingBall->x += dx;
	startingBall->y += dy;

	if (startingBall->x + startingBall->radius >= worldBounds->xCollisionRightBound) {
		startingBall->x = worldBounds->xCollisionRightBound - startingBall->radius;

	} 
	
	if (startingBall->x - startingBall->radius <= worldBounds->xCollisionLeftBound) {
		startingBall->x = worldBounds->xCollisionLeftBound + startingBall->radius;

	} 
	
	if (startingBall->y + startingBall->radius >= worldBounds->yCollisionTopBound) {
		startingBall->y = worldBounds->yCollisionTopBound - startingBall->radius;

	} 
	
	if (startingBall->y - startingBall->radius <= worldBounds->yCollisionBottomBound) {
		startingBall->y = worldBounds->yCollisionBottomBound + startingBall->radius;
	}

	this->translateHelperBallsToPosition(startingBall->x, startingBall->y);
	startingBall->updateMatrices();
}

void ObjectHolder::setLightAsPoint(bool pointLight)
{
	this->lightAsPoint = pointLight;
	updateLight();

}

void ObjectHolder::updateLight() {
	if (lightAsPoint) { //POINT LIGHT ON
		glUniform4fv(glGetUniformLocation(program, "LightPosition"),
			1, vec4(this->lightBall->x, this->lightBall->y, this->lightBall->z, 1.0f));
	}
	else { //DIRECTIONAL LIGHT ON
		glUniform4fv(glGetUniformLocation(program, "LightPosition"),
			1, vec4(-this->shootingVector.x, -this->shootingVector.y, -this->shootingVector.z, 0.0f));
	}
}









