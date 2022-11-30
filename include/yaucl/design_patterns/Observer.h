//
// Created by giacomo on 29/11/22.
//

#ifndef YAUCL_GRAPH_OBSERVER_H
#define YAUCL_GRAPH_OBSERVER_H

#include <vector>
#include <functional>

template <typename Message>
class IObservee {
    std::vector<std::function<void(const Message&)>> list_observer_;
public:
    using IObserver = std::function<void(const Message&)>;
    virtual ~IObservee(){};
    virtual void Attach(IObserver observer) {
        list_observer_.push_back(observer);
    }
    virtual void DetachAll()  {
        list_observer_.clear();
    }
    void Notify(const Message& message_)  {
        auto iterator = list_observer_.begin();
        while (iterator != list_observer_.end()) {
            (*iterator)(message_);
            ++iterator;
        }
    }
};

#endif //YAUCL_GRAPH_OBSERVER_H
