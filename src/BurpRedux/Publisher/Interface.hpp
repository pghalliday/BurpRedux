#pragma once

#include <stddef.h>
#include "../Subscriber.hpp"

namespace BurpRedux {
  namespace Publisher {

    template <class State>
    class Interface {

      public:

        virtual bool subscribe(Subscriber<State> * subscriber) = 0;
        virtual const State * getState() const = 0;
        virtual void publish(const State * state) = 0;
        virtual size_t getSubscriberCount() const = 0;
        virtual size_t getSubscriberMax() const = 0;
        virtual bool isOverSubscribed() const = 0;

    };

  }
}
