#include <iostream>

#include "CuadrosRenderer.h"
#include "twodee/UtilitiesOpenGL.h"

using namespace td;

/* ------------------------------------------------------------------------- */

CuadrosRenderer::CuadrosRenderer() :
  attributes(NULL),
  array(NULL),
  program(NULL) {
}

/* ------------------------------------------------------------------------- */

CuadrosRenderer::~CuadrosRenderer() {
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::initializeGL() {
  glClearColor(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f);

  float positions[] = {
    -0.9f, -0.9f, 0.0f,
    0.9f, -0.9f, 0.0f,
    -0.9f, 0.9f, 0.0f,
    0.9f, 0.9f, 0.0f
  };

  attributes = new VertexAttributes();
  attributes->AddAttribute("position", 4, 3, positions);
  OpenGL::CheckError("after attributes");

  std::string vertex_source =
    "#version 410\n"
    "in vec3 position;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = vec4(position, 1.0);\n"
    "}\n";

  std::string fragment_source =
    "#version 410\n"
    "out vec4 frag_color;\n"
    "\n"
    "void main() {\n"
    "  frag_color = vec4(0.0, 1.0, 1.0, 1.0);\n"
    "}\n";

  program = ShaderProgram::FromSource(vertex_source, fragment_source);
  OpenGL::CheckError("after program");

  array = new VertexArray(*program, *attributes);
  OpenGL::CheckError("after array");

  OpenGL::CheckError("after initialize");
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::resize(int width, int height) {
  glViewport(0, 0, width, height);  
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  program->Bind();
  array->Bind();
  array->DrawSequence(GL_TRIANGLE_STRIP);
  array->Unbind();
  program->Unbind();

  OpenGL::CheckError("after render");
}

/* ------------------------------------------------------------------------- */

