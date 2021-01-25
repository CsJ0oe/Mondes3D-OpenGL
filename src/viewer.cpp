#include "viewer.h"
#include "camera.h"

using namespace Eigen;

Viewer::Viewer()
  : _winWidth(0), _winHeight(0), _zoom(1.), _trans(0., 0.), _angle(0., 0., 0.), _rot(),
    _mode(0), _modeEnable(false), _view(0), _splitViewPortEnable(false)
{
    _rot.setIdentity();
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

    // clear color
    glClearColor(.8, .8, .8, 1.);
    // z-depth
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
}


/*!
   callback to draw graphic primitives
 */
void Viewer::drawScene()
{
    // change view port
    if (_splitViewPortEnable) {
        if (_view == 0) glViewport(0, 0, _winWidth/2, _winHeight);
        else            glViewport(_winWidth/2, 0, _winWidth/2, _winHeight);
    } else {
        glViewport(0, 0, _winWidth, _winHeight);
    }
    // visualiser le maillage
    if (_mode == 0) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    else            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    // Activate shader
    _shader.activate();
    // Uniforms
    Affine3f A;
    A = Translation3f(_trans.x(), _trans.y(), 0.0) *
        AngleAxisf(_angle.x(), Vector3f(1, 0, 0)) *
        AngleAxisf(_angle.y(), Vector3f(0, 1, 0)) *
        AngleAxisf(_angle.z(), Vector3f(0, 0, 1)) *
        Scaling(_zoom);
    // rotate the second viewport
    if (_splitViewPortEnable && _view) A = A * AngleAxisf(M_PI_2, Vector3f(0, 1, 0));
    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"), 1, GL_FALSE, A.data());
    glUniform1i(_shader.getUniformLocation("mode"),  _mode);
    // zoom + translation + viewportSplit (OLD:TD1)
    //glUniform1f(_shader.getUniformLocation("zoom"),  _zoom);
    //glUniform2f(_shader.getUniformLocation("trans"), _trans.x(), _trans.y());
    //glUniform1i(_shader.getUniformLocation("view"),  _view);
    // Draw mesh
    _mesh.draw(_shader);
    // Deactivate shader
    _shader.deactivate();
}

void Viewer::updateAndDrawScene()
{
    // clear screen and z-depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // For chair.off
    //return drawScene2D();

    // change view port
    _view = 0;
    // render mesh
    _mode = 0;
    drawScene();
    if (_modeEnable) {
        // render lines
        _mode = 1;
        drawScene();
    }

    if (_splitViewPortEnable) {
        // change view port
        _view = 1;
        // render mesh
        _mode = 0;
        drawScene();
        if (_modeEnable) {
            // render lines
            _mode = 1;
            drawScene();
        }
    }
}

void Viewer::drawScene2D()
{
    glViewport(0, 0, _winWidth, _winHeight);
    Affine3f A;

    _shader.activate();

    // first chair
    A = Translation3f(-.5, -1., 0.) * Scaling(.5f);
    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"), 1, GL_FALSE, A.data());
    _mesh.draw(_shader);

    // second chair
    A = Translation3f(+.5, -1., 0.) * AngleAxisf(M_PI, Vector3f(0., 1., 0.)) * Scaling(.5f);
    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"), 1, GL_FALSE, A.data());
    _mesh.draw(_shader);

    // third chair : keyboard control with A and D
    A = Translation3f(0., +.5, 0.) * AngleAxisf(_angle.y(), Vector3f(0., 0., 1.)) * Translation3f(0., -.5, 0.) * Scaling(1.f);
    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"), 1, GL_FALSE, A.data());
    _mesh.draw(_shader);

    _shader.deactivate();
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
        _trans += Vector2f(0., .1);
    }
    else if (key==GLFW_KEY_DOWN)
    {
        _trans -= Vector2f(0., .1);
    }
    else if (key==GLFW_KEY_LEFT)
    {
        _trans -= Vector2f(.1, 0.);
    }
    else if (key==GLFW_KEY_RIGHT)
    {
        _trans += Vector2f(.1, 0.);
    }
    else if (key==GLFW_KEY_PAGE_UP)
    {
        _zoom /= 1.1;
    }
    else if (key==GLFW_KEY_PAGE_DOWN)
    {
        _zoom *= 1.1;
    }
    else if (key==GLFW_KEY_L)
    {
        _modeEnable = !_modeEnable;
    }
    else if (key==GLFW_KEY_K)
    {
        _splitViewPortEnable = !_splitViewPortEnable;
    }
    else if (key==GLFW_KEY_Q)
    {
        _angle += Vector3f(0, 0, M_PI/10);
    }
    else if (key==GLFW_KEY_E)
    {
        _angle -= Vector3f(0, 0, M_PI/10);
    }
    else if (key==GLFW_KEY_A)
    {
        _angle += Vector3f(0, M_PI/10, 0);
    }
    else if (key==GLFW_KEY_D)
    {
        _angle -= Vector3f(0, M_PI/10, 0);
    }
    else if (key==GLFW_KEY_W)
    {
        _angle += Vector3f(M_PI/10, 0, 0);
    }
    else if (key==GLFW_KEY_S)
    {
        _angle -= Vector3f(M_PI/10, 0, 0);
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
