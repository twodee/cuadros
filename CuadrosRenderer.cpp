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
  frames(),
  interpolation_mode(INTERPOLATION_NEAREST),
  scale(1.0f),
  rgb(0, 0, 0, 255),
  mode(RECTANGLE_SELECT),
  has_selection(false) {
  frame_index = -1;
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

  // The background's origin should start at the top left. Otherwise it will
  // shift down when the window is resized, which is distracting and awkward.
  // We flip the y in -1:1 space, and then rangemap to [0, 1].
  std::string vertex_source =
    "#version 410\n"
    "\n"
    "uniform vec2 resolution;\n"
    "\n"
    "in vec3 position;\n"
    "\n"
    "out vec2 ftexcoords;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = vec4(position, 1.0);\n"
    "  ftexcoords = (vec2(position.x, -position.y) + 1.0) * 0.5 * resolution;\n"
    "}\n";

  std::string fragment_source =
    "#version 410\n"
    "\n"
    "uniform sampler2D background;\n"
    "\n"
    "in vec2 ftexcoords;\n"
    "\n"
    "out vec4 frag_color;\n"
    "\n"
    "void main() {\n"
    "  frag_color = vec4(vec3(texture(background, ftexcoords).r), 1.0);\n"
    "}\n";

  background_program = ShaderProgram::FromSource(vertex_source, fragment_source);
  OpenGL::CheckError("after bg program");

  background_array = new VertexArray(*background_program, *background_attributes);
  OpenGL::CheckError("after bg array");

  background_texture = new Texture(1);
  float texels[] = {
    0.8f, 0.6f,
    0.6f, 0.8f
  };
  background_texture->Channels(Texture::GRAYSCALE);
  background_texture->Magnify(Texture::MAGNIFY_NEAREST);
  background_texture->Minify(Texture::MINIFY_NEAREST);
  background_texture->Wrap(Texture::REPEAT);
  background_texture->Upload(2, 2, texels);

  background_program->Bind();
  background_program->SetUniform("background", *background_texture);
  background_program->Unbind();
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::initializeImage() {
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
    /* "  frag_color = vec4(ftexcoords, 0.0, 1.0);\n" */
    "  frag_color = texture(tex, ftexcoords);\n"
    "}\n";

  program = ShaderProgram::FromSource(vertex_source, fragment_source);
  OpenGL::CheckError("after program");

  array = new VertexArray(*program, *attributes);
  OpenGL::CheckError("after array");

  texture = new Texture(0);

  if (frames[frame_index]->GetChannelCount() == 1) {
    texture->Channels(Texture::GRAYSCALE);
  } else if (frames[frame_index]->GetChannelCount() == 3) {
    std::cout << "3" << std::endl;
    texture->Channels(Texture::RGB);
  } else if (frames[frame_index]->GetChannelCount() == 4) {
    std::cout << "4" << std::endl;
    texture->Channels(Texture::RGBA);
  } else {
    std::cout << "no!!!!!!!!!!" << std::endl;
  }
  texture->Wrap(Texture::REPEAT);
  texture->Upload(frames[frame_index]->GetDimensions()[0], frames[frame_index]->GetDimensions()[1], frames[frame_index]->GetData());
  setInterpolation(INTERPOLATION_NEAREST);

  program->Bind();
  program->SetUniform("tex", *texture);
  program->Unbind();

  OpenGL::CheckError("after initialize");
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::initializeLasso() {
  float positions[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    1.0f, 1.0f,
    -1.0f, 1.0f
  };

  lasso_attributes = new VertexAttributes();
  lasso_attributes->AddAttribute("position", 4, 2, positions);
  OpenGL::CheckError("after attributes");

  std::string vertex_source =
    "#version 410\n"
    "\n"
    "uniform mat4 projection;\n"
    "uniform mat4 modelview;\n"
    "\n"
    "in vec3 position;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = projection * modelview * vec4(position, 1.0);\n"
    "}\n";

  std::string fragment_source =
    "#version 410\n"
    "\n"
    "out vec4 frag_color;\n"
    "\n"
    "void main() {\n"
    "  frag_color = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "}\n";

  lasso_program = ShaderProgram::FromSource(vertex_source, fragment_source);
  OpenGL::CheckError("after program");

  lasso_array = new VertexArray(*lasso_program, *lasso_attributes);
  OpenGL::CheckError("after array");
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::initializeGL() {
  glClearColor(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  initializeBackground();
  initializeImage();
  initializeLasso();

  modelview = QMatrix4<float>(1.0f);
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::resize(int width, int height) {
  glViewport(0, 0, width, height);  
  dimensions[0] = width;
  dimensions[1] = height;

  float window_aspect = width / (float) height;
  float image_aspect = frames[frame_index]->GetDimensions()[0] / (float) frames[frame_index]->GetDimensions()[1];

  if (window_aspect < image_aspect) {
    projection = QMatrix4<float>::GetOrthographic(-1.0f, 1.0f, -1.0f / window_aspect * image_aspect, 1.0f / window_aspect * image_aspect);
  } else {
    projection = QMatrix4<float>::GetOrthographic(-1.0f * window_aspect / image_aspect, 1.0f * window_aspect / image_aspect, -1.0f, 1.0f);
  }
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const float TILE_SIZE = 10.0f;

  background_program->Bind();
  background_program->SetUniform("resolution", dimensions[0] / (2 * TILE_SIZE), dimensions[1] / (2 * TILE_SIZE));
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

  lasso_program->Bind();
  lasso_program->SetUniform("projection", projection);
  lasso_program->SetUniform("modelview", modelview);
  lasso_array->Bind();
  lasso_array->DrawSequence(GL_LINE_LOOP);
  lasso_array->Unbind();
  lasso_program->Unbind();

  OpenGL::CheckError("after render");
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::show(const std::string &path) {
  this->path = path;
  frame_index = 0;
  frames.push_back(new NField<unsigned char, 2>(path));
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::show(int width, int height) {
  frame_index = 0;
  frames.push_back(new NField<unsigned char, 2>(QVector2<int>(width, height), 3));
  frames[0]->Clear(255);
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
  if (mode == DRAW) {
    fill(x, y);
  } else if (mode == RECTANGLE_SELECT) {
    rectangle1 = mouseToImage(x, y);
    rectangle2 = rectangle1;
    updateLasso();
  }
  mouse_at[0] = x;
  mouse_at[1] = y;
}

/* ------------------------------------------------------------------------- */ 

bool CuadrosRenderer::isMouseOverImage(int x, int y) const {
  QVector2<int> pixel = mouseToImage(x, y);
  return pixel[0] >= 0 && pixel[0] < frames[frame_index]->GetDimensions()[0] &&
         pixel[1] >= 0 && pixel[1] < frames[frame_index]->GetDimensions()[1];
}

/* ------------------------------------------------------------------------- */

td::QVector2<int> CuadrosRenderer::mouseToImage(int x, int y) const {
  float half_width = 0.5f * dimensions[0];
  float half_height = 0.5f * dimensions[1];
  float half_image_width = 0.5f * frames[frame_index]->GetDimensions()[0];
  float half_image_height = 0.5f * frames[frame_index]->GetDimensions()[1];
  float image_aspect = frames[frame_index]->GetDimensions()[0] / (float) frames[frame_index]->GetDimensions()[1];
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
  if (mode == RECTANGLE_SELECT) {
    has_selection = true;
    rectangle2 = mouseToImage(x, y);
    updateLasso();
  } else if (mode == DRAW) {
    fill(mouse_at[0], mouse_at[1], x, y);
  }
  mouse_at[0] = x;
  mouse_at[1] = y;
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::fill(int x, int y) {
  QVector2<int> pixel = mouseToImage(x, y);
  if (pixel[0] >= 0 && pixel[0] < frames[frame_index]->GetDimensions()[0] &&
      pixel[1] >= 0 && pixel[1] < frames[frame_index]->GetDimensions()[1]) {
    for (int d = 0; d < frames[frame_index]->GetChannelCount(); ++d) {
      (*frames[frame_index])(pixel)[d] = rgb[d];
    }
    uploadFrame();
  }
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::uploadFrame() {
  texture->Upload(frames[frame_index]->GetDimensions()[0], frames[frame_index]->GetDimensions()[1], frames[frame_index]->GetData());
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::fill(int x1, int y1, int x2, int y2) {
  QVector2<int> a(x1, y1);   
  QVector2<int> b(x2, y2);   
  QVector2<float> diff(b - a);
  int n = a.ChessboardDistanceTo(b);
  for (int i = 0; i <= n; ++i) {
    float t = n == 0 ? 0.0 : i / (float) n;
    QVector2<float> mix(QVector2<float>(a) + diff * t);
    fill(roundf(mix[0]), roundf(mix[1]));
  }
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::leftMouseUpAt(int x, int y) {
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
    if (scale * factor > 0.1f) {
      scale *= factor;
      modelview = QMatrix4<float>::GetScale(factor, factor, 1.0f) * modelview;
    }
  }
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::saveAs(const std::string &path) const {
  frames[frame_index]->Write(path);
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::setColor(const td::QVector4<int> rgb) {
  this->rgb = rgb; 
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::setFrameIndex(int i) {
  frame_index = i; 
  uploadFrame();
}

/* ------------------------------------------------------------------------- */

int CuadrosRenderer::getFrameIndex() const {
  return frame_index;
}

/* ------------------------------------------------------------------------- */

int CuadrosRenderer::getFrameCount() const {
  return frames.size();
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::addFrame() {
  frames.push_back(new NField<unsigned char, 2>(QVector2<int>(frames[0]->GetDimensions()[0], frames[0]->GetDimensions()[1]), frames[0]->GetChannelCount()));
  frames[frames.size() - 1]->Clear(128);
}

/* ------------------------------------------------------------------------- */

void CuadrosRenderer::updateLasso() {
  VertexAttribute *attribute = lasso_attributes->GetAttribute("position");  

  float l = Utilities::Minimum(rectangle1[0], rectangle2[0]);
  float r = Utilities::Maximum(rectangle1[0], rectangle2[0]);
  float b = Utilities::Minimum(rectangle1[1], rectangle2[1]);
  float t = Utilities::Maximum(rectangle1[1], rectangle2[1]);
  float positions[] = {
    l, b,
    r, b,
    r, t,
    l, t
  };

  float *position = positions;
  for (int i = 0; i < 4; ++i) {
    position[0] = position[0] / frames[frame_index]->GetDimensions()[0] * 2 - 1;
    position[1] = position[1] / frames[frame_index]->GetDimensions()[1] * 2 - 1;
    position += 2;
  }

  attribute->Update(positions);
}

/* ------------------------------------------------------------------------- */

