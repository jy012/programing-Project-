#include<iostream>
#include<format>
#include<array>
#include<vector>
#include<cmath>
#include<thread>

#include "Radar.h"
#include "Position.h"
#include "Plane.h"
#include "Functions.h"

using namespace std;

vector<Plane> planeList; //list of all planes

Position p(0, 0, 0);
long long range = 100000;
Radar radar(
    p, //position
    0, //minimum angle
    360, //maximum angle
    range, //range
    0,  //elevation
    30, //degrees per second
    34 //gain (dBi)
);

void loop(){
    Position origin = radar.getPosition(); //radar position (never changes)

    while (true){ //makes it loop forever
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); //without this, it runs so fast that the degrees turned is so close to 0 it gets rounded to 0 and the radar never actually turns
        auto nowTime = chrono::steady_clock::now(); //get the current time
        auto duration = chrono::duration_cast<std::chrono::milliseconds>(nowTime - radar.getTime()); //get the time between now and the last time the radar turned (should always be about 1 millisecond)
        radar.updateTime(nowTime); //update the time the radar last turned

        double currentDeg = radar.getDeg(); //get the currente radar angle
        double dPS = radar.getDPS(); //get how fast the radar turns
        double seconds = static_cast<double>(duration.count()) / 1000; //convert the duration from milliseconds to seconds
        double degreesTurned = seconds * dPS; //get how many degrees the radar turned

        radar.updateDeg(currentDeg + degreesTurned); //update current degree of radar


        for (int p{0}; p < planeList.size(); p++){ //loop through planes
            planeList[p].positionUpdate(nowTime); //update the plane position based on its speed and length of time since last update

            //double xp, yp, zp;
            //planeList[p].getPosition(xp, yp, zp);
            //Position pos = {xp, yp, zp}; //position of the plane
            Position pos = planeList[p].getPosition();
            //cout << pos.toString() << endl;

            double planeMagnitude = distanceMagnitude(radar.getPosition(), pos); //distance from the radar to the plane
            double planeFlatDeg; //flat angle between radar and plane
            double planeHeightDeg; //height angle between radar and plane
            bool planeDetected = false; //this is just to prevent having to call the the printPlaneDetected twice for both edge cases

            if (planeMagnitude <= radar.getRange()){ //check if plane is within range
                planeFlatDeg = relativeFlatAngle(pos, radar); //calculate flat angle
                planeHeightDeg = relativeHeightAngle(pos, radar); //calculate height angle
                if ((currentDeg + degreesTurned) > 360){ //for special case (e.g. 300 + 70 = 370, without extra logic it wont detect planes within that 0-10 degree window)
                    double extraDeg = (currentDeg + degreesTurned) - 360; //this wont factor in if it did a full circle more than once, but that is in all likelihood not possible
                    if (extraDeg >= currentDeg){ //radar has pinged every angle
                        planeDetected = true;
                    }
                }
                else { //if not special case
                    if (planeFlatDeg >= currentDeg && planeFlatDeg <= (currentDeg + degreesTurned)){ //if plane lies within the degrees the radar has scanned
                        planeDetected = true;
                    }
                }
            }


            if (planeDetected){
                double SNR = calculateSNR(pos, radar); //calculate the Signal to Noise ratio
                radar.printPlaneDetected(pos, planeFlatDeg, planeHeightDeg, planeMagnitude, SNR, rangeAccuracy(SNR, radar), angularAccuracy(SNR, radar));
            }
        }
    }
}

int main(){
    Plane p1("A1"); planeList.push_back(p1); //create planes
    Plane p2("A2"); planeList.push_back(p2);
    Plane p3("A3"); planeList.push_back(p3);
    Plane p4("A4"); planeList.push_back(p4);
    Plane p5("A5"); planeList.push_back(p5);
    Plane p6("A6"); planeList.push_back(p6);
    Plane p7("A7"); planeList.push_back(p7);
    Plane p8("A8"); planeList.push_back(p8);
    Plane p9("A9"); planeList.push_back(p9);
    Plane p10("A10"); planeList.push_back(p10);

    //p10.printall();

    loop(); //call the loop function
}