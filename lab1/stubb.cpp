
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
#include <osg/LOD>
#include <osgUtil/Simplifier>

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
    RotateCB() : angle( 0. ) {}

    virtual void operator()( osg::Node* node,
            osg::NodeVisitor* nv )
    {
        // we know its a MatrixTransform.
        osg::MatrixTransform* mtRotate =
                dynamic_cast<osg::MatrixTransform*>( node );
        osg::Matrix mR, mT;
	//translat to the "middel" of the world
        mT.makeTranslate( 2.5f, 2.5f, 0 );
        mR.makeRotate( angle, osg::Vec3( 0., 0., 1. ) );
        mtRotate->setMatrix( mR * mT );

        // Increment the angle for the next from.
        angle += 0.01;

        // Continue traversing so that OSG can process
        //   any other nodes with callbacks.
        traverse( node, nv );
    }

protected:
    double angle;
};

  
// add intersector line globaly
osg::Vec3 line_p0 (-6, 0, 5);
osg::Vec3 line_p1 ( -1, 0, 5);
osg::ref_ptr<osg::Group> root = new osg::Group;


const int NR_OF_LIGHTS = 3;
osg::LightSource* lightSource[NR_OF_LIGHTS];
osg::PositionAttitudeTransform* lightTransform[NR_OF_LIGHTS];
osg::Geode* lightMarker[NR_OF_LIGHTS];
  
osg::Light* blinkLight = new osg::Light();

class CheckIntersect : public osg::NodeCallback
{
public:
  CheckIntersect() {}
  
  //void checkIntersectFunction()
  virtual void operator()( osg::Node* node,
            osg::NodeVisitor* nv )
  {
    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(line_p0, line_p1);
    // we know its a MatrixTransform.
    osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>( node );
   
    osgUtil::IntersectionVisitor intersectVisitor(intersector.get());
    root->accept(intersectVisitor);
    
    if(intersector->containsIntersections())
    {
      std::cout << "innanför \n";
      blinkLight->setDiffuse(osg::Vec4(1.0, 1.0, 1.0, 1.0));
      lightMarker[0]->getOrCreateStateSet()->setAttribute(setMaterial(osg::Vec4(1.0, 1.0, 1.0, 1.0)));
      
    }
    else
    {
      std::cout << "utanför \n";
      lightMarker[0]->getOrCreateStateSet()->setAttribute(setMaterial(osg::Vec4(0.0, 0.0, 0.8, 1.0)));
      blinkLight->setDiffuse(osg::Vec4(0.0, 0.0, 0.8, 1.0));
      blinkLight->setAmbient(osg::Vec4(0.0, 0.0, 0.0, 1.0));
      blinkLight->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
      
    }
  }
};

int main(int argc, char *argv[]){

  // global ful lösning
  blinkLight->setLightNum(0);  
  blinkLight->setPosition(osg::Vec4(0.0, 0.0, 0.0, 1.0));
  /// Line ---
  
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
  /*
  //add texture (ej klart)
 
  Image *image = osgDB::readImageFile("texture.png");
  osg::Texture2D *texture = new Texture2D;
  texture->setImage(image);
  
  StateSet *sphereStateSet = sphere->getOrCreateStateSet();
  sphereStateSet->setAttribute(material);
  sphereStateSet->setTextureAttributeAndModes(0, texture, StateAttribute::ON);*/
  
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
  int uniqueLightnr;
  
  osg::Vec4 lightColor[] = {osg::Vec4(0.0, 0.0, 0.8, 1.0), osg::Vec4(0.0, 0.8, 0.0, 1.0),  osg::Vec4(0.8, 0.0, 0.0, 1.0)};

  osg::ref_ptr<osg::StateSet> lightSS (root->getOrCreateStateSet());
  
  for(uniqueLightnr = 0; uniqueLightnr < NR_OF_LIGHTS; uniqueLightnr++) {
    
      // create sphere to represent the light
      lightMarker[uniqueLightnr] = new osg::Geode();
      lightMarker[uniqueLightnr]->addDrawable(new osg::ShapeDrawable(new       osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), 0.05f)));//osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 0.05f)));
      lightMarker[uniqueLightnr]->getOrCreateStateSet()->setAttribute(setMaterial(lightColor[uniqueLightnr]));
      lightMarker[uniqueLightnr]->setDataVariance( osg::Object::DYNAMIC );
    
      // create the light
      lightSource[uniqueLightnr] = new osg::LightSource();
      if(uniqueLightnr == 0 )
	lightSource[uniqueLightnr]->setLight(blinkLight);  
      else 
	lightSource[uniqueLightnr]->setLight(createLight(uniqueLightnr, lightColor[uniqueLightnr]));
      lightSource[uniqueLightnr]->setLocalStateSetModes(osg::StateAttribute::ON);
      lightSource[uniqueLightnr]->setStateSetModes(*lightSS,osg::StateAttribute::ON);
      
      // light transform
      lightTransform[uniqueLightnr] = new osg::PositionAttitudeTransform();
      //lightTransform[uniqueLightnr]->setPosition(osg::Vec3(2 * uniqueLightnr, 2 * uniqueLightnr, 4.0f));
      if(uniqueLightnr == 0 )
	lightTransform[0]->setPosition(osg::Vec3(0, 0, 4.0f));      
      else if(uniqueLightnr == 1 )
	lightTransform[1]->setPosition(osg::Vec3(4, 4, 4.0f));
      else if(uniqueLightnr == 2 )
	lightTransform[2]->setPosition(osg::Vec3(2, 0, 4.0f));
      
      // add light marker and lightsource to transform
      lightTransform[uniqueLightnr]->addChild(lightSource[uniqueLightnr]);
      lightTransform[uniqueLightnr]->addChild(lightMarker[uniqueLightnr]);
      
      if(uniqueLightnr == 0 )
      {
	  // Create a MatrixTransform to set setUpdateCallback on.
	  osg::ref_ptr<osg::MatrixTransform> LightChange =
		  new osg::MatrixTransform;
	  LightChange->setName( "color Light" );
	  // Set data variance to DYNAMIC to let OSG know that we
	  //   will modify this node during the update traversal.
	  LightChange->setDataVariance( osg::Object::DYNAMIC );
	  // Set the update callback.
	  LightChange->addChild( lightTransform[uniqueLightnr]);
	  LightChange->setUpdateCallback( new CheckIntersect );
	  // add transform to root
	  root->addChild( LightChange.get() );
      }
      else if(uniqueLightnr == 2 )
      {
	  // Create a MatrixTransform to set setUpdateCallback on.
	  osg::ref_ptr<osg::MatrixTransform> LightMove =
		  new osg::MatrixTransform;
	  LightMove->setName( "Moving Light" );
	  // Set data variance to DYNAMIC to let OSG know that we
	  //   will modify this node during the update traversal.
	  LightMove->setDataVariance( osg::Object::DYNAMIC );
	  // Set the update callback.
	  LightMove->setUpdateCallback( new RotateCB );
	  LightMove->addChild( lightTransform[uniqueLightnr]);
	  // add transform to root
	  root->addChild( LightMove.get() );
      }
      else
      {
	// add transform to root
	root->addChild(lightTransform[uniqueLightnr]);
      }
  }
  
  // add the LOD \____________________________________________________________________
  osg::LOD* lod = new osg::LOD;
  // read  object to change
  osg::Node* dumptruckNode = osgDB::readNodeFile("dumptruck.osg");
  osg::PositionAttitudeTransform* dumptruckTrans = new osg::PositionAttitudeTransform();
  dumptruckTrans->setPosition(osg::Vec3(2.0f, 4.0f, 3.0f));
  dumptruckTrans->setScale(osg::Vec3(0.07f, 0.07f, 0.07f));  
  dumptruckTrans->addChild(dumptruckNode);
  
  // reduce the number of vertices and faces
  osgUtil::Simplifier simplifier;
  osg::Node* dumptruckMediumNode = dynamic_cast<osg::Node*>( dumptruckNode->clone( osg::CopyOp::DEEP_COPY_ALL ));
  simplifier.setSampleRatio( 0.6f );
  dumptruckMediumNode->accept( simplifier );
  osg::PositionAttitudeTransform* dumptruckMediumTrans = new osg::PositionAttitudeTransform();
  dumptruckMediumTrans->setPosition(osg::Vec3(2.0f, 4.0f, 3.0f));
  dumptruckMediumTrans->setScale(osg::Vec3(0.07f, 0.07f, 0.07f));  
  dumptruckMediumTrans->addChild(dumptruckMediumNode);
  
  osg::PositionAttitudeTransform* dumptruckLowTrans = dynamic_cast<osg::PositionAttitudeTransform*>( dumptruckTrans->clone( osg::CopyOp::DEEP_COPY_ALL ));
  simplifier.setSampleRatio( 0.1f );
  dumptruckLowTrans->accept( simplifier );
  // add object to lod
  lod->addChild(dumptruckTrans, 0.f, 20.f); 
  lod->addChild(dumptruckMediumTrans, 20.f, 40.f);  
  lod->addChild(dumptruckLowTrans, 40.f, 100.f);
  root->addChild(lod);
  
  
  // Animation Phath ______________________________________________________________________
  
  // Create animation path
  osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
  
  // Define control points
  /*osg::AnimationPath::ControlPoint CP0(osg::Vec3( 5.f, 5.f, 5.f ));
  osg::AnimationPath::ControlPoint CP1(osg::Vec3( -5.f, 5.f, 5.f ), osg::Quat(-osg::PI/6,osg::Vec3( 0.f, 0.f, 1.f )));
  osg::AnimationPath::ControlPoint CP2 (osg::Vec3( -5.f, -5.f, 5.f ), osg::Quat(-osg::PI*2/6,osg::Vec3( 0.f, 0.f, 1.f )), osg::Vec3( 0.5f, 0.5f, 0.5f ));
  osg::AnimationPath::ControlPoint CP3 (osg::Vec3( 5.f, -5.f, 6.f ) ,osg::Quat(osg::PI*3/6,osg::Vec3( 0.f, 0.f, 1.f )));
  osg::AnimationPath::ControlPoint CP4 (osg::Vec3( 5.f, 5.f, 5.f ));*/
  osg::AnimationPath::ControlPoint CP0(osg::Vec3( 5.f, 5.f, 5.f ));
  osg::AnimationPath::ControlPoint CP1(osg::Vec3( -5.f, 5.f, 5.f ));
  osg::AnimationPath::ControlPoint CP2 (osg::Vec3( -5.f, -5.f, 5.f ));
  osg::AnimationPath::ControlPoint CP3 (osg::Vec3( 5.f, -5.f, 6.f ));
  osg::AnimationPath::ControlPoint CP4 (osg::Vec3( 5.f, 5.f, 5.f ));

  // Insert them to the path
  path->insert( 0.0f, CP0 );
  // time, point
  path->insert( 1.0f, CP1 );
  path->insert( 2.0f, CP2 );
  path->insert( 3.0f, CP3 );
  path->insert( 4.0f, CP4 );
  path->insert( 4.0f, CP4 );
  
  // Define animation path callback
  osg::ref_ptr<osg::AnimationPathCallback> APCallback = new osg::AnimationPathCallback(path.get());
  
  // Update the matrix transform (of the object) with the animation path
  //cessenaTrans->setUpdateCallback( APCallback.get() );
  gliderTrans-> setUpdateCallback( APCallback.get() );
  
  
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

