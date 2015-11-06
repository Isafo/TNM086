
#include <osg/Version>

#include <osgViewer/Viewer>
#include <osgUtil/Optimizer>

#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>
#include <osg/Light>
#include <osg/LightSource>


osg::Light* createLight(int uniqueLightnr, osg::Vec4 color, osg::Vec4 position);

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
  gliderTrans->setPosition(osg::Vec3(20.0f, 30.0f, 0.0f));
  
  root->addChild(gliderTrans);
  gliderTrans->addChild(gliderNode);
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
  int uniqueLightnr = 0;
  const int NR_OF_LIGHTS = 2;
  osg::LightSource* lightSource[NR_OF_LIGHTS];
  osg::Geode lightMaker[NR_OF_LIGHTS]
  
  osg::Vec4 lightPos = osg::Vec4(100.0f , 80.0f, 0.0f, 1.0f);
  osg::Vec4 lightColor = osg::Vec4(0.3f , 0.0f, 1.0f, 1.0f);
  
  // create the first light
  lightSource[uniqueLightnr] = new osg::LightSource();
  
  osg::Light* light1 = createLight(uniqueLightnr, lightColor, lightPos);
  lightSource[uniqueLightnr]->setLight(light1);
  lightSource[uniqueLightnr]->setLocalStateSetModes(osg::StateAttribute::ON);
  
  lightTransform[i] = new osg::PositionAttitudeTransform();
  
  
  root->addChild()
  
  uniqueLightnr++;
  
  
  
  // create second light
  
  
  // not our stuff \_______________________________________________________________________________________
  // Optimizes the scene-graph
  osgUtil::Optimizer optimizer;
  optimizer.optimize(root);
  
  // Set up the viewer and add the scene-graph root
  osgViewer::Viewer viewer;
  viewer.setSceneData(root);
  
  return viewer.run();
  
  
  
}

osg::Light* createLight(int uniqueLightnr, osg::Vec4 color, osg::Vec4 position) {
  
  osg::Light* light = new osg::Light();
  light->setLightNum(uniqueLightnr);
  
  light->setPosition(osg::Vec4(position));
  light->setDiffuse(osg::Vec4(color));
  light->setAmbient(osg::Vec4(0.0, 0.0, 0.1, 1.0));
  light->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
  
  return light;
}

