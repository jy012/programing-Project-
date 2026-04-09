#pragma once

using namespace std;

class Position{
private:
    double m_x;//north-south
    double m_y; //west-east
    double m_z; //vertical
public:
    Position(){ //zero vector
        m_x = 0;
        m_y = 0;
        m_z = 0;
    }
    Position(double xx, double yy, double zz){ //create new position
        m_x = xx;
        m_y = yy;
        m_z = zz;
    }
    Position(double xx, double yy){ //when we dont care about height
        m_x = xx;
        m_y = yy;
        m_z = 0;
    }

    void getXYZ(double &x, double &y, double &z){ //get x, y, z
        x = m_x;
        y = m_y;
        z = m_z;
    }
    void set_position(double xx, double yy,double zz){ //when we dont care about height
        m_x = xx;
        m_y = yy;
        m_z = zz;
    }
    double getX(){ //get x
        return m_x;
    }
    double getY(){ //get y
        return m_y;
    }
    double getZ(){ //get z
        return m_z;
    }

    string showpos(){ //convert position to a string (for output)
        double x = std::round(m_x * 100) / 100; //round to 2 decimal places
        double y = std::round(m_y * 100) / 100;
        double z = std::round(m_z * 100) / 100;
        string result = format ("{:.2f}, {:.2f}, {:.2f} ",x,y,z);
        return result ;
    }
    
    Position operator+(const Position& right) const;
    Position operator-(const Position& right) const;
};

Position Position::operator+(const Position& operand2) const { //vector addition
    return Position{m_x + operand2.m_x, m_y + operand2.m_y, m_z + operand2.m_z};
}
Position Position::operator-(const Position& operand2) const { //vector subtraction
    return Position{m_x - operand2.m_x, m_y - operand2.m_y, m_z - operand2.m_z};
}

Position zeroVector(){ //get a vector with all zeroes
    return Position(0, 0,0);
}
