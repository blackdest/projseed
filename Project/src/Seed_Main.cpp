#include <GL/glew.h>
#include <GL/glfw.h>
#include <iostream>
#include <cmath>
#include <cassert>
#include <vector>
#include <cstdio>

using namespace std;
static const double MY_PI = 3.14159265358979323846264338327;

struct Vtx{
	GLfloat x,y,z;
	GLuint color;
};
enum{ATTRIB_POS, ATTRIB_COLOR};
GLuint mvpMat;
GLuint mainProgram;

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
struct GLMatrix4 {
	GLfloat mat[16];

	void setIdentity() {
		mat[0] = 1, mat[4] = 0, mat[8] = 0, mat[12] = 0;
		mat[1] = 0, mat[5] = 1, mat[9] = 0, mat[13] = 0;
		mat[2] = 0, mat[6] = 0, mat[10] = 1, mat[14] = 0;
		mat[3] = 0, mat[7] = 0, mat[11] = 0, mat[15] = 1;
	}
	
	void multiply_matrix_4x4(const GLfloat *A, const GLfloat *B, GLfloat *C) {
        int i = 0;
        for( i = 0; i < 4; i++)
        {
            C[i] = (A[i] * B[0]) + (A[i+3] * B[1]) + (A[i+6] * B[2]) + (A[i+9] * B[3]);
            C[i+3] = (A[i] * B[4]) + (A[i+3] * B[5]) + (A[i+6] * B[6]) + (A[i+9] * B[7]);
            C[i+6] = (A[i] * B[8]) + (A[i+3] * B[9]) + (A[i+6] * B[10]) + (A[i+9] * B[11]);
            C[i+9] = (A[i] * B[12]) + (A[i+3] * B[13]) + (A[i+6] * B[14]) + (A[i+9] * B[15]);
        }
	}	
	
	void cross_product_3d(GLfloat *A, GLfloat *B, GLfloat*C){
		C[0] = (A[1] * B[2]) - (A[2] * B[1]);
		C[1] = -((A[0] * B[2]) - (A[2] * B[0]));
		C[2] = (A[0] * B[1]) - (A[1] * B[0]);
	}
	
	void transposer (GLfloat *A, GLfloat *C){
		C[0] = A[0], C[3] = A[1], C[6] = A[2];
		C[1] = A[3], C[4] = A[4], C[7] = A[5];
		C[2] = A[6], C[5] = A[7], C[8] = A[8];
	}
	
	void setRotation(GLfloat x, GLfloat y, GLfloat z, GLfloat theta ) {
		 GLfloat Rx[16];
		 Rx[0] = 1, Rx[4] = 0         , Rx[8] =  0          ,  Rx[12] = 0;
		 Rx[1] = 0, Rx[5] = cos(theta), Rx[9] =  -sin(theta),  Rx[13] = 0;
		 Rx[2] = 0, Rx[6] = sin(theta), Rx[10] = cos(theta) ,  Rx[14] = 0;
		 Rx[3] = 0, Rx[7] = 0         , Rx[11] = 0      ,  Rx[15] = 1;
		 GLfloat *a;
		 a[0] = a[1] = a[2] = 0;
		 GLfloat Ax[16];
		 GLfloat u2[3];
		 GLfloat u3[3];
		 GLfloat Ainv[16];
		 GLfloat res1[16];
		 GLfloat xvec[3];
		 xvec[0] = 1, xvec[1] = 0, xvec[2] = 0;
		 cross_product_3d(a,xvec,u2);
		 cross_product_3d(a,u2,u3);
		 Ax[0] = a[0], Ax[4] = u2[0], Ax[8] =  u3[0],  Ax[12] = 0;
		 Ax[1] = a[1], Ax[5] = u2[1], Ax[9] =  u3[1],  Ax[13] = 0;
		 Ax[2] = a[2], Ax[6] = u2[2], Ax[10] = u3[2],  Ax[14] = 0;
		 Ax[3] = 0,    Ax[7] = 0,     Ax[11] = 0    ,  Ax[15] = 1;
		 transposer(Ax, Ainv);
		 multiply_matrix_4x4(Rx,Ainv,res1);
		 multiply_matrix_4x4(Ax,res1,mat);
	}

	void setTranslation(GLfloat x, GLfloat y, GLfloat z) {
		mat[0] = 1, mat[4] = 0, mat[8] = 0, mat[12] = x;
		mat[1] = 0, mat[5] = 1, mat[9] = 0, mat[13] = y;
		mat[2] = 0, mat[6] = 0, mat[10] = 1, mat[14] = z;
		mat[3] = 0, mat[7] = 0, mat[11] = 0, mat[15] = 1;
	}

	void translate(GLfloat x, GLfloat y, GLfloat z) {
		mat[12] += x;
		mat[13] += y;
		mat[14] += z;
	}

	void scale(GLfloat sx, GLfloat sy, GLfloat sz) {
		mat[0] *= sx;
		mat[4] *= sx;
		mat[8] *= sx;
		mat[12] *= sx;

		mat[1] *= sy;
		mat[5] *= sy;
		mat[9] *= sy;
		mat[13] *= sy;

		mat[2] *= sz;
		mat[6] *= sz;
		mat[10] *= sz;
		mat[14] *= sz;
	}
	
	void swap(GLfloat a, GLfloat b)
	{
		GLfloat temp = a;
		a = b;
		b = temp;
	}
	
	void transpose_3d() {
		swap(mat[4],mat[1]);
		swap(mat[8],mat[2]);
		swap(mat[12],mat[3]);
		swap(mat[9],mat[6]);
		swap(mat[13],mat[7]);
		swap(mat[14], mat[11]);
	}

	GLMatrix4& operator=(const GLMatrix4 &rhs) {
		memcpy(mat, rhs.mat, sizeof(mat));
		return *this;
	}

	GLMatrix4 operator*(const GLMatrix4 &rhs) const {
		GLMatrix4 ret;

		int i = 0;
        for( i = 0; i < 4; i++)
        {
			ret.mat[i] = (mat[i] * rhs.mat[0]) + (mat[i+3] * rhs.mat[1]) + (mat[i+6] * rhs.mat[2]) + (mat[i+9] * rhs.mat[3]);
			ret.mat[i+3] = (mat[i] * rhs.mat[4]) + (mat[i+3] * rhs.mat[5]) + (mat[i+6] * rhs.mat[6]) + (mat[i+9] * rhs.mat[7]);
			ret.mat[i+6] = (mat[i] * rhs.mat[8]) + (mat[i+3] * rhs.mat[9]) + (mat[i+6] * rhs.mat[10]) + (mat[i+9] * rhs.mat[11]);
			ret.mat[i+9] = (mat[i] * rhs.mat[12]) + (mat[i+3] * rhs.mat[13]) + (mat[i+6] * rhs.mat[14]) + (mat[i+9] * rhs.mat[15]);
        }

		return ret;
	}

	GLMatrix4& operator*=(const GLMatrix4 &rhs) {
		GLfloat tmp[16];
		multiply_matrix_4x4(mat, rhs.mat, tmp);

		memcpy(mat, tmp, sizeof(mat));
		return *this;
	}
};
class SceneNode {
public:
	GLMatrix4 transform;
	vector<SceneNode*> children;
	SceneNode() {
		transform.setIdentity();
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
	
	virtual ~SceneNode() {
	}
};

class PyramidNode : public SceneNode {
	vector<Vtx> vertices;
	GLfloat height;
	public:
	PyramidNode(GLfloat radius, GLfloat height, GLuint sides, GLuint tipColor, GLuint color1, GLuint color2) : vertices(2 + max(sides,3u)), height(height) {
		sides = max(sides,3u);
		vertices.front().x = vertices.front().y = 0;
		vertices.front().z = height;
		vertices.front().color = tipColor;

		for ( size_t i = 0; i < sides; ++i ) {
			const double angle = 2.0 * i * MY_PI/(double)sides;
			vertices[i + 1].x = radius * cos(angle);
			vertices[i + 1].y = radius * sin(angle);
			vertices[i + 1].z = 0;
			if ( (i + 1) % 2 == 0 )
				vertices[i + 1].color = color1;
			else
				vertices[i + 1].color = color2;
		}
		vertices.back().x = radius;
		vertices.back().y = 0;
		vertices.back().z = 0;
		vertices.back().color = ((vertices.size() % 2 == 0) ? color1 : color2);
	}

	virtual void draw(const GLMatrix4 &parentTransform) {
		const GLMatrix4 &t = parentTransform * transform;
		glVertexAttribPointer(ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), &vertices[0].x);
		glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vtx), &vertices[0].color);
		//setTransform(t);

		vertices[0].z = height;
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
		vertices[0].z = 0;
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());

		drawChildren(t);
	}
};

bool loadShaderSource(GLuint shader, const char *path) {
	FILE *f = fopen(path, "r");
	if ( !f ) {
		std::cerr << "ERROR: shader source not found: " << path << '\n';
		return false;
	}
	fseek(f, 0, SEEK_END);
	vector<char> sauce(ftell(f) + 1);
	fseek(f, 0, SEEK_SET);
	fread(&sauce[0], 1, sauce.size(), f);
	fclose(f);
	const GLchar *ptr = &sauce[0];
	glShaderSource(shader, 1, &ptr, 0);
	if ( glGetError() != GL_NO_ERROR ) {
		std::cerr << "ERROR: Unable to load shader\n";
		return false;
	}
	return true;
}

void checkShaderStatus(GLuint shader) {
	GLint logLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar *log = (GLchar *)malloc(logLength);
		glGetShaderInfoLog(shader, logLength, &logLength, log);
		std::cout << "Shader compile log:\n" << log << endl;
		free(log);
	}
}

void initShader(){
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
	
	SceneNode parent;

}