#define STB_IMAGE_IMPLEMENTATION
#include <vector>  
#include <iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<map>
#include "gl/glut.h"  
#include "stb_image.h"

using namespace std;

//predefine variables
//control animation play
static int indexOfCactu = 0; 

//realize rotate view
static float c = 3.1415926 / 180.0f;
static float r = 1.0f, h = 0.0f;
static int degree = 90;
static int oldPosY = -1;
static int oldPosX = -1;

//realize interctive moveing for object
static float rotateAngle = -50, scale = 1;
static int xMove = 0, yMove = 0, zMove = 0;

//shading type and material type
static int mtlType = 2;

//predefine materials
static GLfloat Ka[3][4] = { {0.1,0.18725,0.1745,1.0},{0.24725,0.1995,0.0745,1.0},{0.19225,0.19225,0.19225,1.0} };
static GLfloat Kd[3][4] = { {0.396,0.74151,0.69102,1.0},{0.75164,0.60648,0.22648,1.0},{0.50754,0.50754,0.50754,1.0} };
static GLfloat Ks[3][4] = { {0.297254,0.30829,0.306678},{0.628281,0.555802,0.366065,1.0},{0.508273,0.508273,0.508273,1.0} };
static GLfloat Ns[3] = { 12.8,51.2,51.2 };

//3^th surface
GLfloat texturePoints[2][2][2] = {
 {{0.0, 0.0}, {1.0, 0.0}},
{{0.0, 1.0}, {1.0, 1.0}} };

GLfloat controlPoints[4][4][3] = {
{{-0.8f,-0.6f,0.8f},{-0.2f,-0.6f,1.6f},{0.2f,-0.6,-0.4f},{0.6f,-0.6f,0.8f}},
{{-0.6f,-0.2f,0.8f},{-0.2f,-0.2f,1.6f},{0.2f,-0.2f,-0.4f},{0.6f,-0.2f,0.8f}},
{{-0.6f,0.2f,0.8f},{-0.2f,0.2f,0.4f},{0.2f,0.2f,0.0f},{0.3f,0.2f,-0.4f}},
{{-0.6f,0.6f,0.8},{-0.2f,0.6f,0.4},{-0.8f,0.6f,0.0f},{0.8f,0.6f,-0.4f}}
};


GLuint desert; //texture
string cactuPath = "./modelSequence/cactus/cactus_";

GLfloat sknots[8] = { 0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,1.0f };
GLfloat tknots[8] = { 0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,1.0f };

static bool shadeType=1;

//GLuint textureLoad() {
//	GLfloat texpts[2][2][2] = {
//	 {{0.0, 0.0}, {1.0, 0.0}},
//	{{0.0, 1.0}, {1.0, 1.0}} };
//	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2,
//		0, 1, 4, 2, &texpts[0][0][0]);
//	glEnable(GL_MAP2_TEXTURE_COORD_2);
//	GLuint texture = 0;
//	int width, height, nrChannels;
//	stbi_set_flip_vertically_on_load(true);
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	unsigned char* data = stbi_load("./texture/desert.png", &width, &height, &nrChannels, 0);
//
//	glGenTextures(1, &textureID);
//	glBindTexture(GL_TEXTURE_2D, texture);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//	stbi_image_free(data);
//	return textureID;
//}

//load texture
GLuint textureLoad() {
	GLuint textureID = 0;
	int w, h, channels;
	stbi_set_flip_vertically_on_load(true);

	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2,
		0, 1, 4, 2, &texturePoints[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char* data = stbi_load("./texture/desert.png", &w, &h, &channels, 0);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (data) {
		try
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		catch (exception ex)
		{
			cout << ex.what();
		}
	}
	else {
		cout << "Failed to load png\n";
	}
	stbi_image_free(data);
	return textureID;
}


//obj loader class
class objLoader {
private:
	vector<vector<GLfloat>> v;  //vectex
	vector<vector<GLfloat>> fv; //face's vector
	vector<vector<GLfloat>> vn; //vectex's normal vctor
	vector<vector<GLfloat>> fn; //face's normal vector
	map<int, vector<int>> neighbourFace; //the neighbour faces of v
public:
	objLoader(string filePath) {
		//if (objKind) {
		//	myMtlLoader = mtlLoader(chairMtl);
		//}
		//else {
		//	myMtlLoader = mtlLoader(tableMtl);
		//}
		string eachLine, tempString;
		int indexForf = 0;
		GLfloat x, y, z;
		GLint v1, v2, v3;
		fstream readObj;
		vector<GLfloat> tempVector;//store temp x,y,z,fv... variables
		vector<GLfloat> tempVector2;//store temp fn variables

		readObj.open(filePath, ios::in);
		if (!readObj.is_open()) {
			cout << "Error: obj file not open";
		}

		while (!readObj.eof()) {

			getline(readObj, eachLine);
			istringstream getVariables(eachLine);
			vector<GLfloat>().swap(tempVector);
			vector<GLfloat>().swap(tempVector2);

			//v
			if (eachLine[0] == 'v') {
				getVariables >> tempString >> x >> y >> z;
				tempVector.push_back(x);
				tempVector.push_back(y);
				tempVector.push_back(z);
				v.push_back(tempVector);
			}
			//f
			else if (eachLine[0] == 'f') {
				indexForf += 1;
				getVariables >> tempString >> v1 >> v2 >> v3;
				tempVector.push_back(v1);
				tempVector.push_back(v2);
				tempVector.push_back(v3);
				fv.push_back(tempVector);
			}
			//other attritubes opmitted now
			else {
				//g,muti...
				continue;
			}
		}
		preProcessnNormal();
		readObj.close();
	};

	void preProcessnNormal() {
		try
		{
			//get face normal vector
			for (int i = 0; i < int(fv.size()); i++) {
				GLfloat tempVec1[3], tempVec2[3], vec[3], absVec;
				for (int j = 0; j < 3; j++) {
					tempVec1[j] = v[fv[i][0] - 1][j] - v[fv[i][1] - 1][j];
				}

				for (int j = 0; j < 3; j++) {
					tempVec2[j] = v[fv[i][0] - 1][j] - v[fv[i][2] - 1][j];
				}

				vec[0] = tempVec1[1] * tempVec2[2] - tempVec1[2] * tempVec2[1];
				vec[1] = tempVec1[0] * tempVec2[2] - tempVec1[2] * tempVec2[0];
				vec[2] = tempVec1[1] * tempVec2[0] - tempVec1[0] * tempVec2[1];
				absVec = sqrt(pow(vec[0], 2) + pow(vec[1], 2) + pow(vec[2], 2));

				vector <GLfloat> tempNormal{ vec[0] / absVec ,vec[1] / absVec ,vec[2] / absVec };
				fn.push_back(tempNormal);

				for (int j = 0; j < 3; j++) {
					if (neighbourFace.find(fv[i][j] - 1) == neighbourFace.end()) {
						neighbourFace[fv[i][j] - 1] = vector<int>{ i };
					}
					else {
						neighbourFace[fv[i][j] - 1].push_back(i);
					}
				}
			}

			//i-vertex index
			for (int i = 0; i<int(v.size()); i++) {
				GLfloat vec[3] = { 0,0,0 };

				for (int j = 0; j<int(neighbourFace[i].size()); j++) {
					vec[0] += fn[neighbourFace[i][j]][0];
					vec[1] += fn[neighbourFace[i][j]][1];
					vec[2] += fn[neighbourFace[i][j]][2];
				}

				vec[0] /= neighbourFace[i].size();
				vec[1] /= neighbourFace[i].size();
				vec[2] /= neighbourFace[i].size();

				vn.push_back(vector<GLfloat> {vec[0], vec[1], vec[2]});
			}
		}
		catch (exception& ex)
		{
			cout << ex.what() << endl;
		}
	}

	void draw(int shadeType) {
		try {
			glPushMatrix();
			glTranslatef(xMove, yMove, zMove);
			glRotatef(rotateAngle, 0, 1, 0);
			glScalef(scale, scale, scale);
			//gourand shading
			if (shadeType) {
				for (int i = 0; i < int(fv.size()); i++) {
					glBegin(GL_TRIANGLES);

					glVertex3f(v[fv[i][0] - 1][0], v[fv[i][0] - 1][1], v[fv[i][0] - 1][2]);
					glNormal3f(vn[fv[i][0] - 1][0], vn[fv[i][0] - 1][1], vn[fv[i][0] - 1][2]);

					glVertex3f(v[fv[i][1] - 1][0], v[fv[i][1] - 1][1], v[fv[i][1] - 1][2]);
					glNormal3f(vn[fv[i][1] - 1][0], vn[fv[i][1] - 1][1], vn[fv[i][1] - 1][2]);

					glVertex3f(v[fv[i][2] - 1][0], v[fv[i][2] - 1][1], v[fv[i][2] - 1][2]);
					glNormal3f(vn[fv[i][2] - 1][0], vn[fv[i][2] - 1][1], vn[fv[i][2] - 1][2]);

					glEnd();
				}
			}
			//flat shading
			else {
				for (int i = 0; i < int(fv.size()); i++) {
					glBegin(GL_TRIANGLES);

					glNormal3f(fn[i][0], fn[i][1], fn[i][2]);

					//three vertexs draw
					glVertex3f(v[fv[i][0] - 1][0], v[fv[i][0] - 1][1], v[fv[i][0] - 1][2]);
					glVertex3f(v[fv[i][1] - 1][0], v[fv[i][1] - 1][1], v[fv[i][1] - 1][2]);
					glVertex3f(v[fv[i][2] - 1][0], v[fv[i][2] - 1][1], v[fv[i][2] - 1][2]);

					glEnd();
				}
			}
			glPopMatrix();
		}
		catch (exception ex) {
			cout << ex.what();
		}

	};

	void print() {
		for (int i = 0; i < fv.size(); i++) {
			cout << fv[i][0] << " " << fv[i][1] << " " << fv[i][2] << endl;
		}
	}

};

//create cactus vector
vector<objLoader> cactus;

//set light sources
void setLightRes() {

	GLfloat lightPosition[] = { 3.0f, 3.0f, 0.0f, 1.0f };
	GLfloat lightPosition2[] = { 0.0f, 3.0f, 3.0f, 1.0f };
	GLfloat lightAmbient[] = { 1,1,1,1 };
	GLfloat lightDiffuse[] = { 1,1,1,1 };
	GLfloat lightSpecular[] = { 1,1,1,1 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
	glEnable(GL_LIGHT1);
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(r * cos(c * degree), h, r * sin(c * degree), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	setLightRes();

	glPushAttrib(GL_LIGHTING_BIT);
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, Ka[mtlType]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Kd[mtlType]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Ks[mtlType]);
	glMaterialf(GL_FRONT, GL_SHININESS, Ns[mtlType]);
	cactus[indexOfCactu].draw(shadeType);
	glPopMatrix();
	glPopAttrib();

	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);

	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &controlPoints[0][0][0]);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glPopMatrix();
	glPopAttrib();

	glutSwapBuffers();
}




void myInit(int shadeType) {
	//load cactu
	for (int i = 0; i < 128; i++) {
		cactus.push_back(objLoader(cactuPath + to_string(i) + ".obj"));
		cout <<"Loading cactu: "<< i << endl;
	}

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Grass");
	
	if(shadeType)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	glEnable(GL_MAP2_VERTEX_3);

	desert = textureLoad();

}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLdouble)width / (GLdouble)height, 1.0f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
}

void mouseClicked(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		oldPosX = x; oldPosY = y;
	}

}

void changeViewPoint(int x, int y)
{
	degree += x - oldPosX;
	h += 0.03 * (y - oldPosY);
	oldPosX = x;
	oldPosY = y;
}

void keyFunction(unsigned char key, int x, int y) {
	switch (key)
	{
	//move part
	case 'w':
		zMove += 1;
		break;
	case 's':
		zMove -= 1;
		break;
	case 'a':
		xMove += 1;
		break;
	case 'd':
		xMove -= 1;
		break;
	case 'z':
		yMove += 1;
		break;
	case'x':
		yMove -= 1;
		break;

	//rotate part
	case '4':
		rotateAngle += 45;
		break;
	case '6':
		rotateAngle += -45;
		break;

	//scalf part
	case '+':
		scale *= 2;
		break;
	case '-':
		scale *= 0.5;
		break;

		//material part
	case '1':
		mtlType = 0;
		break;
	case '2':
		mtlType = 1;
		break;
	case '3':
		mtlType = 2;
		break;
	default:
		break;
	}
}

//play animation
void myIdle()
{
	if (indexOfCactu == 127) {
		indexOfCactu = 0;
	}
	else {
		indexOfCactu += 1;
	}
	Sleep(16);
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	cout << "Please choose the shading model: 0-flat, 1-gouraud\n";
	cin >> shadeType;

	glutInit(&argc, argv);
	myInit(shadeType);

	//resigter functions 
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseClicked);
	glutMotionFunc(changeViewPoint);
	glutKeyboardFunc(keyFunction);
	glutIdleFunc(myIdle);

	glutMainLoop();

}
