/** @file Week6-0-OctreeSceneManagerDemo
 *  @brief create a scene using Octree
 * each node has a maximum of eight children, hence the name Octree. 
 * The beauty of this algorithm is that we can discard a lot of the scene in the first few steps.
 * https://commons.wikimedia.org/wiki/File:Octree2.svg
 * if we have a 3D scene with two objects in it, we can enclose this scene with a cube.
 * If we divide this cube at half of its width, height, and depth, we get eight new cubes, each
 * enclosing an eighth of the scene. These eight cubes can be thought of as the eight children
 * of the original cube.
 * Now the two objects of the scene are in the right upper-front cube. The seven other
 * cubes are empty and are therefore leaves. We will divide the cube that contains the
 * two objects again.
 * Now each cube has either one or no object enclosed and is a leaf.
 * To determine which objects have to be rendered, we start with the
 * root node of the Octree. If the view frustum intersects the cube, we continue.
 * This should always be the case, because the Octree on level 0 encloses the complete scene with the
 * camera. We then continue with level one of the Octree, meaning the root node's children.
 * We test the view frustum against each child, and if they intersect, we continue with the
 * children of this cube. Also, if the cube is completely inside the frustum, we don't need to go
 * any deeper because we know all children of the cube are in the frustum as well. We do this
 * till we have reached a leaf, and then continue with the next child till we only have leaves left.
 *
 *  @author Hooman Salamat
 *  @bug No known bugs.
 */

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "ConsoleColor.h"
#include <iostream>

using namespace Ogre;
using namespace OgreBites;

Ogre::Vector3 translate(0, 0, 0);
float rotX = 0.0f;
float rotY = 0.0f;

class ExampleFrameListener : public Ogre::FrameListener
{
private:
    Ogre::SceneNode* _sceneNode;
    Ogre::SceneNode* _camNode;
    float _movementspeed;
    float _mousespeed;
public:

    ExampleFrameListener(Ogre::SceneNode* sceneNode, Ogre::SceneNode* camNode)
    {
        _sceneNode = sceneNode;
        _camNode = camNode;
        _movementspeed = 2.0f;
        _mousespeed = 0.002f;
    }

    bool frameStarted(const Ogre::FrameEvent& evt)
    {

        float rotXNew = rotX * evt.timeSinceLastFrame * -1;
        float rotYNew = rotY * evt.timeSinceLastFrame * -1;


        //_camNode->yaw(Ogre::Radian(rotXNew));
        //_camNode->pitch(Ogre::Radian(rotYNew));
        //_camNode->moveRelative(translate * evt.timeSinceLastFrame * _movementspeed);
        _camNode->translate(translate * evt.timeSinceLastFrame * _movementspeed);

        return true;
    }
};

class Game
    : public ApplicationContext
    , public InputListener
{
private:
    SceneNode* SinbadNode;
    SceneManager* mScnMgr;
    Root* mRoot;
    Ogre::PolygonMode mPolyMode;
    Camera* cam;
    SceneNode* camNode;
public:
    Game();
    virtual ~Game() {}

    void setup();
    bool keyPressed(const KeyboardEvent& evt);
    void createScene();
    void createCamera();
    bool mouseMoved(const MouseMotionEvent& evt);
    void createFrameListener();
};


Game::Game()
    : ApplicationContext("Week6-1-Demo")
{
}


void Game::setup()
{
    // do not forget to call the base first
    ApplicationContext::setup();
    addInputListener(this);

    // get a pointer to the already created root
    mRoot = getRoot();
    mScnMgr = mRoot->createSceneManager();


    // register our scene with the RTSS
    RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(mScnMgr);

    mPolyMode = Ogre::PolygonMode::PM_SOLID;

    createScene();
    createCamera();
    createFrameListener();
}

void Game::createScene()
{
    // -- tutorial section start --

    Ogre::SceneNode* node = mScnMgr->createSceneNode("Node1");
    mScnMgr->getRootSceneNode()->addChild(node);

    //! [turnlights]
    mScnMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
    mScnMgr->setShadowTechnique(ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
    //! [turnlights]

    //! [newlight]
    //
    Light* light1 = mScnMgr->createLight("Light1");
    light1->setType(Ogre::Light::LT_DIRECTIONAL);
    // Set Light Color
    light1->setDiffuseColour(1.0f, 1.0f, 1.0f);
    // Set Light Reflective Color
    light1->setSpecularColour(1.0f, 1.0f, 0.0f);
    // Set Light (Range, Brightness, Fade Speed, Rapid Fade Speed)
    //light1->setAttenuation(10, 0.5, 0.045, 0.0);

    //
    Entity* lightEnt = mScnMgr->createEntity("LightEntity", "sphere.mesh");
    SceneNode* lightNode = node->createChildSceneNode("LightNode");
    lightNode->attachObject(lightEnt);
    lightNode->attachObject(light1);
    lightNode->setScale(0.01f, 0.01f, 0.01f);

    //! [newlight]



    //! [lightDirection]
    lightNode->setDirection(1, -1, 0);
    //! [lightDirection]


     Entity* Sinbad = mScnMgr->createEntity("Sinbad.mesh");
    Sinbad->setCastShadows(true);
    SceneNode* SinbadNode = mScnMgr->createSceneNode("SinbadNode");
    SinbadNode->attachObject(Sinbad);
    mScnMgr->getRootSceneNode()->addChild(SinbadNode);
    SinbadNode->setScale(1.0f, 1.0f, 1.0f);
    SinbadNode->setPosition(0, 1.0, 0);

    
    //mScnMgr = mRoot->createSceneManager("BspSceneManager");
    //mScnMgr = mRoot->createSceneManager("OctreeSceneManager");

    std::cout << green << mScnMgr->getTypeName() << "::" << mScnMgr->getName() << white << std::endl;

    // -- tutorial section end --
}

void Game::createCamera()
{
    //! [camera]
    camNode = mScnMgr->getRootSceneNode()->createChildSceneNode();

    // create the camera
    cam = mScnMgr->createCamera("myCam");
    cam->setNearClipDistance(4); // specific to this sample
    cam->setFarClipDistance(4000);
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);
    camNode->setPosition(0, 20, 20);
    camNode->lookAt(Ogre::Vector3(0, 0, 0), Node::TS_WORLD);
    //cam->setPolygonMode(Ogre::PM_WIREFRAME);


    // and tell it to render into the main window
    getRenderWindow()->addViewport(cam);
}


void Game::createFrameListener()
{
    Ogre::FrameListener* FrameListener = new ExampleFrameListener(SinbadNode, camNode);
    mRoot->addFrameListener(FrameListener);
}


bool Game::mouseMoved(const MouseMotionEvent& evt)
{
    rotX = evt.xrel;
    rotY = evt.yrel;
    return true;
}

bool Game::keyPressed(const KeyboardEvent& evt)
{

    switch (evt.keysym.sym)
    {
    case SDLK_ESCAPE:
        getRoot()->queueEndRendering();
        break;
    case 'w':
        translate = Ogre::Vector3(0, 0, -1);
        break;
    case 's':
        translate = Ogre::Vector3(0, 0, 1);
        break;
    case 'a':
        translate = Ogre::Vector3(-1, 0, 0);
        break;
    case 'd':
        translate = Ogre::Vector3(1, 0, 0);
        break;
    case 'p':
        translate = Ogre::Vector3(0, 0, 0);
        break;
    case ' ':
        if (mPolyMode == Ogre::PolygonMode::PM_SOLID)
            mPolyMode = Ogre::PolygonMode::PM_WIREFRAME;
        else
            mPolyMode = Ogre::PolygonMode::PM_SOLID;
        cam->setPolygonMode(mPolyMode);
        break;
    default:
        break;
    }
    return true;
}


int main(int argc, char** argv)
{
    try
    {
        Game app;
        app.initApp();
        app.getRoot()->startRendering();
        app.closeApp();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occurred during execution: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

//! [fullsource]
