#ifndef LOCALIZATION_H
#define	LOCALIZATION_H

#include "DataTypes.h"
#include <vector>
#include <map>
#include <opencv2/opencv.hpp>

using namespace std;

class IdDist {
public:
    double id;
    double dist;
};

class FindOnWay {
public:
    Ll pointFound;
    double pointId1;
    double pointId2;
    int pathId;
    int pathPosition;
};

class WayPoint : public Ll {
public:
    vector<double> nextPoints;

    //for searching
    double dist;
    double previous;

    WayPoint() {
        dist = DBL_MAX;
        previous = -1;
    };

    WayPoint(Ll src) {
        dist = DBL_MAX;
        previous = -1;
        this->latitude = src.latitude;
        this->longitude = src.longitude;
    };
};

class Path {
public:
    vector<double> points;
};

class BoundsLl {
public:
    double minlat;
    double minlon;
    double maxlat;
    double maxlon;
};

class LocalizationAndPlaning {
public:
    vector<Path > paths;
    map< double, WayPoint> points;
    BoundsLl bounds;

    int guiMapWidth;
    int guiMapHeight;
    int guiDebugWidth;
    int guiDebugHeight;

    //main points
    Ll destinationPoint;
    Ll curPoint;
    Ll headingPoint;

    //last known gps position
    Ll lastPosition;

    LocalizationAndPlaning(int guiWidth, int guiHeight);
    virtual ~LocalizationAndPlaning();
    //read osm map from xml
    void readMap(char* filename);
    //gets image of map and main points
    IplImage* getGui();
    //destination set
    void setDestination(Ll point);
    void readDestination(char* filename);
    //update state with new gps data
    GpsAngles update(Ll gps);
    //calculate shortest path between two paths
    void calcPath(double strtPoint, double strtPointB, double destPoint, double destPointB);

    //bool distCompare(IdDist i, IdDist j);
    Ll reverse(Point location);

private:
    //shortest path to destination
    vector<double> bestWay;
    
    //ellipse parameters
    double ell_a;
    double ell_b;
    
    //sphere radius
    double EarthRadius;
    
    //km radius of ellipse for heading point calculation
    double heading_search_radius;
    
    //fonts for gui
    CvFont font;
    CvFont fontBig;
    
    //calc point to linesegment distance and closest point on segment to target point
    pair<double, Ll> dist_point_linesegment(Ll point, Ll start, Ll end);
    //calc intersections of a line and ellipse
    pair<Ll, Ll> ellipseLineIntersection(Ll p0, Ll p1);
    //najde bod segmentu a segmente najblizsie k bodu point
    FindOnWay find_on_way(Ll point);
    //lon,lat to map x,y
    Point convert(Ll point);
    //distance between 2 points on a sphere
    double distance(Ll p1, Ll p2);
    //calculates bearing (initial)
    double calc_bearing(Ll a, Ll b);
    //calculates intersection when going from p1 with initial bearing b1 and path from p2 along bearing b2
    Ll intersection_of_bearings(Ll p1,double b1,Ll p2,double b2);
    //calc ellipse parameters
    void calcEllipse(Ll point, double km);
    //calc heading point
    Ll calcHeadingPoint();

};

#endif	/* LOCALIZATION_H */

