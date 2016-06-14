//
//  QLearningRobot.cpp
//  Q-Learning
//
//  Created by 黄正跃 on 5/3/16.
//  Copyright © 2016 黄正跃. All rights reserved.
//
#include "QLearningRobot.h"
#include <random>
#include <math.h>
#include <iostream>

void QLearningRobot::processNext(){

    Evironment *evironment=Evironment::getInstance();

    int action=0;
    double sample=0;
    double nowQvalue=0;

    iter++;
    //next action depends on epison and the current policy
    action=this->takeNextAction(posNow);

    valueMap[posNow.x][posNow.y].countPlusOne(action);

    EvironmentFeedBack feedBack=evironment->getNextState(posNow, action);


    if(feedBack.isTerminal){
        for(int i=0;i<4;i++){
            valueMap[posNow.x][posNow.y].setQValue(i,feedBack.reward);
        }
        valueMap[posNow.x][posNow.y].updateValue();


        //take a random place to start

        static std::default_random_engine randEngine(static_cast<int>(time(0)));
        static std::uniform_int_distribution<int> I(0,11);
        while(true){
            int temp=I(randEngine);
            int tempx=temp%4;
            int tempy=temp/4;
            if(!evironment->isTerminal(Vect2(tempx, tempy)) && evironment->isValid(Vect2(tempx, tempy))){
                startX=tempx;
                startY=tempy;
                break;
            }
        }

        gameTime++;
        posNow=Vect2(startX,startY);
        return;
    }

    sample=discount* valueMap[feedBack.position.x][feedBack.position.y].getValue()+feedBack.livingReward;


    nowQvalue=valueMap[posNow.x][posNow.y].getQValue(action);
//    static int step=0;
//    step++;
//    alphaNow=alpha/sqrt(step);
//    std::cout<<"debug:"<<alphaNow<<std::endl;
    alphaNow*=0.9999;
//      alphaNow-=0.000001;
//    if(alphaNow<0){
//        alphaNow=0;
//    }
    valueMap[posNow.x][posNow.y].setQValue(action, (1-alphaNow)*nowQvalue+ alphaNow*sample);

    //go next state
    posNow=feedBack.position;
}



int QLearningRobot::takeNextAction(const Vect2& pos){
    int action=0;
    //random engine
    static std::default_random_engine randEngine(static_cast<int>(time(0)));
    static std::bernoulli_distribution T(1-epison);
    static std::uniform_real_distribution<double> R(0,1);
    static std::uniform_int_distribution<int> I(0,3);

    if(R(randEngine) > epison){
        //exploit
        action=valueMap[pos.x][pos.y].getPolicy();

    }else{
        //explorer
        action=I(randEngine);

    }

    return action;

}

void QLearningRobot::runNextIteration(int delta){
    int end=iter+delta;
    while (end>iter) {
        processNext();
    }
}
