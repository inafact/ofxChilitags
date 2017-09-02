#include "ofxChilitags.h"

using namespace ofxCv;

ofxChilitags::ofxChilitags() 
{}

ofxChilitags::~ofxChilitags()
{}

void ofxChilitags::init(int detection_period)
{
  chilitags.setPerformance(chilitags::Chilitags::ROBUST);
  chilitags.setFilter(0, 0.);
  chilitags.setDetectionPeriod(detection_period);
  trig = chilitags::Chilitags::ASYNC_DETECT_PERIODICALLY;
  //trig = chilitags::Chilitags::DETECT_PERIODICALLY;
  UP = ofVec2f(0,1);
}

void ofxChilitags::update(ofPixels &pixels)
{
  int width = pixels.getWidth();
  int height = pixels.getHeight(); 

  cv::Mat inputImage = toCv(pixels);
  auto tags2d = chilitags.find(inputImage, trig);

  _tags.clear();
  for (const auto& tag : tags2d)
  {
    int id = tag.first;
    const cv::Mat_<cv::Point2f> corners(tag.second);
    Tag t = make_tag(id, corners, width, height);
    _tags.push_back(t);
  }
}

vector<Tag>& ofxChilitags::tags()
{
  return _tags;
}

Tag ofxChilitags::make_tag(int id, const cv::Mat_<cv::Point2f> &corners, int width, int height)
{ 
  Tag t;
  t.id = id;

  t.center = toOf(0.5*(corners(0) + corners(2)));
  t.corners.push_back(toOf(corners(0)));
  t.corners.push_back(toOf(corners(1)));
  t.corners.push_back(toOf(corners(2)));
  t.corners.push_back(toOf(corners(3)));

  t.center_n.set(
      t.center.x / width, 
      t.center.y / height);

  for ( int i = 0; i < t.corners.size(); i++ )
    t.corners_n.push_back(ofVec2f(
        t.corners[i].x / width, 
        t.corners[i].y / height));

  t.dir.set(t.corners_n[0] - t.corners_n[1]);
  t.dir.normalize();
  t.angle = t.dir.angleRad(UP) + PI; 

  return t;
};

//void ofxChilitags::init3d(string intrinsicsFilename)
//{
  //cv::Size calibratedImageSize = chilitags3D.readCalibration(intrinsicsFilename);
  //cv::Mat projectionMat = cv::Mat::zeros(4,4,CV_64F);
  //chilitags3D.getCameraMatrix().copyTo(projectionMat(cv::Rect(0,0,3,3)));
  //cv::Matx44d projection = projectionMat;
  //projection(3,2) = 1;
//}

//void ofxChilitags::update3d(const std::map<int, chilitags::Quad> &tags2d)
//{
  //auto tags3d = chilitags3D.estimate( tags2d );
  //for (auto& tag : tags3d)
  //{
    //string id = tag.first;
    //cv::Matx44d transformation = tag.second;

    //ofVec3f translation(
      //(float)transformation(0,3),
      //(float)transformation(1,3),
      //(float)transformation(2,3));

    //static const cv::Vec4d UNITS[4] {
        //{0.f, 0.f, 0.f, 1.f},
        //{tag_size, 0.f, 0.f, 1.f},
        //{0.f, tag_size, 0.f, 1.f},
        //{0.f, 0.f, tag_size, 1.f},
      //};

    //cv::Vec4f referential[4] = {
        //projection*transformation*UNITS[0],
        //projection*transformation*UNITS[1],
        //projection*transformation*UNITS[2],
        //projection*transformation*UNITS[3],
      //};

    //std::vector<cv::Point2f> t2DPoints;
    //for ( auto homogenousPoint : referential )
    //{
      //t2DPoints.push_back( cv::Point2f(
            //homogenousPoint[0]/homogenousPoint[3],
            //homogenousPoint[1]/homogenousPoint[3] ) );
    //}

    //static const int SHIFT = 16;
    //static const float PRECISION = 1<<SHIFT;
    //static const std::string AXIS_NAMES[3] = { "x", "y", "z" };
    //static const cv::Scalar AXIS_COLORS[3] = {
      //{0,0,255},{0,255,0},{255,0,0},
    //};

    //for ( int i : {1,2,3} )
    //{
      //cv::line(
          //outputImage,
          //PRECISION*t2DPoints[0],
          //PRECISION*t2DPoints[i],
          //AXIS_COLORS[i-1],
          //1, cv::LINE_AA, SHIFT);

      //cv::putText( outputImage, AXIS_NAMES[i-1], t2DPoints[i],
          //cv::FONT_HERSHEY_SIMPLEX, 0.5f, AXIS_COLORS[i-1]);
    //}
  //}
//}
