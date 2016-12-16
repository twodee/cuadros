#include <iostream>

#include "CuadrosRenderer.h"
#include "twodee/NField.h"
#include "twodee/UtilitiesOpenGL.h"

using namespace td;

/* ------------------------------------------------------------------------- */

CuadrosRenderer::CuadrosRenderer() :
  attributes(NULL),
  array(NULL),
  program(NULL),
  image(NULL),
  interpolation_mode(1) {
}

/* ------------------------------------------------------------------------- */

CuadrosRenderer::~CuadrosRenderer() {
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::initializeGL() {
  glClearColor(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f);

  float positions[] = {
    -0.95f, -0.95f, 0.0f,
    0.95f, -0.95f, 0.0f,
    -0.95f, 0.95f, 0.0f,
    0.95f, 0.95f, 0.0f
  };

  float texcoords[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
  };

  attributes = new VertexAttributes();
  attributes->AddAttribute("position", 4, 3, positions);
  attributes->AddAttribute("texcoords", 4, 2, texcoords);
  OpenGL::CheckError("after attributes");

  std::string vertex_source =
    "#version 410\n"
    "\n"
    "uniform mat4 projection;\n"
    "\n"
    "in vec3 position;\n"
    "in vec2 texcoords;\n"
    "\n"
    "out vec2 ftexcoords;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = projection * vec4(position, 1.0);\n"
    "  ftexcoords = texcoords;\n"
    "}\n";

  std::string fragment_source =
    "#version 410\n"
    "\n"
    "uniform sampler2D tex;\n"
    "\n"
    "in vec2 ftexcoords;\n"
    "\n"
    "out vec4 frag_color;\n"
    "\n"
    "void main() {\n"
    "  frag_color = texture(tex, ftexcoords);\n"
    "}\n";

  program = ShaderProgram::FromSource(vertex_source, fragment_source);
  OpenGL::CheckError("after program");

  array = new VertexArray(*program, *attributes);
  OpenGL::CheckError("after array");

  texture = new Texture(0);
  /* float texels[32 * 32]; */
  /* for (int r = 0; r < 32; ++r) { */
    /* for (int c = 0; c < 32; ++c) { */
      /* texels[r * 32 + c] = r % 2 == 0 ? 1.0f : 0.0f; */
    /* } */
  /* } */


  /* texture->Channels(Texture::GRAYSCALE); */
  /* texture->Upload(32, 32, texels); */

  image = new NField<float, 2>(path);
  texture->Channels(Texture::RGB);
  texture->Upload(image->GetDimensions()[0], image->GetDimensions()[1], image->GetData());
  setInterpolation(INTERPOLATION_NEAREST);

  program->Bind();
  program->SetUniform("tex", *texture);
  program->Unbind();

  OpenGL::CheckError("after initialize");
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::resize(int width, int height) {
  glViewport(0, 0, width, height);  

  float window_aspect = width / (float) height;
  float image_aspect = image->GetDimensions()[0] / (float) image->GetDimensions()[1];

  if (window_aspect < image_aspect) {
    projection = QMatrix4<float>::GetOrthographic(-1.0f, 1.0f, -1.0f / window_aspect * image_aspect, 1.0f / window_aspect * image_aspect);
  } else {
    projection = QMatrix4<float>::GetOrthographic(-1.0f * window_aspect / image_aspect, 1.0f * window_aspect / image_aspect, -1.0f, 1.0f);
  }
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  program->Bind();
  program->SetUniform("projection", projection);
  texture->Bind();
  array->Bind();
  array->DrawSequence(GL_TRIANGLE_STRIP);
  array->Unbind();
  texture->Unbind();
  program->Unbind();

  OpenGL::CheckError("after render");
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::show(const std::string &path) {
  this->path = path;
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::setInterpolation(int mode) {
  interpolation_mode = mode; 
  if (interpolation_mode == INTERPOLATION_LINEAR) {
    texture->Magnify(Texture::MAGNIFY_LINEAR);
    texture->Minify(Texture::MINIFY_LINEAR);
  } else {
    texture->Magnify(Texture::MAGNIFY_NEAREST);
    texture->Minify(Texture::MINIFY_NEAREST);
  }
  texture->SetParameters();
}

/* ------------------------------------------------------------------------- */

int CuadrosRenderer::getInterpolation() const {
  return interpolation_mode; 
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::leftMouseDownAt(int x, int y) {
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::leftMouseDraggedTo(int x, int y) {
  
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::leftMouseUpAt(int x, int y) {
  
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::scroll(int nTicks) {
  std::cout << "nTicks: " << nTicks << std::endl;
}

/* ------------------------------------------------------------------------- */

