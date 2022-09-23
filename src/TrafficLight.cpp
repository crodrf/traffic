#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<mutex> uLock(_mutex);
    _conition.wait(uLock,[this]() {return !_queue.empty();});
    T msg = std::move(_queue.back());
    _queue.pop_back(); //check
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> uLock(_mutex);
    //add a new message to the queue
    _queue.push_back(std::move(msg));
    //send a notification
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
        if(_messages.receive() == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ 
    //should be started in a thread when the public method „simulate“ is called.
    // To do this, use the thread queue in the base class. 
    //when the public method „simulate“ is called

    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green 
    //and sends an update method to the message queue using move semantics. 
    //The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    std::chrono::time_point<std::chrono::system_clock> lastUpdate = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> now;
    std::chrono::duration<double> elapsed_seconds;
    
    //infinite loop template
     while (true)
     {
        //measures time between two loop cycles
        now = std::chrono::system_clock::now();
        elapsed_seconds = now - lastUpdate;
       
        //toggles the current phase of the traffic light between red and green
        if (elapsed_seconds.count() >= 4)
        {
            if (_currentPhase == TrafficLightPhase::red)
            {
                _currentPhase = TrafficLightPhase::green;
            }
            else
            {
                _currentPhase = TrafficLightPhase::red;
            }
            lastUpdate = std::chrono::system_clock::now();
            
            // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
            // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
            // send in conjunction with move semantics.
            _messages.send(std::move(_currentPhase));

            //std::this_thread::sleep_for to wait 1ms 
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } 
     
     }

}

