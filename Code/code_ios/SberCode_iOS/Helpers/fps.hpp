//
//  fps.hpp
//  SberCode_iOS
//
//  Created by Alexander Graschenkov on 01.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#ifndef fps_hpp
#define fps_hpp

#include <stdio.h>
#include <chrono>
#include <list>
#include <vector>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_type;

class FPS {
    
public:
    FPS();
    
    void tick();
    double getFPS();
    
private:
    std::list<time_type> m_time;
};




// считает FPS толко между строчками start() и end()
class FPSIteration {
    
public:
    FPSIteration();
    
    void start();
    void end();
    double getFPS();
    
private:
    std::vector<double> last_times;
    time_type start_time;
    bool started;
};

#endif /* fps_hpp */
