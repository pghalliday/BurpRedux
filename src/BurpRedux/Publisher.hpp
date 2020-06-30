#pragma once

#include <array>
#include "State/Interface.hpp"
#include "Subscriber.hpp"

namespace BurpRedux {

  template <size_t subscriberCount>
  class Publisher {

    public:

      using State = BurpRedux::State::Interface;
      using Subscribers = std::array<Subscriber *, subscriberCount>;

      Publisher(const Subscribers subscribers) :
          _subscribers(subscribers),
          _state(nullptr),
          _uid(0)
      {}

      void setup(const State * state) {
        _setState(state, true);
        for (auto subscriber : _subscribers) {
          if (subscriber) {
            subscriber->setup(_state);
          }
        }
      }

      const State * getState() const {
        return _state;
      }

      void publish(const State * state) {
        if (_setState(state)) {
          for (auto subscriber : _subscribers) {
            if (subscriber) {
              subscriber->onPublish(_state);
            }
          }
        }
      }

    private:

      const Subscribers _subscribers;
      const State * _state;
      unsigned long _uid;

      bool _setState(const State * state, bool force = false) {
        // All states must implement the State interface
        // to expose a unique id. We will use this to
        // check if a state has changed. Simply checking pointers
        // is not good for this as we don't know that pointers
        // won't be reused (eg in a memory pool). Batching
        // actions mean that the State can change many times
        // between publishing, so any solution that rotates/reuses
        // identifying features like pointers will be problematic.
        // We have to get and store the unique id alongside the current
        // state as the current state may well have been cleaned
        // up by the time we need to check it.
        unsigned long uid = state->getUid();
        if (force || uid != _uid) {
          _uid = uid;
          _state = state;
          return true;
        }
        return false;
      }

  };

}