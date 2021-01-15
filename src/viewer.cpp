#include "viewer.h"
#include "camera.h"

using namespace Eigen;

Viewer::Viewer()
  : _winWidth(0), _winHeight(0), _zoom(1.), _trans(0., 0.), _mode(0), _modeEnable(false)
{
}

Viewer::~Viewer()
{
}

////////////////////////////////////////////////////////////////////////////////
// GL stuff

// initialize OpenGL context
void Viewer::init(int w, int h){
    loadShaders();

    if(!_mesh.load(DATA_DIR"/models/lemming.off")) exit(1);
    _mesh.initVBA();

    // clear color TODO TOCHECK
    glClearColor(.6, .6, .6, 1.);
    // z-depth TODO TOCHECK LOCATION
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.);
    // Lines antialiasing
    glEnable(GL_LINE_SMOOTH);

    reshape(w,h);
    _trackball.setCamera(&_cam);
}

void Viewer::reshape(int w, int h){
    _winWidth = w;
    _winHeight = h;
    _cam.setViewport(w,h);
    glViewport(0, 0, _winWidth, _winHeight);
}


/*!
   callback to draw graphic primitives
 */
void Viewer::drawScene()
{
    // visualiser le maillage
    if (_mode == 0) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    else            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    // Activate shader
    _shader.activate();
    // Uniforms
    glUniform1f(_shader.getUniformLocation("zoom"),  _zoom);
    glUniform2f(_shader.getUniformLocation("trans"), _trans.x(), _trans.y());
    glUniform1i(_shader.getUniformLocation("mode"),  _mode);
    // Draw mesh
    _mesh.draw(_shader);
    // Deactivate shader
    _shader.deactivate();
}


void Viewer::updateAndDrawScene()
{
    // clear screen TODO TOCHECK
    glClear(GL_COLOR_BUFFER_BIT);
    // z-depth clear TODO TOCHECK LOCATION
    glClear(GL_DEPTH_BUFFER_BIT);
    _mode = 0;
    drawScene();
    if (_modeEnable) {
        _mode = 1;
        drawScene();
    }
}

void Viewer::loadShaders()
{
    // Here we can load as many shaders as we want, currently we have only one:
    _shader.loadFromFiles(DATA_DIR"/shaders/simple.vert", DATA_DIR"/shaders/simple.frag");
    checkError();
}

////////////////////////////////////////////////////////////////////////////////
// Events

/*!
   callback to manage keyboard interactions
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::keyPressed(int key, int action, int /*mods*/)
{
  if(key == GLFW_KEY_R && action == GLFW_PRESS)
  {
    loadShaders();
  }

  if(action == GLFW_PRESS || action == GLFW_REPEAT )
  {
    if (key==GLFW_KEY_UP)
    {
        _trans -= Vector2f(0., .1);
    }
    else if (key==GLFW_KEY_DOWN)
    {
        _trans += Vector2f(0., .1);
    }
    else if (key==GLFW_KEY_LEFT)
    {
        _trans += Vector2f(.1, 0.);
    }
    else if (key==GLFW_KEY_RIGHT)
    {
        _trans -= Vector2f(.1, 0.);
    }
    else if (key==GLFW_KEY_PAGE_UP)
    {
        _zoom *= 1.1;
    }
    else if (key==GLFW_KEY_PAGE_DOWN)
    {
        _zoom /= 1.1;
    }
    else if (key==GLFW_KEY_L)
    {
        _modeEnable = !_modeEnable;
    }
  }
}

/*!
   callback to manage mouse : called when user press or release mouse button
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mousePressed(GLFWwindow */*window*/, int /*button*/, int action)
{
  if(action == GLFW_PRESS)
  {
      _trackingMode = TM_ROTATE_AROUND;
      _trackball.start();
      _trackball.track(_lastMousePos);
  }
  else if(action == GLFW_RELEASE)
  {
      _trackingMode = TM_NO_TRACK;
  }
}


/*!
   callback to manage mouse : called when user move mouse with button pressed
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mouseMoved(int x, int y)
{
    if(_trackingMode == TM_ROTATE_AROUND)
    {
        _trackball.track(Vector2i(x,y));
    }

    _lastMousePos = Vector2i(x,y);
}

void Viewer::mouseScroll(double /*x*/, double y)
{
  _cam.zoom(-0.1*y);
}

void Viewer::charPressed(int /*key*/)
{
}
