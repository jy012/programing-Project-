#pragma once

#include<array>
#include<chrono>
#include<format>
#include<iostream>
#include <cmath>
#include "Position.h"
#include "Plane.h"

using namespace std;

class Radar{
private:
    Position position; //position of radar
    int minDeg; //the minimum degree the radar will rotate to
    int maxDeg; //the maximum degree the radar will rotate to
    double range; //maximum range of radar
    double elevation; // degrees above horizontal the radar can see
    double degPerSec; //how fast the radar rotates
    double currentDeg = 10; //the current degree the radar is at
    int bandWidthLower = 2700; //bandwidth the radar operates at (S-Band, 2700-2900)
    int bandWidthUpper = 2900; //bandwidth the radar operates at
    double gain; //antenna gain
    double pulseWidth = 0.000001; //1 microsecond
    double radarDiameter = 5; //assumed
    chrono::steady_clock::time_point timeOfLastUpdate; //the last time the rotation of this radar was updated
public:
    Radar(Position pos, int miD, int maD, double rg, double ev, double dPS, double g){
        position = pos;
        minDeg = miD;
        maxDeg = maD;
        range = rg;
        elevation = ev;
        degPerSec = dPS;
        gain = g;
        timeOfLastUpdate = chrono::steady_clock::now();
    }

    void updateDeg(double newDeg){ //update degrees of the radar
        currentDeg = newDeg;
        if (currentDeg > 360){ //make sure it stays within 0-360 degrees
            currentDeg -= 360;
        }
    }
    void updateTime(chrono::steady_clock::time_point newTime){ //update the time the radar last updated
        timeOfLastUpdate = newTime;
    }
    double getDeg(){ //get current radar angle
        return currentDeg;
    }
    double getDPS(){ //get the turn speed of radar
        return degPerSec;
    }
    double getRange(){ //get the range of radar
        return range;
    }
    array<int, 2> getBandwidth(){ //get the bandwidth range of radar
        array<int, 2> bandWidth = {bandWidthLower, bandWidthUpper};
        return bandWidth;
    }
    double getGain(){ //get gain of radar
        return gain;
    }
    double getPulseWidth(){ //get the pulse width of the radar
        return pulseWidth;
    }
    double getDiameter(){ //get the radar diameter
        return radarDiameter;
    }
    Position getPosition(){ //get the radar position
        return position;
    }
    chrono::steady_clock::time_point getTime(){ //get the last update time
        return timeOfLastUpdate;
    }

    void setRange(double s_range) {
        range = s_range;
    }

    void setDPS (double s_dps){
        degPerSec = s_dps;
    }

    void printPlaneDetected(Position planePos, double fdeg, double hdeg, double magnitude, double SNR, double rangeAccuracy, double angleAccuracy){ //this is just to display all the information the radar has about the plane

        cout << format("Plane detected:\nPosition: ({}) meters\nAngle: {:.2f} degrees flat, {:.2f} degrees high\nMagntiude: {:.2f} meters\nSNR: {:.2f} S/N, ({:.2f} decibels)\nRange Accuracy: {:.2f} meters, Angular Accuracy: {:.4f} degrees\n\n",
            planePos.showpos(), fdeg, hdeg, magnitude, SNR, 10 * std::log10(SNR), rangeAccuracy, angleAccuracy);
    }


    string printPlaneDetected_final(Position planePos, double fdeg, double hdeg, double magnitude, double SNR, double rangeAccuracy, double angleAccuracy){ //this is just to display all the information the radar has about the plane
         return std::format(
        "\nPlane detected:\nco-ords:{} m\nAngle: \n *degrees flat {:.2f} \n *degrees high {:.2f} \n"
        "Magntiude: {:.2f} meters\nSNR: {:.2f} S/N, ({:.2f} decibels)\n"
        "Range Accuracy: {:.2f} meters \nAngular Accuracy: {:.4f} degrees\n\n",
        planePos.showpos(), fdeg, hdeg, magnitude, SNR, 10 * std::log10(SNR), rangeAccuracy, angleAccuracy
    );

    }
};
