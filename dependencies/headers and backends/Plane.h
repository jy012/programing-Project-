#pragma once
#include <iostream>
#include <string>
#include <random>
#include <cmath>
#include <chrono>
#include "Position.h"

//Class of aircrafts
class Plane{
  public:
    //Object creators
    Plane();
    Plane(std::string name);
    Plane(string name, double vx, double vy, double vz, Position pos);

    //functions in class
    std::string getplaneName();
    Position getPosition();
    void getSpeed(double &x, double &y, double &z);
    void ping();
    void ping(chrono::steady_clock::time_point time, Position pos);
    chrono::steady_clock::time_point getPingTime();
    Position getPingPos();
    void positionUpdate(chrono::steady_clock::time_point time);
    //debug
    void printall();
    string return_all();
    string info = "No Detected";
    std::chrono::steady_clock::time_point lastDetectedTime = std::chrono::steady_clock::now();
    bool isVisible = false;
    float lastDetectedAngle = 0.0f;
    float lastDetectedDistance = 0.0f;
    
    
  private:
  //data values saved for each aircraft
    std::string m_planeName;
    double m_speedx;
    double m_speedy;
    double m_speedz;
    Position m_pos;
    std::chrono::steady_clock::time_point timeOfLastUpdate;
    std::chrono::steady_clock::time_point timeOfLastPing;
    Position posAtLastPing;
};

using namespace std;

    std::random_device rd;
    // Fallback to system time if random_device isn't non-deterministic
    // auto seed = rd() ? rd() : std::chrono::system_clock::now().time_since_epoch().count(); 
    
    // Seed the random number engine (e.g., Mersenne Twister mt19937)
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    // Seed the engine with that unique time value
    std::mt19937 eng(static_cast<unsigned int>(seed));

    //distribution for the desired range
    uniform_int_distribution<int> distspeed(-300, 300);
    uniform_int_distribution<int> distpos(-90000, 90000);
    uniform_int_distribution<int> distz(1500, 12000);

//Sets Aircraft velocity in x, y, and z
void speeds(double &x, double &y){
    double magnitude;
    int sx, sy;
    do{
        //uses random number within set distribution
        sx = distspeed(eng);
        sy = distspeed(eng);
        //ensures the speed is within reasonable range
        magnitude = sqrt(pow(sx, 2) + pow(sy, 2));
    }
    while(magnitude<100 || magnitude>300);
    //static cast integer value into double before saving in class
    x = static_cast<double>(sx);
    y = static_cast<double>(sy);
    
}
//sets the aircraft position in airspace
void position(double &x, double &y, double &z){
    int x1, y1, z1;
    //uses random number within distribution
    x1 = distpos(eng);
    y1 = distpos(eng);
    //m_pos.getXYZ(x, y, z);
    z1 = distz(eng); 
    //static cast integer value into double before saving in class
    x = static_cast<double>(x1);
    y = static_cast<double>(y1);
    z = static_cast<double>(z1);

}
//Class default creator
Plane::Plane(){
     m_planeName = "Unidentified flying object";
        m_speedx = 0;
        m_speedy = 0;
        m_speedz = 0;
        m_pos = zeroVector();
        timeOfLastUpdate = chrono::steady_clock::now();
        timeOfLastPing = timeOfLastUpdate;
}
//Class called creator
Plane::Plane(string name){
        m_planeName = name;
        speeds(m_speedx, m_speedy);
        m_speedz = 0;
        double m_posx, m_posy, m_posz;
        position(m_posx, m_posy, m_posz);
        m_pos = Position(m_posx, m_posy, m_posz);
        timeOfLastUpdate = chrono::steady_clock::now();
        timeOfLastPing = timeOfLastUpdate;
      
}

Plane::Plane(string name, double vx, double vy, double vz, Position pos) {
    m_planeName = name;
    m_speedx = vx;
    m_speedy = vy;
    m_speedz = vz;
    m_pos = pos;
    timeOfLastUpdate = chrono::steady_clock::now();
    timeOfLastPing = timeOfLastUpdate;
    posAtLastPing = pos; 
}




//Debug data
void Plane::printall(){
    cout << m_speedx << endl;
    cout << m_speedy << endl;
    cout << m_speedz << endl;
    cout << m_pos.showpos() << endl;
}



//gets name
string Plane::getplaneName(){
    return m_planeName;
}
//update position based on aircraft speed and time since last update
void Plane::positionUpdate(chrono::steady_clock::time_point time){

    auto duration = chrono::duration_cast<std::chrono::milliseconds>(time-timeOfLastUpdate); //time since position was last updated
    double seconds = static_cast<double>(duration.count()) / 1000; //convert milliseconds to seconds
    Position distanceTravelled((m_speedx * seconds), (m_speedy * seconds), (m_speedz * seconds)); //the amount of distance travelled
    
    m_pos = m_pos + distanceTravelled; //update position

    timeOfLastUpdate = time;
}
//gets current position of aircraft
Position Plane::getPosition(){ //get the position of the plane
    return m_pos;
}

void Plane::getSpeed(double &x, double &y, double &z){ //get the speed of the plane
    x = m_speedx;
    y = m_speedy;
    z = m_speedz;
}

void Plane::ping(chrono::steady_clock::time_point time, Position pos){ //was used for velocity, not important now
    timeOfLastPing = time;
    posAtLastPing = pos;
}




chrono::steady_clock::time_point Plane::getPingTime(){ //was used for velocity, not important now
    return timeOfLastPing;
}

Position Plane::getPingPos(){ //was used for velocity, not important now
    return posAtLastPing;
}
