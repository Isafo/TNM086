
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
#include <osg/AnimationPath>
#include <osg/MatrixTransform>

// *************** function declaration *********************************
osg::Light* createLight(int uniqueLightnr, osg::Vec4 color);
osg::Material *setMaterial(osg::Vec4 color);
void update(osg::PositionAttitudeTransform* lightTransform);
// **********************************************************************

// Derive a class from NodeCallback to manipulate a
//   MatrixTransform object's matrix.
class RotateCB : public osg::NodeCallback
{
public:
    RotateCB() : _angle( 0. ) {}

    virtual void operator()( osg::Node* node,
            osg::NodeVisitor* nv )
    {
        // Normally, check to make sure we have an update
        //   visitor, not necessary in this simple example.
        osg::MatrixTransform* mtLeft =
                dynamic_cast<osg::MatrixTransform*>( node );
        osg::Matrix mR, mT;
        mT.makeTranslate( -6., 0., 0. );
        mR.makeRotate( _angle, osg::Vec3( 0., 0., 1. ) );
        mtLeft->setMatrix( mR * mT );

        // Increment the angle for the next from.
        _angle += 0.01;

        // Continue traversing so that OSG can process
        //   any other nodes with callbacks.
        traverse( node, nv );
    }

protected:
    double _angle;
};

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

  const float dimX = 6;
  const float dimY = 6;
  
  // Add your stuff to the root node here...
  
  // Ground stuff \_______________________________________________________________________________________
  // create ground
  osg::HeightField* ground = new osg::HeightField();
  ground->allocate(dimX, dimY);
  ground->setXInterval(1.0f); //TODO vad gör setXinterval????? spec när man inte lopar över detta område
  ground->setYInterval(1.0f);
  ground->setOrigin(osg::Vec3(0.0f, 0.0f, 0.0f));
  
  for(int i = 0; i < dimX; i++) {
    for (int j = 0; j < dimY; j++) {
      ground->setHeight(i, j, cos(i) + sin(j));
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
  gliderTrans->setPosition(osg::Vec3(0.0f, 0.0f, 2.0f));
  
  gliderTrans->addChild(gliderNode);
  root->addChild(gliderTrans);

  // read secound objects
  osg::Node* cessenaNode = osgDB::readNodeFile("cessna.osg");
  osg::PositionAttitudeTransform* cessenaTrans = new osg::PositionAttitudeTransform();
  cessenaTrans->setPosition(osg::Vec3(0.0f, 0.0f, 6.0f));
  cessenaTrans->setScale(osg::Vec3(0.07f, 0.07f, 0.07f));
  
  cessenaTrans->addChild(cessenaNode);
  root->addChild(cessenaTrans);
  
  // add the lights \____________________________________________________________________
  const int NR_OF_LIGHTS = 3;
  osg::LightSource* lightSource[NR_OF_LIGHTS];
  osg::PositionAttitudeTransform* lightTransform[NR_OF_LIGHTS];
  osg::Geode* lightMarker[NR_OF_LIGHTS];
  int uniqueLightnr;
  
  osg::Vec4 lightColor[] = {osg::Vec4(0.0, 0.0, 0.8, 1.0), osg::Vec4(0.0, 0.8, 0.0, 1.0),  osg::Vec4(0.8, 0.0, 0.0, 1.0)};

  osg::ref_ptr<osg::StateSet> lightSS (root->getOrCreateStateSet());
  
  for(uniqueLightnr = 0; uniqueLightnr < NR_OF_LIGHTS; uniqueLightnr++) {
    
      // create sphere to represent the light
      lightMarker[uniqueLightnr] = new osg::Geode();
      lightMarker[uniqueLightnr]->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 0.05f)));
      lightMarker[uniqueLightnr]->getOrCreateStateSet()->setAttribute(setMaterial(lightColor[uniqueLightnr]));
    
      // create the light
      lightSource[uniqueLightnr] = new osg::LightSource();
      lightSource[uniqueLightnr]->setLight(createLight(uniqueLightnr, lightColor[uniqueLightnr]));
      lightSource[uniqueLightnr]->setLocalStateSetModes(osg::StateAttribute::ON);
      lightSource[uniqueLightnr]->setStateSetModes(*lightSS,osg::StateAttribute::ON);
      
      // light transform
      lightTransform[uniqueLightnr] = new osg::PositionAttitudeTransform();
      lightTransform[uniqueLightnr]->setPosition(osg::Vec3(2 * uniqueLightnr, 2 * uniqueLightnr, 4.0f));
      
      // add light marker and lightsource to transform
      lightTransform[uniqueLightnr]->addChild(lightSource[uniqueLightnr]);
      lightTransform[uniqueLightnr]->addChild(lightMarker[uniqueLightnr]);
      
      if(uniqueLightnr == 2 )
      {
	//lightTransform[uniqueLightnr]->setUpdateCallback( new RotateCB );
      }
      
      // add transform to root
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
  
  lightTransform->setPosition(osg::Vec3(0.1 * cos(timer->elapsedTime_m() /10.0f),
					0.1 * sin(timer->elapsedTime_m() /10.0f), 1.0f));
  
  
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

