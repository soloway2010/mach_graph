//internal includes
#include "common.h"
#include "ShaderProgram.h"
#include "Camera.h"

//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <random>
#include <cmath>
#include <string.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "./dependencies/include/stb_image/stb_image.h"

static const GLsizei WIDTH = 1024, HEIGHT = 512; //размеры окна
static int filling = 0;
static bool keys[1024]; //массив состояний кнопок - нажата/не нажата
static GLfloat lastX = WIDTH/2, lastY = HEIGHT/2; //исходное положение мыши
static bool firstMouse = true;
static bool g_captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
static bool g_capturedMouseJustNow = false;

const int sharpness = 10;
const float size = 40;
const float sea_level = 0.5f;                  
int norm = 0;                       
float3 light_src = float3(0, size/2, 0);

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(float3(0.0f, 5.0f, 30.0f));

//функция для обработки нажатий на кнопки клавиатуры
void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//std::cout << key << std::endl;
	switch (key)
	{
	case GLFW_KEY_ESCAPE: //на Esc выходим из программы
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_SPACE: //на пробел переключение в каркасный режим и обратно
		if (action == GLFW_PRESS)
		{
			if (filling == 0)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				filling = 1;
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				filling = 0;
			}
		}
		break;
  case GLFW_KEY_1:
   	norm = 0;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    norm = 1;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

//функция для обработки клавиш мыши
void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    g_captureMouse = !g_captureMouse;


  if (g_captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    g_capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

//функция для обработки перемещения мыши
void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = float(xpos);
    lastY = float(ypos);
    firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - lastX;
  GLfloat yoffset = lastY - float(ypos);  

  lastX = float(xpos);
  lastY = float(ypos);

  if (g_captureMouse)
    camera.ProcessMouseMove(xoffset, yoffset);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(GLfloat(yoffset));
}

void doCameraMovement(Camera &camera, GLfloat deltaTime)
{
  if (keys[GLFW_KEY_W])
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (keys[GLFW_KEY_A])
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (keys[GLFW_KEY_S])
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (keys[GLFW_KEY_D])
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

GLuint cube_tex_load(){
	GLuint textureID;
	int w, h, channels;
	unsigned char* data;

	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	data = stbi_load("morning_up.tga", &w, &h, &channels, 3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	data = stbi_load("morning_dn.tga", &w, &h, &channels, 3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	data = stbi_load("morning_ft.tga", &w, &h, &channels, 3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	data = stbi_load("morning_bk.tga", &w, &h, &channels, 3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	data = stbi_load("morning_rt.tga", &w, &h, &channels, 3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	data = stbi_load("morning_lf.tga", &w, &h, &channels, 3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

GLuint tex_load(const char* path){
	int w, h, channels;
	
	unsigned char* data = stbi_load(path, &w, &h, &channels, 3);
	
	GLuint textureID;

	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return textureID;
}

static int sky_load(int rows, int cols, float size, GLuint* vao){
  glGenVertexArrays(6, vao);
  int numIndices = 2 * cols*(rows - 1) + rows - 1;

  for(int side = 0; side < 6; side++){
	  std::vector<GLfloat> vertices_vec; //вектор атрибута координат вершин
	  vertices_vec.reserve(rows * cols * 3);

	  std::vector<GLfloat> normals_vec; //вектор атрибута нормалей к вершинам
	  normals_vec.reserve(rows * cols * 3);

	  std::vector<GLfloat> texcoords_vec; //вектор атрибут текстурных координат вершин
	  texcoords_vec.reserve(rows * cols * 2);

	  std::vector<float3> normals_vec_tmp(rows * cols, float3(0.0f, 0.0f, 0.0f)); //временный вектор нормалей, используемый для расчетов

	  std::vector<int3> faces;         //вектор граней (треугольников), каждая грань - три индекса вершин, её составляющих; используется для удобства расчета нормалей
	  faces.reserve(numIndices / 3);

	  std::vector<GLuint> indices_vec; //вектор индексов вершин для передачи шейдерной программе
	  indices_vec.reserve(numIndices);

	  for (int z = 0; z < rows; ++z)
	  {
	    for (int x = 0; x < cols; ++x)
	    {
	      float xx, yy, zz;
      //вычисляем координаты каждой из вершин 
      	  switch(side){
	      	  case 0:
			      xx = -size / 2 + x*size / (cols - 1);
			      zz = -size / 2 + z*size / (rows - 1);
			      yy = size/2;
			      break;
		      case 1:
			      xx = -size / 2 + x*size / (cols - 1);
			      zz = size/2;
			      yy = -size / 2 + z*size / (rows - 1);
			      break;
			  case 2:
			      xx = -size / 2 + x*size / (cols - 1);
			      zz = -size/2;
			      yy = -size / 2 + z*size / (rows - 1);
			      break;
		      case 3:
			      xx = size/2;
			      zz = -size / 2 + z*size / (rows - 1);
			      yy = -size / 2 + x*size / (cols - 1);
			      break;
		      case 4:
			      xx = -size/2;
			      zz = -size / 2 + z*size / (rows - 1);
			      yy = -size / 2 + x*size / (cols - 1);
			      break;
		      case 5:
			      xx = -size / 2 + x*size / (cols - 1);
			      zz = -size / 2 + z*size / (rows - 1);
			      yy = -size/2;
			      break;
	  	  }

	      vertices_vec.push_back(xx);
	      vertices_vec.push_back(yy);
	      vertices_vec.push_back(zz);

	      texcoords_vec.push_back(x/float(cols-1)); // вычисляем первую текстурную координату u, для плоскости это просто относительное положение вершины
	      texcoords_vec.push_back(z/float(rows-1)); // аналогично вычисляем вторую текстурную координату v
	    }
	  }

	  //primitive restart - специальный индекс, который обозначает конец строки из треугольников в triangle_strip
	  //после этого индекса формирование треугольников из массива индексов начнется заново - будут взяты следующие 3 индекса для первого треугольника
	  //и далее каждый последующий индекс будет добавлять один новый треугольник пока снова не встретится primitive restart index

	  int primRestart = cols * rows;

	  for (int x = 0; x < cols - 1; ++x)
	  {
	    for (int z = 0; z < rows - 1; ++z)
	    {
	      int offset = x*cols + z;

	      //каждую итерацию добавляем по два треугольника, которые вместе формируют четырехугольник
	      if (z == 0) //если мы в начале строки треугольников, нам нужны первые четыре индекса
	      {
	        indices_vec.push_back(offset + 0);
	        indices_vec.push_back(offset + rows);
	        indices_vec.push_back(offset + 1);
	        indices_vec.push_back(offset + rows + 1);
	      }
	      else // иначе нам достаточно двух индексов, чтобы добавить два треугольника
	      {
	        indices_vec.push_back(offset + 1);
	        indices_vec.push_back(offset + rows + 1);

	        if (z == rows - 2) indices_vec.push_back(primRestart); // если мы дошли до конца строки, вставляем primRestart, чтобы обозначить переход на следующую строку
	      }
	    }
	  }

	  ///////////////////////
	  //формируем вектор граней(треугольников) по 3 индекса на каждый
	  int currFace = 1;
	  for (int i = 0; i < indices_vec.size() - 2; ++i)
	  {
	    int3 face;

	    int index0 = indices_vec.at(i);
	    int index1 = indices_vec.at(i + 1);
	    int index2 = indices_vec.at(i + 2);

	    if (index0 != primRestart && index1 != primRestart && index2 != primRestart)
	    {
	      if (currFace % 2 != 0) //если это нечетный треугольник, то индексы и так в правильном порядке обхода - против часовой стрелки
	      {
	        face.x = indices_vec.at(i);
	        face.y = indices_vec.at(i + 1);
	        face.z = indices_vec.at(i + 2);

	        currFace++;
	      }
	      else //если треугольник четный, то нужно поменять местами 2-й и 3-й индекс;
	      {    //при отрисовке opengl делает это за нас, но при расчете нормалей нам нужно это сделать самостоятельно
	        face.x = indices_vec.at(i);
	        face.y = indices_vec.at(i + 2);
	        face.z = indices_vec.at(i + 1);

	        currFace++;
	      }
	      faces.push_back(face);
	    }
	  }


	  ///////////////////////
	  //расчет нормалей
	  for (int i = 0; i < faces.size(); ++i)
	  {
	    //получаем из вектора вершин координаты каждой из вершин одного треугольника
	    float3 A(vertices_vec.at(3 * faces.at(i).x + 0), vertices_vec.at(3 * faces.at(i).x + 1), vertices_vec.at(3 * faces.at(i).x + 2));
	    float3 B(vertices_vec.at(3 * faces.at(i).y + 0), vertices_vec.at(3 * faces.at(i).y + 1), vertices_vec.at(3 * faces.at(i).y + 2));
	    float3 C(vertices_vec.at(3 * faces.at(i).z + 0), vertices_vec.at(3 * faces.at(i).z + 1), vertices_vec.at(3 * faces.at(i).z + 2));

	    //получаем векторы для ребер треугольника из каждой из 3-х вершин
	    float3 edge1A(normalize(B - A));
	    float3 edge2A(normalize(C - A));

	    float3 edge1B(normalize(A - B));
	    float3 edge2B(normalize(C - B));

	    float3 edge1C(normalize(A - C));
	    float3 edge2C(normalize(B - C));

	    //нормаль к треугольнику - векторное произведение любой пары векторов из одной вершины
	    float3 face_normal = cross(edge1A, edge2A);

	    //простой подход: нормаль к вершине = средняя по треугольникам, к которым принадлежит вершина
	    normals_vec_tmp.at(faces.at(i).x) += face_normal;
	    normals_vec_tmp.at(faces.at(i).y) += face_normal;
	    normals_vec_tmp.at(faces.at(i).z) += face_normal;
	  }

	  //нормализуем векторы нормалей и записываем их в вектор из GLFloat, который будет передан в шейдерную программу
	  for (int i = 0; i < normals_vec_tmp.size(); ++i)
	  {
	    float3 N = normalize(normals_vec_tmp.at(i));

	    normals_vec.push_back(N.x);
	    normals_vec.push_back(N.y);
	    normals_vec.push_back(N.z);
	  }

	  GLuint vboVertices, vboIndices;

	  glGenBuffers(1, &vboVertices);
	  glGenBuffers(1, &vboIndices);

	  glBindVertexArray(vao[side]); GL_CHECK_ERRORS;
	  {
	    //передаем в шейдерную программу атрибут координат вершин
	    glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
	    glBufferData(GL_ARRAY_BUFFER, vertices_vec.size() * sizeof(GL_FLOAT), &vertices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
	    glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

	    //передаем в шейдерную программу индексы
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_vec.size() * sizeof(GLuint), &indices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;

	    glEnable(GL_PRIMITIVE_RESTART); GL_CHECK_ERRORS;
	    glPrimitiveRestartIndex(primRestart); GL_CHECK_ERRORS;
	  }
	  glBindBuffer(GL_ARRAY_BUFFER, 0);

	  glBindVertexArray(0);
  }

  return numIndices;
}

static int land_water_load(int rows, int cols, float size, GLuint &vao, float** heights, int mode){

  int numIndices = 2 * cols*(rows - 1) + rows - 1;

  std::vector<GLfloat> vertices_vec; //вектор атрибута координат вершин
  vertices_vec.reserve(rows * cols * 3);

  std::vector<GLfloat> normals_vec; //вектор атрибута нормалей к вершинам
  normals_vec.reserve(rows * cols * 3);

  std::vector<GLfloat> texcoords_vec; //вектор атрибут текстурных координат вершин
  texcoords_vec.reserve(rows * cols * 2);

  std::vector<float3> normals_vec_tmp(rows * cols, float3(0.0f, 0.0f, 0.0f)); //временный вектор нормалей, используемый для расчетов

  std::vector<int3> faces;         //вектор граней (треугольников), каждая грань - три индекса вершин, её составляющих; используется для удобства расчета нормалей
  faces.reserve(numIndices / 3);

  std::vector<GLuint> indices_vec; //вектор индексов вершин для передачи шейдерной программе
  indices_vec.reserve(numIndices);

  for (int z = 0; z < rows; ++z)
  {
    for (int x = 0; x < cols; ++x)
    {
      //вычисляем координаты каждой из вершин 
      float xx = -size / 2 + x*size / (cols - 1);
      float zz = -size / 2 + z*size / (rows - 1);
      float yy = (mode ? sea_level : heights[x][z]);

      vertices_vec.push_back(xx);
      vertices_vec.push_back(yy);
      vertices_vec.push_back(zz);

      texcoords_vec.push_back((x % 2)); // вычисляем первую текстурную координату u, для плоскости это просто относительное положение вершины
      texcoords_vec.push_back((z % 2)); // аналогично вычисляем вторую текстурную координату v
    }
  }

  //primitive restart - специальный индекс, который обозначает конец строки из треугольников в triangle_strip
  //после этого индекса формирование треугольников из массива индексов начнется заново - будут взяты следующие 3 индекса для первого треугольника
  //и далее каждый последующий индекс будет добавлять один новый треугольник пока снова не встретится primitive restart index

  int primRestart = cols * rows;

  for (int x = 0; x < cols - 1; ++x)
  {
    for (int z = 0; z < rows - 1; ++z)
    {
      int offset = x*cols + z;

      //каждую итерацию добавляем по два треугольника, которые вместе формируют четырехугольник
      if (z == 0) //если мы в начале строки треугольников, нам нужны первые четыре индекса
      {
        indices_vec.push_back(offset + 0);
        indices_vec.push_back(offset + rows);
        indices_vec.push_back(offset + 1);
        indices_vec.push_back(offset + rows + 1);
      }
      else // иначе нам достаточно двух индексов, чтобы добавить два треугольника
      {
        indices_vec.push_back(offset + 1);
        indices_vec.push_back(offset + rows + 1);

        if (z == rows - 2) indices_vec.push_back(primRestart); // если мы дошли до конца строки, вставляем primRestart, чтобы обозначить переход на следующую строку
      }
    }
  }

  ///////////////////////
  //формируем вектор граней(треугольников) по 3 индекса на каждый
  int currFace = 1;
  for (int i = 0; i < indices_vec.size() - 2; ++i)
  {
    int3 face;

    int index0 = indices_vec.at(i);
    int index1 = indices_vec.at(i + 1);
    int index2 = indices_vec.at(i + 2);

    if (index0 != primRestart && index1 != primRestart && index2 != primRestart)
    {
      if (currFace % 2 != 0) //если это нечетный треугольник, то индексы и так в правильном порядке обхода - против часовой стрелки
      {
        face.x = indices_vec.at(i);
        face.y = indices_vec.at(i + 1);
        face.z = indices_vec.at(i + 2);

        currFace++;
      }
      else //если треугольник четный, то нужно поменять местами 2-й и 3-й индекс;
      {    //при отрисовке opengl делает это за нас, но при расчете нормалей нам нужно это сделать самостоятельно
        face.x = indices_vec.at(i);
        face.y = indices_vec.at(i + 2);
        face.z = indices_vec.at(i + 1);

        currFace++;
      }
      faces.push_back(face);
    }
  }


  ///////////////////////
  //расчет нормалей
  for (int i = 0; i < faces.size(); ++i)
  {
    //получаем из вектора вершин координаты каждой из вершин одного треугольника
    float3 A(vertices_vec.at(3 * faces.at(i).x + 0), vertices_vec.at(3 * faces.at(i).x + 1), vertices_vec.at(3 * faces.at(i).x + 2));
    float3 B(vertices_vec.at(3 * faces.at(i).y + 0), vertices_vec.at(3 * faces.at(i).y + 1), vertices_vec.at(3 * faces.at(i).y + 2));
    float3 C(vertices_vec.at(3 * faces.at(i).z + 0), vertices_vec.at(3 * faces.at(i).z + 1), vertices_vec.at(3 * faces.at(i).z + 2));

    //получаем векторы для ребер треугольника из каждой из 3-х вершин
    float3 edge1A(normalize(B - A));
    float3 edge2A(normalize(C - A));

    float3 edge1B(normalize(A - B));
    float3 edge2B(normalize(C - B));

    float3 edge1C(normalize(A - C));
    float3 edge2C(normalize(B - C));

    //нормаль к треугольнику - векторное произведение любой пары векторов из одной вершины
    float3 face_normal = cross(edge1A, edge2A);

    //простой подход: нормаль к вершине = средняя по треугольникам, к которым принадлежит вершина
    normals_vec_tmp.at(faces.at(i).x) += face_normal;
    normals_vec_tmp.at(faces.at(i).y) += face_normal;
    normals_vec_tmp.at(faces.at(i).z) += face_normal;
  }

  //нормализуем векторы нормалей и записываем их в вектор из GLFloat, который будет передан в шейдерную программу
  for (int i = 0; i < normals_vec_tmp.size(); ++i)
  {
    float3 N = normalize(normals_vec_tmp.at(i));

    normals_vec.push_back(N.x);
    normals_vec.push_back(N.y);
    normals_vec.push_back(N.z);
  }

  GLuint vboVertices, vboIndices, vboNormals, vboTexCoords;
  GLuint sand_textureID, grass_textureID, cube_textureID, water_textureID;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vboVertices);
  glGenBuffers(1, &vboIndices);
  glGenBuffers(1, &vboNormals);
  glGenBuffers(1, &vboTexCoords);

  sand_textureID = tex_load("sand.jpg");
  grass_textureID = tex_load("grass.jpeg");
  water_textureID = tex_load("water.jpg");
  cube_textureID = cube_tex_load(); 

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sand_textureID);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, grass_textureID);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, water_textureID);
 
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cube_textureID);

  glBindVertexArray(vao); GL_CHECK_ERRORS;
  {

    //передаем в шейдерную программу атрибут координат вершин
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, vertices_vec.size() * sizeof(GL_FLOAT), &vertices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

    //передаем в шейдерную программу атрибут нормалей
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, normals_vec.size() * sizeof(GL_FLOAT), &normals_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(1); GL_CHECK_ERRORS;

    //передаем в шейдерную программу атрибут текстурных координат
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, texcoords_vec.size() * sizeof(GL_FLOAT), &texcoords_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(2); GL_CHECK_ERRORS;

    //передаем в шейдерную программу индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_vec.size() * sizeof(GLuint), &indices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;

    glEnable(GL_PRIMITIVE_RESTART); GL_CHECK_ERRORS;
    glPrimitiveRestartIndex(primRestart); GL_CHECK_ERRORS;
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);


  return numIndices;
}

static int gen_heights_load_land(int rows, int cols, float size, GLuint &vao){
	float** heights = new float*[rows];

	for(int i = 0; i < rows; i++)
		heights[i] = new float[cols];

	heights[0][0]           = rand() % 10;
	heights[rows-1][0]      = rand() % 10;
	heights[0][cols-1]      = rand() % 10;
	heights[rows-1][cols-1] = rand() % 10;

	int side = rows - 1;

	while(side != 1){

		for(int i = 0; i < rows/side; i++)
			for(int j = 0; j < cols/side; j++)
				heights[i*side + side/2][j*side + side/2] = (heights[i*side][j*side] + 
															 heights[i*side][(j+1)*side] + 
															 heights[(i+1)*side][j*side] + 
															 heights[(i+1)*side][(j+1)*side]
															)/4 + rand() % (2*sharpness*side) - sharpness*side;

		for(int i = 0; i < rows/side; i++)
			for(int j = 0; j < cols/side; j++){
				heights[i*side+side/2][j*side]     = (heights[i*side][j*side] + 
					                                  heights[(i+1)*side][j*side] + 
					                                  heights[i*side + side/2][j*side + side/2] + 
					                                  (j == 0 ? 0 : heights[i*side+side/2][j*side-side/2])
					                                 )/(j==0?3:4) + rand() % (2*sharpness*side) - sharpness*side;

				heights[i*side+side/2][(j+1)*side] = (heights[i*side][(j+1)*side] + 
					                                  heights[(i+1)*side][(j+1)*side] + 
					                                  heights[i*side + side/2][j*side + side/2] + 
					                                  (j == cols/side-1 ? 0 : heights[i*side+side/2][j*side+3*side/2])
					                                 )/(j==cols/side-1?3:4) + rand() % (2*sharpness*side) - sharpness*side;

				heights[i*side][j*side+side/2]     = (heights[i*side][j*side] + 
					                                  heights[i*side][(j+1)*side] + 
					                                  heights[i*side + side/2][j*side + side/2] + 
					                                  (i == 0 ? 0 : heights[i*side-side/2][j*side+side/2])
					                                 )/(i==0?3:4) + rand() % (2*sharpness*side) - sharpness*side;

				heights[(i+1)*side][j*side+side/2] = (heights[(i+1)*side][j*side] + 
					                                  heights[(i+1)*side][(j+1)*side] + 
					                                  heights[i*side + side/2][j*side + side/2] + 
					                                  (i == rows/side-1 ? 0 : heights[i*side+3*side/2][j*side+side/2])
					                                 )/(i==rows/side-1?3:4) + rand() % (2*sharpness*side) - sharpness*side;
			}

		side /= 2;
	}

	float min = heights[0][0]; 
	float max = heights[0][0];

	for(int i = 0; i < rows; i++)
		for(int j = 0; j < cols; j++){
			if(heights[i][j] > max)
				max = heights[i][j];
			if(heights[i][j] < min)
				min = heights[i][j];
		}

	for(int i = 0; i < rows; i++)
		for(int j = 0; j < cols; j++)
			heights[i][j] = 4*((heights[i][j] - min)/(max - min))*((heights[i][j] - min)/(max - min));

	int numIndices = land_water_load(rows, cols, size, vao, heights, 0);

	for(int i = 0; i < rows; i++)
		delete [] heights[i];
	delete[] heights;

	return numIndices;
}

int initGL()
{
	int res = 0;

	//грузим функции opengl через glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	//выводим в консоль некоторую информацию о драйвере и контексте opengl
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press left mose button to capture/release mouse cursor  "<< std::endl;
  std::cout << "press spacebar to alternate between shaded wireframe and fill display modes" << std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

int main(int argc, char** argv)
{
	std::srand(0);

	if(!glfwInit())
    return -1;

	//запрашиваем контекст opengl версии 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 


  GLFWwindow*  window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL basic sample", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	//регистрируем коллбеки для обработки сообщений от пользователя - клавиатура, мышь..
	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);
	glfwSetInputMode          (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

	if(initGL() != 0) 
		return -1;
	
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	//создание шейдерной программы из двух файлов с исходниками шейдеров
	//используется класс-обертка ShaderProgram
	std::unordered_map<GLenum, std::string> land_shaders;
	land_shaders[GL_VERTEX_SHADER]   = "vertex_land.glsl";
	land_shaders[GL_FRAGMENT_SHADER] = "fragment_land.glsl";
	ShaderProgram program_land(land_shaders); GL_CHECK_ERRORS;

	std::unordered_map<GLenum, std::string> water_shaders;
	water_shaders[GL_VERTEX_SHADER]   = "vertex_water.glsl";
	water_shaders[GL_FRAGMENT_SHADER] = "fragment_water.glsl";
	ShaderProgram program_water(water_shaders); GL_CHECK_ERRORS;

	std::unordered_map<GLenum, std::string> sky_shaders;
	sky_shaders[GL_VERTEX_SHADER]   = "vertex_sky.glsl";
	sky_shaders[GL_FRAGMENT_SHADER] = "fragment_sky.glsl";
	ShaderProgram program_sky(sky_shaders); GL_CHECK_ERRORS;

	std::unordered_map<GLenum, std::string> normal_arrows_shaders;
	normal_arrows_shaders[GL_VERTEX_SHADER]   = "vertex_norm_arrows.glsl";
	normal_arrows_shaders[GL_FRAGMENT_SHADER] = "fragment_norm_arrows.glsl";
	normal_arrows_shaders[GL_GEOMETRY_SHADER] = "geometry_norm_arrows.glsl";
	ShaderProgram program_norm_arrows(normal_arrows_shaders); GL_CHECK_ERRORS;

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  
  //Создаем и загружаем геометрию поверхности
  GLuint* vaoSky = new GLuint[6];
  int skyIndices = sky_load(2, 2, size, vaoSky);

  GLuint vaoWater;
  int waterIndices = land_water_load(129, 129, size, vaoWater, NULL,  1);

  GLuint vaoTriStrip;
  int triStripIndices = gen_heights_load_land(129, 129, size, vaoTriStrip);

  glViewport(0, 0, WIDTH, HEIGHT);  GL_CHECK_ERRORS;
  glEnable(GL_DEPTH_TEST);  GL_CHECK_ERRORS;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//цикл обработки сообщений и отрисовки сцены каждый кадр
	while (!glfwWindowShouldClose(window))
	{
		//считаем сколько времени прошло за кадр
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
    doCameraMovement(camera, deltaTime);

		//очищаем экран каждый кадр
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); GL_CHECK_ERRORS;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;

		//обновляем матрицы камеры и проекции каждый кадр
    float4x4 view       = camera.GetViewMatrix();
    float4x4 projection = projectionMatrixTransposed(camera.zoom, float(WIDTH) / float(HEIGHT), 0.1f, 1000.0f);

		                //модельная матрица, определяющая положение объекта в мировом пространстве
		float4x4 model; //начинаем с единичной матрицы
		
    program_land.StartUseShader();

    //загружаем uniform-переменные в шейдерную программу (одинаковые для всех параллельно запускаемых копий шейдера)
    program_land.SetUniform("view",       view);       GL_CHECK_ERRORS;
    program_land.SetUniform("projection", projection); GL_CHECK_ERRORS;
    program_land.SetUniform("model",      model);	   GL_CHECK_ERRORS;
    
    program_land.SetUniform("texture_sand", 0);        GL_CHECK_ERRORS;
    program_land.SetUniform("texture_grass", 1);       GL_CHECK_ERRORS;
    program_land.SetUniform("norm", norm);             GL_CHECK_ERRORS;
    program_land.SetUniform("light_src", light_src);   GL_CHECK_ERRORS;
   
    //рисуем плоскость
    glBindVertexArray(vaoTriStrip);
    glDrawElements(GL_TRIANGLE_STRIP, triStripIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS

    program_land.StopUseShader();

    program_water.StartUseShader();

    //загружаем uniform-переменные в шейдерную программу (одинаковые для всех параллельно запускаемых копий шейдера)
    program_water.SetUniform("view",       view);       GL_CHECK_ERRORS;
    program_water.SetUniform("projection", projection); GL_CHECK_ERRORS;
    program_water.SetUniform("model",      model);	    GL_CHECK_ERRORS;

    program_water.SetUniform("texture_water", 2);	    GL_CHECK_ERRORS;
    program_water.SetUniform("cube_texture", 3);        GL_CHECK_ERRORS;
    program_water.SetUniform("light_src", light_src);   GL_CHECK_ERRORS;

    //рисуем плоскость
    glBindVertexArray(vaoWater);
    glDrawElements(GL_TRIANGLE_STRIP, waterIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;

    program_water.StopUseShader();

    program_sky.StartUseShader();

    float4x4 model_sky;

    model_sky.M(0, 3) = camera.pos.x;
    model_sky.M(1, 3) = camera.pos.y;
    model_sky.M(2, 3) = camera.pos.z;

    GLint oldCullMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &oldCullMode);
    GLint oldDepthMode;
    glGetIntegerv(GL_DEPTH_FUNC, &oldDepthMode);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    //загружаем uniform-переменные в шейдерную программу (одинаковые для всех параллельно запускаемых копий шейдера)
    program_sky.SetUniform("view",       view);       GL_CHECK_ERRORS;
    program_sky.SetUniform("projection", projection); GL_CHECK_ERRORS;
    program_sky.SetUniform("model",      model_sky);  GL_CHECK_ERRORS;
    
    program_sky.SetUniform("cube_texture", 3);        GL_CHECK_ERRORS;

    for(int i = 0; i < 6; i++){
	    glBindVertexArray(vaoSky[i]);
	    glDrawElements(GL_TRIANGLE_STRIP, skyIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
	    glBindVertexArray(0); GL_CHECK_ERRORS;
	}

    glCullFace(oldCullMode);
    glDepthFunc(oldDepthMode);

    program_sky.StopUseShader();

    if(norm){
	    program_norm_arrows.StartUseShader();

	    program_norm_arrows.SetUniform("view",       view);       GL_CHECK_ERRORS;
	    program_norm_arrows.SetUniform("projection", projection); GL_CHECK_ERRORS;
	    program_norm_arrows.SetUniform("model",      model);	  GL_CHECK_ERRORS;

	    glBindVertexArray(vaoTriStrip);
	    glDrawElements(GL_TRIANGLE_STRIP, triStripIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
	    glBindVertexArray(0); GL_CHECK_ERRORS;

	    program_norm_arrows.StopUseShader();
	}

		glfwSwapBuffers(window); 
	}

	//очищаем vao перед закрытием программы
	glDeleteVertexArrays(1, &vaoTriStrip);
	glDeleteVertexArrays(1, &vaoWater);
	glDeleteVertexArrays(6, vaoSky);

	glfwTerminate();
	return 0;
}
