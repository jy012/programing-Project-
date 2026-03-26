#include <iostream>
#include <string>
#include <random>
#include <cmath>
#include <chrono>
#include "plane.h"

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
    uniform_int_distribution<int> distpos(-15000, 15000);
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
        m_posx= 0;
        m_posy = 0;
        m_posz = 0;
}
//Class called creator
Plane::Plane(string name){
        m_planeName = name;
        speeds(m_speedx, m_speedy);
        m_speedz = 0;
        position(m_posx, m_posy, m_posz);
}
//Debug data
void Plane::printall(){
    cout << m_speedx << endl;
    cout << m_speedy << endl;
    cout << m_speedz << endl;
    cout << m_posx << endl;
    cout << m_posy << endl;
    cout << m_posz << endl;
}
//gets name
string Plane::getplaneName(){
    return m_planeName;
}
//update position based on aircraft speed
void Plane::positionUpdate(){
    m_posx = m_posx + (m_speedx);
    m_posy = m_posy + (m_speedy);
}
//gets current position of aircraft
void Plane::getPosition(double &x, double &y, double &z){
    x = m_posx;
    y = m_posy;
    z = m_posz;
}
   




