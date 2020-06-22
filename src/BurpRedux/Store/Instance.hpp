#pragma once

#include <functional>
#include "../Publisher/Instance.hpp"
#include "../Reducer.hpp"
#include "Interface.hpp"

namespace BurpRedux {
  namespace Store {

    template <class State, class Action, size_t size>
    class Instance : public Interface<State, Action> {

      public:

        Instance(Reducer<State, Action> & reducer) :
          publisher(),
          reducer(reducer),
          reducing(false),
          notifying(false),
          nextState(nullptr)
        {}

        void setup(const State * state) {
          publisher.setup(state);
        }

        void loop() override {
          // Notify asynchronously so that 
          // actions can be batched synchronously.
          // State reduction is always synchronous
          if (nextState) {
            publish(nextState);
          }
        }

        Error dispatch(const Action & action) override {
          if (reducing) {
            // prevent dispatch during reduce and report error
            return Error::dispatchDuringReduceError;
          }
          Error error = Error::noError;
          if (notifying) {
            // don't prevent dispatch during notification but
            // report warning so that users can detect when it happens
            error = Error::dispatchDuringNotificationWarning;
          }
          reducing = true;
          nextState = reducer.reduce(publisher.getState(), action);
          reducing = false;
          return error;
        }

        bool subscribe(Subscriber<State> * subscriber) override {
          return publisher.subscribe(subscriber);
        }

        const State * getState() const override {
          return publisher.getState();
        }

        size_t getSubscriberCount() const override {
          return publisher.getSubscriberCount();
        }
        
        size_t getSubscriberMax() const override {
          return publisher.getSubscriberMax();
        }
        
        bool isOverSubscribed() const override {
          return publisher.isOverSubscribed();
        }
        
      private:

        Publisher::Instance<State, size> publisher;
        Reducer<State, Action> & reducer;
        bool reducing;
        bool notifying;
        const State * nextState;

        void publish(const State * state) override {
          nextState = nullptr;
          notifying = true;
          publisher.publish(state);
          notifying = false;
        }

    };

  }
}
