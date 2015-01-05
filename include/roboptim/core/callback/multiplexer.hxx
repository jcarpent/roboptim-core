// Copyright (C) 2014 by Benjamin Chretien, CNRS-LIRMM.
//
// This file is part of the roboptim.
//
// roboptim is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// roboptim is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with roboptim.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ROBOPTIM_CORE_CALLBACK_MULTIPLEXER_HXX
# define ROBOPTIM_CORE_CALLBACK_MULTIPLEXER_HXX

# include <boost/bind.hpp>

namespace roboptim
{
  namespace callback
  {

    template <typename S>
    Multiplexer<S>::Multiplexer (solver_t& solver)
    : solver_ (solver),
      callbacks_ ()
    {
      attach ();
    }

    template <typename S>
    Multiplexer<S>::Multiplexer (solver_t& solver,
                                 const callbacks_t& callbacks)
    : solver_ (solver),
      callbacks_ (callbacks)
    {
      attach ();
    }

    template <typename S>
    Multiplexer<S>::~Multiplexer ()
    {
      unregister ();
    }

    template <typename S>
    typename Multiplexer<S>::callbacks_t& Multiplexer<S>::callbacks ()
    {
      return callbacks_;
    }

    template <typename S>
    const typename Multiplexer<S>::callbacks_t& Multiplexer<S>::callbacks ()
    const
    {
      return callbacks_;
    }

    template <typename S>
    void Multiplexer<S>::perIterationCallback
    (const problem_t& pb,
     solverState_t& state)
    {
      try
      {
        perIterationCallbackUnsafe (pb, state);
      }
      catch (std::exception& e)
      {
        std::cerr << e.what () << std::endl;
      }
      catch (...)
      {
        std::cerr << "unknown exception" << std::endl;
      }
    }

    template <typename S>
    void Multiplexer<S>::perIterationCallbackUnsafe
    (const problem_t& pb,
     solverState_t& state)
    {
      // For each callback function
      for (typename callbacks_t::iterator
           iter  = callbacks_.begin ();
           iter != callbacks_.end (); ++iter)
      {
        (*iter) (pb, state);
      }
    }

    template <typename S>
    void Multiplexer<S>::attach ()
    {
      // Set the callback.
      try
      {
        solver_.setIterationCallback
          (boost::bind (&Multiplexer<S>::perIterationCallback,
                        this, _1, _2));
      }
      catch (std::runtime_error& e)
      {
        std::cerr
          << "failed to set callback multiplexer:\n"
          << e.what () << std::endl;
      }
    }

    template <typename S>
    void Multiplexer<S>::unregister ()
    {
      // Unregister the callback, do not fail if this is impossible.
      try
      {
        solver_.setIterationCallback (callback_t ());
      }
      catch (std::exception& e)
      {}
    }
  } // end of namespace callback.
} // end of namespace roboptim.

#endif //! ROBOPTIM_CORE_CALLBACK_MULTIPLEXER_HXX
