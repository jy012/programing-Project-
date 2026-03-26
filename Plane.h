#pragma once
#include <string>
//Class of aircrafts
class Plane{
  public:
    //Object creators
    Plane();
    Plane(std::string name);

    //functions in class
    std::string getplaneName();
    void getPosition(double &x, double &y, double &z);
    void positionUpdate();
    //debug
    void printall();
    
  private:
  //data values saved for each aircraft
    std::string m_planeName;
    double m_speedx;
    double m_speedy;
    double m_speedz;
    double m_posx;
    double m_posy;
    double m_posz;
};
