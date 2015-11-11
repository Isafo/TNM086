
#include <osg/Version>

#include <osgViewer/Viewer>
#include <osgUtil/Optimizer>

#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>
#include <osg/Light>
#include <osg/LightSource>

// *************** function declaration *********************************
osg::Light* createLight(int uniqueLightnr, osg::Vec4 color);
osg::Material *setMaterial(osg::Vec4 color);
void update(osg::PositionAttitudeTransform* lightTransform);
// **********************************************************************

int main(int argc, char *argv[]){
  
  osg::ref_ptr<osg::Group> root = new osg::Group;

#if 0
  /// Line ---

  osg::Vec3 line_p0 (-1, 0, 0);
  osg::Vec3 line_p1 ( 1, 0, 0);
  
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  vertices->push_back(line_p0);
  vertices->push_back(line_p1);
  
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
  colors->push_back(osg::Vec4(0.9f,0.2f,0.3f,1.0f));

  osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();
  linesGeom->setVertexArray(vertices);
  linesGeom->setColorArray(colors, osg::Array::BIND_OVERALL);
  
  linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));
  
  osg::ref_ptr<osg::Geode> lineGeode = new osg::Geode();
  lineGeode->addDrawable(linesGeom);
  lineGeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
  
  root->addChild(lineGeode);
  
  /// ---
#endif

  const int dimX = 256;
  const int dimY = 256;
  
  // Add your stuff to the root node here...
  
  // Ground stuff \_______________________________________________________________________________________
  // create ground
  osg::HeightField* ground = new osg::HeightField();
  ground->allocate(dimX, dimY);
  ground->setXInterval(10.0f); //TODO vad gör setXinterval????? spec när man inte lopar över detta område
  ground->setYInterval(10.0f);
  ground->setOrigin(osg::Vec3(0.0f, 0.0f, 0.0f));
  
  for(int i = 0; i < dimX; i++) {
    for (int j = 0; j < dimY; j++) {
      ground->setHeight(i, j, cos(i)*9.0f + sin(j)*5.0f);
    }
  }
  
  // add the ground to the scene
  osg::Geode* groundGeode = new osg::Geode();
  
  groundGeode->addDrawable(new osg::ShapeDrawable(ground));
  
  root->addChild(groundGeode);
  
  // Read objects \_______________________________________________________________________________________
  // read first objects
  osg::Node* gliderNode = osgDB::readNodeFile("glider.osg");
  osg::PositionAttitudeTransform* gliderTrans = new osg::PositionAttitudeTransform();
  gliderTrans->setPosition(osg::Vec3(0.0f, 10.0f, 0.0f));
  gliderTrans->setScale(osg::Vec3(10.0f, 10.0f, 10.0f));
  
  gliderTrans->addChild(gliderNode);
  root->addChild(gliderTrans);

  
  
  /*
  // add the ground to the scene
  osg::Geode* gliderGeode = new osg::Geode();
  
  gliderGeode->addDrawable(new osg::ShapeDrawable(gliderNode));
  
  root->addChild(groundGeode);
  
  
  // read second objects
  osg::Node* spaceNode = osgDB::readNodeFile("spaceship.osg");
  gliderNode->setOrigin(0.0f, 0.0f, 160.0f);
  
  // read third objects
  osg::Node* cNode = osgDB::readNodeFile("cessna.osg");
  gliderNode->setOrigin(0.0f, 0.0f, 160.0f);
  
  // read fourth objects
  osg::Node* dumpNode = osgDB::readNodeFile("dumptruck.osg");
  gliderNode->setOrigin(0.0f, 0.0f, 160.0f);*/
  
  
  // add the lights \____________________________________________________________________
  const int NR_OF_LIGHTS = 3;
  osg::LightSource* lightSource[NR_OF_LIGHTS];
  osg::PositionAttitudeTransform* lightTransform[NR_OF_LIGHTS];
  osg::Geode* lightMarker[NR_OF_LIGHTS];
  int uniqueLightnr;
  
  osg::Vec4 lightColor[] = {osg::Vec4(0.0, 0.0, 0.8, 1.0), osg::Vec4(0.0, 0.8, 0.0, 1.0),  osg::Vec4(0.8, 0.0, 0.0, 1.0)};


  for(uniqueLightnr = 0; uniqueLightnr < NR_OF_LIGHTS; uniqueLightnr++) {
    
      // create sphere to represent the light
      lightMarker[uniqueLightnr] = new osg::Geode();
      lightMarker[uniqueLightnr]->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 50.0f)));
      lightMarker[uniqueLightnr]->getOrCreateStateSet()->setAttribute(setMaterial(lightColor[uniqueLightnr]));
    
      // create the light
      lightSource[uniqueLightnr] = new osg::LightSource();
      lightSource[uniqueLightnr]->setLight(createLight(uniqueLightnr, lightColor[uniqueLightnr]));
      lightSource[uniqueLightnr]->setLocalStateSetModes(osg::StateAttribute::ON);
      
      // light transform
      lightTransform[uniqueLightnr] = new osg::PositionAttitudeTransform();
      lightTransform[uniqueLightnr]->addChild(lightSource[uniqueLightnr]);
      lightTransform[uniqueLightnr]->addChild(lightMarker[uniqueLightnr]);
      lightTransform[uniqueLightnr]->setPosition(osg::Vec3(2560.0 * uniqueLightnr/3, 2560.0f * uniqueLightnr/3, 1250.0f));
      
      root->addChild(lightTransform[uniqueLightnr]);
  }
  
  
  // not our stuff \______________________________________________________________________
  // Optimizes the scene-graph
  osgUtil::Optimizer optimizer;
  optimizer.optimize(root);
  
  // Set up the viewer and add the scene-graph root
  osgViewer::Viewer viewer;
  viewer.setSceneData(root);
  
  return viewer.run();
}

void update(osg::PositionAttitudeTransform* lightTransform) {
  
  osg::ElapsedTime* timer = new osg::ElapsedTime();
  
  lightTransform->setPosition(osg::Vec3(2560.0 * cos(timer->elapsedTime_m() /1000.0f),
					2560.0 * sin(timer->elapsedTime_m() /1000.0f), 1250.0f));
  
  
}

osg::Material *setMaterial(osg::Vec4 color) {
    osg::Material *material = new osg::Material();
    material->setDiffuse(osg::Material::FRONT,  osg::Vec4(0.0, 0.0, 0.0, 1.0));
    material->setEmission(osg::Material::FRONT, color);

    return material;
}


osg::Light* createLight(int uniqueLightnr, osg::Vec4 color) {
  
  osg::Light* light = new osg::Light();
  light->setLightNum(uniqueLightnr);
  
  light->setPosition(osg::Vec4(0.0, 0.0, 0.0, 1.0));
  light->setDiffuse(osg::Vec4(color));
  light->setAmbient(osg::Vec4(0.0, 0.0, 0.0, 1.0));
  light->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
  
  return light;
}

