#include <GL/glew.h>
#include <GL/glfw.h>
#include <iostream>
#include <cmath>
#include <cassert>

using namespace std;

struct GLMatrix3 {
	GLfloat mat[9];

	void setIdentity() {
		mat[0] = 1, mat[3] = 0, mat[6] = 0;
		mat[1] = 0, mat[4] = 1, mat[7] = 0;
		mat[2] = 0, mat[5] = 0, mat[8] = 1;
	}

	void setRotation(GLfloat x, GLfloat y, GLfloat theta) {
		const GLfloat c = cos(theta), s = sin(theta);
		mat[0] = c, mat[3] = -s, mat[6] = -c * x + s * y + x;
		mat[1] = s, mat[4] = c,  mat[7] = -s * x - c * y + y;
		mat[2] = 0, mat[5] = 0,  mat[8] = 1;
	}

	void setTranslation(GLfloat x, GLfloat y) {
		mat[0] = 1, mat[3] = 0, mat[6] = x;
		mat[1] = 0, mat[4] = 1, mat[7] = y;
		mat[2] = 0, mat[5] = 0, mat[8] = 1;
	}

	void translate(GLfloat x, GLfloat y) {
		mat[6] += x;
		mat[7] += y;
	}

	void scale(GLfloat sx, GLfloat sy) {
		mat[0] *= sx;
		mat[3] *= sx;
		mat[6] *= sx;

		mat[1] *= sy;
		mat[4] *= sy;
		mat[7] *= sy;
	}

	void swap(GLfloat a, GLfloat b)
	{
		GLfloat temp = a;
		a = b;
		b = temp;
	}
	
	void transpose() {
		swap(mat[3],mat[1]);
		swap(mat[6],mat[2]);
		swap(mat[7],mat[5]);
	}

	GLMatrix3& operator=(const GLMatrix3 &rhs) {
		memcpy(mat, rhs.mat, sizeof(mat));
		return *this;
	}

	GLMatrix3 operator*(const GLMatrix3 &rhs) const {
		GLMatrix3 ret;
		for ( int i = 0; i < 9; ++i ) {
			const int a = i % 3, b = (i / 3) * 3;
			ret.mat[i] = mat[a]*rhs.mat[b] + mat[a+3]*rhs.mat[b+1] + mat[a+6]*rhs.mat[b+2];
		}
		return ret;
	}

	GLMatrix3& operator*=(const GLMatrix3 &rhs) {
		GLMatrix3 tmp;
		for ( int i = 0; i < 9; ++i ) {
			const int a = i % 3, b = (i / 3) * 3;
			tmp.mat[i] = mat[a]*rhs.mat[b] + mat[a+3]*rhs.mat[b+1] + mat[a+6]*rhs.mat[b+2];
		}
		memcpy(mat, tmp.mat, sizeof(mat));
		return *this;
	}
};

class SceneNode {
public:
	GLMatrix4 transform;
	GLMatrix4 UNIFORM_mvpMatrix;
	
	vector<SceneNode*> children;
	SceneNode() {
		transform.setIdentity();
		UNIFORM_mvpMatrix.setIdentity();
	}
	virtual void draw(const GLMatrix4 &parentTransform) {
		drawChildren(parentTransform * transform);
	}

	virtual void update(double t) {
		for ( size_t i = 0; i < children.size(); ++i )
			children[i]->update(t);
	}

	void drawChildren(const GLMatrix4 &t) {
		for ( size_t i = 0; i < children.size(); ++i )
			children[i]->draw(t);
	}

	static void setTransform(const GLMatrix4 &t) {
		glUniformMatrix4fv(UNIFORM_mvpMatrix, 1, false, t.mat);
	}

	virtual ~SceneNode() {
	}
};

void initShader()
{
	GLuint fShader = glCreateShader( GL_FRAGMENT_SHADER );
	GLuint vShader = glCreateShader( GL_VERTEX_SHADER );

	loadShaderSource( fShader, "finalProj.fsh" );
	loadShaderSource( vShader, "finalProj.vsh" );

	glCompileShader( fShader );
	checkShaderStatus( fShader );

	glCompileShader( vShader );
	checkShaderStatus( vShader );

	mainProgram = glCreateProgram();

	glAttachShader( mainProgram, vShader );
	glAttachShader( mainProgram, fShader );

	glBindAttribLocation( mainProgram, ATTRIB_POS, "position" );
	glBindAttribLocation( mainProgram, ATTRIB_COLOR, "color" );

	glLinkProgram( mainProgram );

	glDeleteShader( fShader );
	glDeleteShader( vShader );
}
int main(){
	if(!glfwInit()){
		std::cerr << "Unable to initialize OpenGL!\n";
		return -1;
	}
	
	if ( !glfwOpenWindow(640,640, //width and height of the screen
				8,8,8,8, //Red, Green, Blue and Alpha bits
				0,0, //Depth and Stencil bits
				GLFW_WINDOW)) {
		std::cerr << "Unable to create OpenGL window.\n";
		glfwTerminate();
		return -1;
	}

	if ( glewInit() != GLEW_OK ) {
		std::cerr << "Unable to hook OpenGL extensions!\n";
		return -1;
	}

	glfwSetWindowTitle("GLFW Simple Example");

	// Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );

	// Enable vertical sync (on cards that support it)
	glfwSwapInterval( 1 );

	glClearColor(0,0,0,0);
	
	initShader();
	
	glEnableVertexAttribArray( ATTRIB_POS );
	glEnableVertexAttribArray( ATTRIB_COLOR );
	
	glUseProgram(mainProgram);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}