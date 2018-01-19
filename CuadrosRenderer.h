#ifndef CUADROSRENDERER_H
#define CUADROSRENDERER_H

#include <string>

#include "twodee/Configuration.h"

#include "twodee/NField.h"
#include "twodee/QMatrix4.h"
#include "twodee/QVector2.h"
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
    void rightMouseDownAt(int x, int y);
    void rightMouseDraggedTo(int x, int y);
    void rightMouseUpAt(int x, int y);
    void scroll(int nTicks);
    bool isMouseOverImage(int x, int y) const;
    td::QVector2<int> mouseToImage(int x, int y) const;

    void saveAs(const std::string &path) const;

  private:
    td::VertexAttributes *attributes;
    td::VertexArray *array;
    td::ShaderProgram *program;
    td::Texture *texture;

    std::string path;
    td::NField<unsigned char, 2> *image;
    td::QMatrix4<float> projection;
    td::QMatrix4<float> modelview;
    int interpolation_mode;
    td::QVector2<int> dimensions;
    td::QVector2<int> mouse_at;
    
    const static int INTERPOLATION_NEAREST = 0;
    const static int INTERPOLATION_LINEAR = 1;
};

#endif
