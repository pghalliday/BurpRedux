#pragma once

#include <functional>
#include "../Interface.hpp"

namespace BurpRedux {
  namespace State {
    namespace Factory {

      template <class State>
      class Pool {

        protected:

          using Uid = Interface::Uid;
          using f_construct = std::function<State * (const Uid uid, void * address)>;

          const State * _create(f_construct construct) {
            State * state = construct(_uid++, &(_memory[_current++]));
            _current %= _size;
            _setPrevious(state);
            return state;
          };

          void _setPrevious(State * state) {
            if (_previous) {
              _previous->~State();
            }
            _previous = state;
          }

          static constexpr size_t _size = 2;
          unsigned char _current = 0;
          State * _previous = nullptr;
          Uid _uid;
          char _memory[_size][sizeof(State)];

      };

    }
  }
}
