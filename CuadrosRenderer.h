#ifndef CUADROSRENDERER_H
#define CUADROSRENDERER_H

#include <string>

#include "twodee/Configuration.h"

#include "twodee/NField.h"
#include "twodee/QMatrix4.h"
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

    void setInterpolation(int mode);
    int getInterpolation() const;

    void leftMouseDownAt(int x, int y);
    void leftMouseDraggedTo(int x, int y);
    void leftMouseUpAt(int x, int y);
    void scroll(int nTicks);

  private:
    td::VertexAttributes *attributes;
    td::VertexArray *array;
    td::ShaderProgram *program;
    td::Texture *texture;

    std::string path;
    td::NField<float, 2> *image;
    td::QMatrix4<float> projection;
    int interpolation_mode;
    
    const static int INTERPOLATION_NEAREST = 0;
    const static int INTERPOLATION_LINEAR = 1;
};

#endif
