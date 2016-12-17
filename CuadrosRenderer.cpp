#include <iostream>

#include "CuadrosRenderer.h"
#include "twodee/NField.h"
#include "twodee/QVector4.h"
#include "twodee/QMath.h"
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
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f
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
    "uniform mat4 modelview;\n"
    "\n"
    "in vec3 position;\n"
    "in vec2 texcoords;\n"
    "\n"
    "out vec2 ftexcoords;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = projection * modelview * vec4(position, 1.0);\n"
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

  /* texture->Upload(32, 32, texels); */

  image = new NField<unsigned char, 2>(path);
  if (image->GetChannelCount() == 1) {
    texture->Channels(Texture::GRAYSCALE);
  } else if (image->GetChannelCount() == 3) {
    texture->Channels(Texture::RGB);
  } else if (image->GetChannelCount() == 4) {
    texture->Channels(Texture::RGBA);
  } else {
    std::cout << "no!!!!!!!!!!" << std::endl;
  }
  texture->Upload(image->GetDimensions()[0], image->GetDimensions()[1], image->GetData());
  setInterpolation(INTERPOLATION_NEAREST);

  program->Bind();
  program->SetUniform("tex", *texture);
  program->Unbind();

  OpenGL::CheckError("after initialize");

  modelview = QMatrix4<float>(1.0f);
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::resize(int width, int height) {
  glViewport(0, 0, width, height);  
  dimensions[0] = width;
  dimensions[1] = height;

  float window_aspect = width / (float) height;
  float image_aspect = image->GetDimensions()[0] / (float) image->GetDimensions()[1];

  if (window_aspect < image_aspect) {
    projection = QMatrix4<float>::GetOrthographic(-1.0f, 1.0f, -1.0f / window_aspect * image_aspect, 1.0f / window_aspect * image_aspect);
  } else {
    projection = QMatrix4<float>::GetOrthographic(-1.0f * window_aspect / image_aspect, 1.0f * window_aspect / image_aspect, -1.0f, 1.0f);
  }
  std::cout << "projection: " << projection << std::endl;
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  program->Bind();
  program->SetUniform("projection", projection);
  program->SetUniform("modelview", modelview);
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
  /* std::cout << "x: " << x << std::endl; */
  /* std::cout << "y: " << y << std::endl; */
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::leftMouseDraggedTo(int x, int y) {
  float half_width = 0.5f * dimensions[0];
  float half_height = 0.5f * dimensions[1];
  float half_image_width = 0.5f * image->GetDimensions()[0];
  float half_image_height = 0.5f * image->GetDimensions()[1];
  float image_aspect = image->GetDimensions()[0] / (float) image->GetDimensions()[1];
  float window_aspect = dimensions[0] / (float) dimensions[1];

  QVector4<float> p((x - half_width) / half_width, (y - half_height) / half_height, 0.0f, 1.0f);

  if (window_aspect < image_aspect) {
    p[1] *= image_aspect / window_aspect;
  } else {
    p[0] /= image_aspect * window_aspect;
  }
  std::cout << "p: " << p << std::endl;

  p[0] = half_image_width * p[0] + half_image_width;
  p[1] = half_image_height * p[1] + half_image_height;
  std::cout << "p: " << p << std::endl;

  QVector2<int> pixel((int) p[0], (int) p[1]);
  if (pixel[0] >= 0 && pixel[0] < image->GetDimensions()[0] &&
      pixel[1] >= 0 && pixel[1] < image->GetDimensions()[1]) {
    (*image)(pixel)[0] = 0;
    (*image)(pixel)[1] = 0;
    (*image)(pixel)[2] = 0;
    texture->Upload(image->GetDimensions()[0], image->GetDimensions()[1], image->GetData());
  }
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::leftMouseUpAt(int x, int y) {
  /* std::cout << "x: " << x << std::endl; */
  /* std::cout << "y: " << y << std::endl; */
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::rightMouseDownAt(int x, int y) {
  mouse_at[0] = x;
  mouse_at[1] = y;
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::rightMouseDraggedTo(int x, int y) {
  modelview = QMatrix4<float>::GetTranslate((x - mouse_at[0]) / (float) dimensions[0] * 2, (y - mouse_at[1]) / (float) dimensions[1] * 2, 0.0f) * modelview;
  mouse_at[0] = x;
  mouse_at[1] = y;
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::scroll(int nTicks) {
  if (nTicks != 0) {
    float factor = 1 + nTicks / 100.0f;;
    std::cout << "nTicks: " << nTicks << std::endl;
    modelview = QMatrix4<float>::GetScale(factor, factor, 1.0f) * modelview;
  }
}

/* ------------------------------------------------------------------------- */

