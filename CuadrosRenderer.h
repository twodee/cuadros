#ifndef CUADROSRENDERER_H
#define CUADROSRENDERER_H

#include <string>

#include "twodee/Configuration.h"
#include "twodee/ShaderProgram.h"
#include "twodee/Texture.h"
#include "twodee/VertexAttributes.h"
#include "twodee/VertexArray.h"

class CuadrosRenderer {
  public:

    CuadrosRenderer();
    ~CuadrosRenderer();

    void render();
    void resize(int width, int height);
    void initializeGL();
    void show(const std::string &path);

  private:
    td::VertexAttributes *attributes;
    td::VertexArray *array;
    td::ShaderProgram *program;
    td::Texture *texture;

    std::string path;
};

#endif
