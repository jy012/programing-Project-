#pragma once

#include<iostream>
#include<array>
#include<vector>
#include<cmath>

#include "Radar.h"
#include "Position.h"
#include "Plane.h"

using namespace std;

int earthRadius = 6371000; // radius of the earth in meters
int d = 2 * earthRadius; //earth diamater

double pi = 3.14159265359; //approximate pi value
double c = 299792458; //speed of light (m/s)
double k = 0.00000000000000000000001380649; //Boltzmann's constant

double degToRad(double deg){ //convert degrees to radians
    double rad = deg * (pi / 180);
    return rad;
}
double radToDeg(double rad){ //convert radians to degrees
    double deg = rad * (180 / pi);
    return deg;
}

double heightCalculation(double distance, double degrees){ //account for curvature of earth (probably not needed)
    double drop = (pow(distance, 2)) / d;
    double heightFromElevation = tan(degrees) * distance;
    double totalHeightAtDistance = drop + heightFromElevation;
    return totalHeightAtDistance;
}

double distanceMagnitude(Position origin, Position pos){ //find distance between two positions
    Position vector = pos - origin;
    double x,y,z;
    vector.getXYZ(x,y,z);
    return (sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)));
}
double distanceMagnitude(double x, double y){ //magnitude without height for flat distance
    return sqrt(pow(x, 2) + pow(y, 2));
}

double relativeFlatAngle(Position pos, Radar radar){ //find flat ground angle between two points
    Position vector = pos - radar.getPosition();
    double x,y,z;
    vector.getXYZ(x, y, z);
    //double x = vector.getX(); double y = vector.getY(); double z = vector.getZ();

    double angle = radToDeg(atan2(y, x));
    //to explain the next bit:
    //  atan2 does not go from 0-360, it actually goes from -180 to 180, and the next logic transforms this into the regular 0 to 360 range
    if (angle >= 0){ //0 to 180 proceeds as normal
        return angle;
    }
    else if (angle < 0){ //if its between -180 and 0, make it postive and then add 180 to create the 180 to 360 range
        return abs(angle) + 180;
    }
    return 0; //this should litreally never happen, this is just so the code actually compiles
}

double relativeFlatAngle_D(Position pos, Radar radar){
    Position vector = pos - radar.getPosition();
    double x,y,z;
    vector.getXYZ(x, y, z);
    //double x = vector.getX(); double y = vector.getY(); double z = vector.getZ();

    double angle = radToDeg(atan2(y, x));
    //to explain the next bit:
    //  atan2 does not go from 0-360, it actually goes from -180 to 180, and the next logic transforms this into the regular 0 to 360 range
    if (angle >= 0){ //0 to 180 proceeds as normal
        return angle;
    }
    else if (angle < 0){ //if its between -180 and 0, make it postive and then add 180 to create the 180 to 360 range
        return abs(angle) + 180;
    }
    return 0; //this should litreally never happen, this is just so the code actually compiles
}

double relativeHeightAngle(Position pos, Radar radar){ //find height angle between two points
    Position vector = pos - radar.getPosition();
    double x, y, z;
    //vector.getXYZ(x, y, z);

    double magnitude = distanceMagnitude(pos, radar.getPosition());
    double height = vector.getZ();

    // sin devuelve radianes entre -pi y pi
    double angle = radToDeg(asin(height / magnitude));
    
    return angle;
}

double relativeHeightAngle_D(Position pos, Radar radar){ //find height angle between two points
   
    Position vector = pos - radar.getPosition();
    double x, y, z;
    //vector.getXYZ(x, y, z);

    double magnitude = distanceMagnitude(pos, radar.getPosition());
    double height = vector.getZ();

    // sin devuelve radianes entre -pi y pi
    double angle = radToDeg(asin(height / magnitude));
    
    return angle;
}

double calculateSNR(Position pos, Radar radar){ //calculate the linear Signal to Noise ratio of the recieved radio waves
    Position radarPos = radar.getPosition(); //get the position of the radar
    array<int, 2> bandWidth = radar.getBandwidth(); //get the upper and lower frequencies of the radar
    double midFrequency = (bandWidth[0] + bandWidth[1]) / 2; //find the middle frequency
    double wavelength = c / (midFrequency * 1000000); //transform midFrequency to Hertz (instead of MHz) and use the speed of light to find the wavelength
    double noiseBandwidth = 1 / radar.getPulseWidth(); //get the noise bandwidth of the radio waves
    double radarCrossSection = 100; //assumed, for boeing 747
    double signalLoss = 1; //assumed
    double transmitPower = 2000000; //assumed, 2 mega watts
    double noiseTemperature = 300; //assumed, degrees kelvin
    double distance = distanceMagnitude(radarPos, pos); //distance between plane and radar

    double SNR = (transmitPower * pow(radar.getGain(), 2) * pow(wavelength, 2) * radarCrossSection) / (pow(4*pi, 3) * k * noiseTemperature * noiseBandwidth * signalLoss * pow(distance, 4));
    return SNR;
}

double rangeAccuracy(double SNR, Radar radar){ //get the approximate accuracy of the distance to the plane
    double noiseBandwidth = 1 / radar.getPulseWidth(); //get the noise bandwidth of the radio waves
    double sigmaR = c / (2 * noiseBandwidth * (sqrt(2 * SNR))); //find accuracy of range
    return sigmaR;
}

double angularAccuracy(double SNR, Radar radar){ //find the approximate accuracy of the angles between the radar and the plane
    array<int, 2> bandWidth = radar.getBandwidth(); //get the upper and lower frequencies of the radar
    double midFrequency = (bandWidth[0] + bandWidth[1]) / 2; //find the middle frequency
    double wavelength = c / (midFrequency * 1000000); //transform midFrequency into Hertz (instead of MHz) and use the speed of light to find the wavelength
    double diameter = radar.getDiameter(); //get diamater of the radar
    double beamWidth = wavelength / diameter; //find the beamwidth of the radio waves using the wavelength and radar diamater
    double km = 1; //accuracy constant, assumed

    double sigmaTheta = beamWidth / (km * sqrt(2 * SNR)); //get angular accuracy
    return sigmaTheta;
}
