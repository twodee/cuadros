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

void CuadrosRenderer::initializeBackground() {
  float positions[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f
  };

  background_attributes = new VertexAttributes();
  background_attributes->AddAttribute("position", 4, 3, positions);
  OpenGL::CheckError("after bg attributes");

  std::string vertex_source =
    "#version 410\n"
    "\n"
    "in vec3 position;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = vec4(position, 1.0);\n"
    "}\n";

  std::string fragment_source =
    "#version 410\n"
    "\n"
    "const int TILE_SIZE = 10;\n"
    "const vec4 LIGHT = vec4(vec3(0.8), 1.0);\n"
    "const vec4 DARK = vec4(vec3(0.6), 1.0);\n"
    "\n"
    "out vec4 frag_color;\n"
    "\n"
    "void main() {\n"
    "  float h = mod(floor(gl_FragCoord.x / TILE_SIZE), 2);\n"
    "  float v = mod(floor(gl_FragCoord.y / TILE_SIZE), 2);\n"
    "  float gray = abs(h - v);\n"
    "  frag_color = mix(LIGHT, DARK, gray);\n"
    "}\n";

  background_program = ShaderProgram::FromSource(vertex_source, fragment_source);
  OpenGL::CheckError("after bg program");

  background_array = new VertexArray(*background_program, *background_attributes);
  OpenGL::CheckError("after bg array");
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::initializeGL() {
  glClearColor(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f);

  initializeBackground();

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

  background_program->Bind();
  background_array->Bind();
  background_array->DrawSequence(GL_TRIANGLE_STRIP);
  background_array->Unbind();
  background_program->Unbind();

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
  image = new NField<unsigned char, 2>(path);
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::show(int width, int height) {
  image = new NField<unsigned char, 2>(QVector2<int>(width, height));
  image->Clear(128);
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

bool CuadrosRenderer::isMouseOverImage(int x, int y) const {
  QVector2<int> pixel = mouseToImage(x, y);
  return pixel[0] >= 0 && pixel[0] < image->GetDimensions()[0] &&
         pixel[1] >= 0 && pixel[1] < image->GetDimensions()[1];
}

/* ------------------------------------------------------------------------- */

td::QVector2<int> CuadrosRenderer::mouseToImage(int x, int y) const {
  float half_width = 0.5f * dimensions[0];
  float half_height = 0.5f * dimensions[1];
  float half_image_width = 0.5f * image->GetDimensions()[0];
  float half_image_height = 0.5f * image->GetDimensions()[1];
  float image_aspect = image->GetDimensions()[0] / (float) image->GetDimensions()[1];
  float window_aspect = dimensions[0] / (float) dimensions[1];

  QVector4<float> p((x - half_width) / half_width, (y - half_height) / half_height, 0.0f, 1.0f);

  if (window_aspect < image_aspect) {
    p[1] = p[1] * image_aspect / window_aspect;
  } else {
    p[0] = p[0] * window_aspect / image_aspect;
  }

  p = modelview.GetInverse() * p;

  p[0] = half_image_width * p[0] + half_image_width;
  p[1] = half_image_height * p[1] + half_image_height;

  QVector2<int> pixel((int) p[0], (int) p[1]);

  return pixel;
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::leftMouseDraggedTo(int x, int y) {
  QVector2<int> pixel = mouseToImage(x, y);
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
    modelview = QMatrix4<float>::GetScale(factor, factor, 1.0f) * modelview;
  }
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::saveAs(const std::string &path) const {
  image->Write(path);
}

/* ------------------------------------------------------------------------- */

