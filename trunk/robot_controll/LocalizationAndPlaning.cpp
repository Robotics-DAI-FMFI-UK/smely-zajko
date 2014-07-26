/* 
 * File:   Localization.cpp
 * Author: myron
 * 
 * Created on Štvrtok, 2010, september 9, 10:17
 */

#include "LocalizationAndPlaning.h"

#include "../rapidxml-1.13/rapidxml.hpp"
#include "../rapidxml-1.13/rapidxml_utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <set>

#include <cmath>
#include <float.h>
//#include <opencv/cxtypes.h>
#include <opencv2/core/types_c.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include <time.h>

using namespace std;
using namespace rapidxml;


LocalizationAndPlaning::LocalizationAndPlaning(int guiWidth, int guiHeight) {
    this->guiWidth = guiWidth;
    this->guiHeight = guiHeight;

    headingPointId = -1;
}

LocalizationAndPlaning::~LocalizationAndPlaning() {
}

void LocalizationAndPlaning::readMap(char* filename){

    ifstream is;
    is.open (filename);

    char* str;

    if (!is.good()){
        printf("cannot read map file \n" );
        exit(-1);
    }

    // get length of file:
    is.seekg (0, ios::end);
    long length = is.tellg();
    is.seekg (0, ios::beg);

    str = new char[length+1];
    for(int i=0; i<length+1; i++){ 
        str[i] = 0;
    }

    is.read( str, length );

    is.close();

    //printf("%s", str);

    xml_document<> doc;    // character type defaults to char
    doc.parse<0>( str );    // 0 means default parse flags

    xml_node<>* base_node = doc.first_node("osm");

    //bounds
    xml_node<>* bounds_node = base_node->first_node("bounds");

    istringstream isstr1( bounds_node->first_attribute("minlat")->value() );
    isstr1 >> bounds.minlat;

    istringstream isstr2( bounds_node->first_attribute("maxlat")->value() );
    isstr2 >> bounds.maxlat;

    istringstream isstr3( bounds_node->first_attribute("minlon")->value() );
    isstr3 >> bounds.minlon;

    istringstream isstr4( bounds_node->first_attribute("maxlon")->value() );
    isstr4 >> bounds.maxlon;

    //find footways
    for (xml_node<>* way_node = base_node->first_node("way"); way_node; way_node = way_node->next_sibling("way") ){
        int candidate = 0;
        for (xml_node<>* tag_node = way_node->first_node("tag"); tag_node; tag_node = tag_node->next_sibling("tag") ){

            xml_attribute<> *attr_v = tag_node->first_attribute("v");

            if( attr_v==0 ){
                continue;
            }

            if ((strcmp(attr_v->value(), "footway" )==0 )
                || (strcmp(attr_v->value(), "path" ) == 0)
                || (strcmp(attr_v->value(), "service" ) == 0)    )
            {
                candidate++;
            }

            if (strcmp(attr_v->value(), "sand") == 0)
                candidate--;
            if (strcmp(attr_v->value(), "grass") == 0)
                candidate--;

        }
        if (candidate > 0)
        {
                Path path;

                //read all nodes
                for (xml_node<>* nd_node = way_node->first_node("nd"); nd_node; nd_node = nd_node->next_sibling("nd") ){
                    xml_attribute<>* attr_ref = nd_node->first_attribute("ref");

                    if( attr_ref==0 ){
                        continue;
                    }

                    //search in 'osm' node for 'node' elements
                    for (xml_node<>* node = base_node->first_node("node"); node; node = node->next_sibling("node") ){

                        xml_attribute<>* attr_id = node->first_attribute("id");
                        if( attr_id==0 ){
                            continue;
                        }

                        if( strcmp(attr_id->value(), attr_ref->value() )==0 ){

                            WayPoint point;


                            istringstream istr1( node->first_attribute("lat")->value() );
                            istr1 >> point.latitude;


                            istringstream istr2( node->first_attribute("lon")->value() );
                            istr2 >> point.longitude;

                            double id = atof( node->first_attribute("id")->value() );

                            points[id] = point;

                            path.points.push_back(id);

                        }
                    }

                }

                paths.push_back(path);            

        }
    }

    doc.clear();

    delete[] str;

    for(int i=0; i<paths.size(); i++){   

        for(int j=1; j<paths[i].points.size()-1; j++ ){
            double id1 = paths[i].points[j];
            double id2 = paths[i].points[j-1];
            double id3 = paths[i].points[j+1];

            points[id1].nextPoints.push_back(id2);
            points[id1].nextPoints.push_back(id3);
        }
        //first
        double id0 = paths[i].points[0];
        double id1 = paths[i].points[1];

        points[id0].nextPoints.push_back(id1);

        if(paths[i].points.size()>=2){
            //last
            id0 = paths[i].points[paths[i].points.size()-1];
            id1 = paths[i].points[paths[i].points.size()-2];

            points[id0].nextPoints.push_back(id1);
        }
    }
}

double LocalizationAndPlaning::dist_point_linesegment(double x, double y, double x1, double y1, double x2, double y2, double *nx, double *ny)
{
    double l = (y - y1) * (y1 - y2) - (x1 - x) * (x1 - x2);
    l /= (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
	*nx = x1 + l * (x1 - x2);
	*ny = y1 + l * (y1 - y2);
	double d1 = (*nx - x) * (*nx - x) + (*ny - y) * (*ny - y);
	double dA = (x1 - *nx) * (x1 - *nx) + (y1 - *ny) * (y1 - *ny);
	double dB = (x2 - *nx) * (x2 - *nx) + (y2 - *ny) * (y2 - *ny);
	double dAB = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
	double d2 = (x - x1) * (x - x1) + (y - y1) * (y - y1);
	double d3 = (x - x2) * (x - x2) + (y - y2) * (y - y2);
	if ((dA > dAB) || (dB > dAB)) d1 = d2 + d3;
	if ((d1 <= d2) && (d1 <= d3)) return d1;
	if (d2 < d3)
	{
		*nx = x1;
		*ny = y1;
		return d2;
	}
	*nx = x2;
	*ny = y2;
	return d3;
}

FindOnWay LocalizationAndPlaning::find_on_way(Ll point){

    FindOnWay result;
    //Ll pointFound;
    double dist = DBL_MAX;

    for(int i=0; i<paths.size(); i++){
        for(int j=1; j<paths.at(i).points.size(); j++ ){
            Ll tmp;
            double id1 = paths.at(i).points[j];
            double id2 = paths.at(i).points[j-1];
            double d = dist_point_linesegment(
                point.longitude, point.latitude,
                points.at(id1).longitude, points.at(id1).latitude,
                points.at(id2).longitude, points.at(id2).latitude,
                &tmp.longitude, &tmp.latitude
            );

            if( d<dist ){
                dist = d;
                result.pointFound = tmp;
                result.pathId = i;
                result.pointId1 = id1;
                result.pointId2 = id2;
                result.pathPosition = j;
            }
        }
    }

    return result;
}

double LocalizationAndPlaning::add_point_on_way(Ll point){

    FindOnWay fw;
    fw = find_on_way( point );

    WayPoint pointFound(fw.pointFound);

    //new point id
    double newID = 0;
    map<double,WayPoint>::iterator it;

    for ( it=points.begin() ; it != points.end(); it++ ){
        if( (*it).first > newID ){
            newID = (*it).first;
        }
    }
    newID++;

    //insert
    pointFound.nextPoints.push_back(fw.pointId1);
    pointFound.nextPoints.push_back(fw.pointId2);
    points[newID] = pointFound;


    paths[fw.pathId].points.insert( paths[fw.pathId].points.begin()+fw.pathPosition, newID);

    points[fw.pointId1].nextPoints.push_back(newID);
    points[fw.pointId2].nextPoints.push_back(newID);

    //to erase
    vector<double>::iterator it2;

    for ( it2=points[fw.pointId1].nextPoints.begin() ; it2 != points[fw.pointId1].nextPoints.end(); it2++ ){
        if( (*it2)==fw.pointId2 ){
            points[fw.pointId1].nextPoints.erase( it2 );
            break;
        }
    }

    for ( it2=points[fw.pointId2].nextPoints.begin() ; it2 != points[fw.pointId2].nextPoints.end(); it2++ ){
        if( (*it2)==fw.pointId1 ){
            points[fw.pointId2].nextPoints.erase( it2 );
            break;
        }
    }

    return newID;
}

Point LocalizationAndPlaning::convert(Ll point){

    double width = bounds.maxlon - bounds.minlon;
    double height = bounds.maxlat - bounds.minlat;

    Point result;

    result.x = ((point.longitude - bounds.minlon) / width) * guiWidth;
    result.y = ( (1- (point.latitude - bounds.minlat) / height) ) * guiHeight;

    return result;
}

Ll LocalizationAndPlaning::reverse(Point location)
{
    Ll result;
    result.longitude = (double)location.x / (double)guiWidth * (double)(bounds.maxlon - bounds.minlon) + bounds.minlon;
    result.latitude = (double)location.y / (double)guiHeight * (double)(bounds.maxlat - bounds.minlat) + bounds.minlat;
    return result;
}

IplImage* LocalizationAndPlaning::getGui(){
    IplImage* result = cvCreateImage( cvSize(guiWidth, guiHeight),32, 3 );

    cvFillImage( result,255);
        
    for(int i=0; i<paths.size(); i++){
        if( paths.at(i).points.size()>1 ){
            Point p1 = convert( points.at( paths.at(i).points[0] )  );

            for(int j=1; j<paths.at(i).points.size(); j++ ){
                Point p2 = convert(points.at( paths.at(i).points[j] ) );

                cvLine( result, cvPoint(p1.x,p1.y), cvPoint(p2.x,p2.y), cvScalar(0, 0, 0));
                p1 = p2;
            }
        }
    }

    //best way
    if( bestWay.size()>0 ){
        Point p1 = convert( points[ bestWay[0] ]  );
        for(int i=1; i<bestWay.size(); i++){
            Point p2 = convert( points[ bestWay[i] ]  );

            cvLine( result, cvPoint(p1.x,p1.y), cvPoint(p2.x,p2.y), cvScalar(i*((double)1/bestWay.size()), 0, 0), 3);
            p1 = p2;
        }
    }

    Point o2 = convert( lastPosition  );
    cvCircle( result, cvPoint(o2.x, o2.y ), 5, cvScalar(0, 1, 0) );

    Point o3 = convert( points[ destId ]  );
    cvCircle( result, cvPoint(o3.x, o3.y ), 5, cvScalar(0, 0, 1) );

    if( headingPointId>0 ){
        Point o1 = convert( points[ headingPointId ]  );
        cvCircle( result, cvPoint(o1.x, o1.y ), 5, cvScalar(0, 0, 0) );
    }

    if( curPoint>0 ){
        Point o1 = convert( points[ curPoint ]  );
        cvCircle( result, cvPoint(o1.x, o1.y ), 5, cvScalar(0, 1, 1) );
    }

//    map<double,WayPoint>::iterator it;
//    for ( it=points.begin() ; it != points.end(); it++ ){
//        if((*it).second.dist!=DBL_MAX ){
//            Point o5 = convert( (*it).second  );
//            cvCircle( result, cvPoint(o5.x, o5.y ), 5, cvScalar(0, 1, 1),2 );
//        }
//    }

    //TEST
    //Point o1 = convert( points[ testid1 ]  );
    //Point o2 = convert( points[ testid2 ]  );

//    cvCircle( result, cvPoint(o1.x, o1.y ), 5, cvScalar(0, 0, 0) );
//    cvCircle( result, cvPoint(o2.x, o2.y ), 5, cvScalar(0, 0, 0) );

    //TODO: pozicia, pozicia na ceste, smer otocenia, smer k cielu
    return result;
}

IplImage* LocalizationAndPlaning::getGui(Ll point){

    IplImage* result = getGui();

    //if( point!=NULL ){
        Point o4 = convert( point  );
        cvCircle( result, cvPoint(o4.x, o4.y ), 5, cvScalar(0, 0, 1),2 );
    //}

    return result;
}

void LocalizationAndPlaning::setDestination(Ll point){

    //destId = add_point_on_way( point );

    FindOnWay fw;
    fw = find_on_way( point );
    if( distance( fw.pointFound, points[fw.pointId1] ) < distance( fw.pointFound, points[fw.pointId2] ) ){
        destId = fw.pointId1;
    }else{
        destId = fw.pointId2;
    }


}

void LocalizationAndPlaning::readDestination(char* filename){

    Ll point;

    ifstream is;
    is.open (filename);

    char buff[128];

    while( is.good() ){
        is.getline(buff, 128);

        string s(buff);
        string s2;

        istringstream iss(s);

        iss >> s2;

        if( s2 == "latitude" ){
            iss >> point.latitude;
        }else if ( s2 == "longitude" ){
            iss >> point.longitude;
        }

    }

    setDestination(point);
}


double LocalizationAndPlaning::distance(Ll p1, Ll p2){
    return sqrt( pow( p1.latitude - p2.latitude,2) + pow( p1.longitude - p2.longitude,2) );
}

double LocalizationAndPlaning::distance2(Ll p1, Ll p2){
    double result = 0;

    double dLat1InRad = p1.latitude * (M_PI / 180);
    double dLong1InRad = p1.longitude * (M_PI / 180);
    double dLat2InRad = p2.latitude * (M_PI / 180);
    double dLong2InRad = p2.longitude * (M_PI / 180);

    double dLongitude = dLong2InRad - dLong1InRad;
    double dLatitude = dLat2InRad - dLat1InRad;

    double a = pow( sin( dLatitude / 2 ), 2) * cos(dLat1InRad) * cos(dLat2InRad) * pow( sin(dLongitude / 2) , 2);
    double c = 2* atan2( sqrt(a), sqrt(1-a ));

    double kEarthRadiusKms = 6376.5;

    result = kEarthRadiusKms * c;
}

double LocalizationAndPlaning::wayDistance(vector<double> w){
    double result = 0;
    for(int i=1; i<w.size(); i++ ){
        double id1 = w[i];
        double id2 = w[i-1];

        result += distance( points[id1], points[id2] );
    }

    return result;
}

bool distCompare(IdDist i, IdDist j){
    return (i.dist<j.dist);
}

void LocalizationAndPlaning::findWay2(double destPoint, double curPoint){
    //vector<long> usedPoints;
    set<double> usedPoints;

    list<double> toProcess;

//    for(int i=0; i<points[curPoint].nextPoints.size(); i++ ){
//        toProcess.push_back( points[curPoint].nextPoints[i] );
//    }

    map<double,WayPoint>::iterator it;
    for ( it=points.begin() ; it != points.end(); it++ ){
        (*it).second.dist = DBL_MAX;
    }

    toProcess.push_back( curPoint );
    //usedPoints.insert( curPoint );

    points[curPoint].dist = 0;

    //process points
    while( toProcess.size()>0 ){
        double id = toProcess.front();
        toProcess.pop_front();

//        if( id==destPoint ){
//            break;
//        }

        //usedPoints.insert( id );

//        printf(">> d:%f\n", points[id].dist);
//
//        IplImage* img = getGui( points[id]);
//
//        cvShowImage( "test", img );
//        cvStartWindowThread();
//
//        //usleep( 50000 );
//
//        cvReleaseImage( &img );

        WayPoint w = points[id];

        vector<IdDist> toProcess2;

        for(int i=0; i<w.nextPoints.size(); i++ ){

            double idn = w.nextPoints[i];

            //if( usedPoints.count( idn )==0 ){ //usedPoints.find( idn )==usedPoints.end() ){

                double dist = distance( w, points[idn] );
                dist += w.dist;

                if( dist<points[ idn ].dist ){

                    IdDist tmp;
                    tmp.dist = dist;
                    tmp.id = idn;
                    toProcess2.push_back( tmp );
                }
            //}
        }

        sort( toProcess2.begin(), toProcess2.end(), distCompare );

        vector<IdDist>::iterator it2;
        for(it2 = toProcess2.begin(); it2!=toProcess2.end(); it2++  ){
            toProcess.push_back((*it2).id );
            points[ (*it2).id  ].dist = (*it2).dist;
            points[ (*it2).id  ].previous = id;

            //usedPoints.insert( (*it2).id );
        }

    }

    //read path
    usedPoints.clear();
    toProcess.clear();

    toProcess.push_back( destPoint );
    vector<double>path;
    path.push_back( destPoint );

    while( toProcess.size()>0 ){
        double id = toProcess.front();
        toProcess.pop_front();

        if( id==curPoint ){
            path.push_back( curPoint );
            break;
        }

        
        WayPoint w = points[id];
        if( w.previous!=-1 && usedPoints.count( w.previous )==0 ){
            path.push_back( w.previous );
            toProcess.push_back( w.previous );
        }
        usedPoints.insert( id );
        //printf("> d:%ld %ld %d\n", id, w.previous, w.nextPoints.size());

    }

    bestWay = path;
}


GpsAngles LocalizationAndPlaning::update(Ll gps){

    //TODO: remove direction computing

    static int cntsame = 0;
    GpsAngles result;

    lastPosition = gps;

    FindOnWay fw;
    fw = find_on_way( gps );

    if ((wasCurPoint != curPoint) & (cntsame >= 10))
      lastCurPoint = wasCurPoint;

    if (curPoint == wasCurPoint) cntsame++;
    else cntsame = 0;

    wasCurPoint = curPoint;

    if( distance( fw.pointFound, points[fw.pointId1] ) < distance( fw.pointFound, points[fw.pointId2] ) ){
        curPoint = fw.pointId1;
    }else{
        curPoint = fw.pointId2;
    }

//    Ll vector;
//    vector.latitude = points[curPoint].latitude - points[lastCurPoint].latitude;
//    vector.longitude = points[curPoint].longitude - points[lastCurPoint].longitude;
//    double angleGps = atan2( vector.longitude, vector.latitude);

    //result.gps = angleGps;
    
    if( bestWay.size()==0 ){
        bestDist = DBL_MAX;

        if(curPoint != destId){
            time_t t = time(NULL);
            //printf("1 new cur: %ld\n", curPoint);
            findWay2(destId, curPoint );
            time_t t2 = time(NULL);
            //printf("best waysize: %d time: %d \n", bestWay.size(), t2-t); //67, 32
        }
    }else{

        //zisti ci su v bestWay
        int point1pos = -1;
        int point2pos = -1;

        for(int i=0; i<bestWay.size(); i++ ){
            if( bestWay[i]==fw.pointId1 ){
                point1pos = i;
            }
            if( bestWay[i]==fw.pointId2 ){
                point2pos = i;
            }
        }

        double angle;

        if( point1pos<point2pos && point1pos!=-1 ){
            headingPointId = fw.pointId1;
        }else if( point1pos>point2pos && point2pos!=-1 ){
            headingPointId = fw.pointId2;
        } else{
            headingPointId = -1;
            bestWay.clear();
            result.map = DBL_MIN;
        }

        if(headingPointId!= -1){
            Ll vec;
            vec.latitude = points[headingPointId].latitude - fw.pointFound.latitude;
            vec.longitude = points[headingPointId].longitude - fw.pointFound.longitude;

            angle = atan2( vec.longitude,vec.latitude );
            angle *= (180/ M_PI);

            //printf("angle %f  vec :%f %f\n", angle, vec.latitude, vec.longitude);

            if( distance(points[destId], gps )<0.00005 ){
                printf("destination \n");
                result.map = DBL_MAX;
            }

            result.map = angle;

        }
        //ak ano smer je vektor od aktualnej pozicie ku bodu ktory ma vecsiu poziciu v bestWay
        //ak nema tak ku poslednemu bodu ktory bol pouzity pre smer
    }

    return result;
};