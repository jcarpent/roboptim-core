// Copyright (C) 2014 by Benjamin Chrétien, CNRS.
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

#include "shared-tests/fixture.hh"

#include <roboptim/core/io.hh>
#include <roboptim/core/problem.hh>
#include <roboptim/core/function/constant.hh>

using namespace roboptim;

typedef boost::mpl::list< ::roboptim::EigenMatrixDense,
			  ::roboptim::EigenMatrixSparse> functionTypes_t;

BOOST_FIXTURE_TEST_SUITE (core, TestSuiteConfiguration)

BOOST_AUTO_TEST_CASE_TEMPLATE (problem, T, functionTypes_t)
{
  typedef Problem<GenericDifferentiableFunction<T>,
		  boost::mpl::vector<GenericDifferentiableFunction<T> > >
    problem_t;

  typedef typename problem_t::function_t      function_t;
  typedef typename function_t::argument_t     argument_t;
  typedef typename problem_t::startingPoint_t startingPoint_t;
  typedef typename problem_t::intervals_t     intervals_t;
  typedef typename problem_t::scales_t        scales_t;

  typedef GenericConstantFunction<T>          constantFunction_t;

  typename constantFunction_t::vector_t v (2);
  v.setZero ();

  constantFunction_t f (v);

  problem_t pb_fail (f);

  argument_t bad_x (3);
  pb_fail.startingPoint () = bad_x;

  {
    BOOST_CHECK_THROW (startingPoint_t test_sp = pb_fail.startingPoint ();
                       std::cout << (*test_sp) << std::endl,
                       std::runtime_error);
  }

  {
    BOOST_CHECK_THROW (const startingPoint_t& test_sp = pb_fail.startingPoint ();
                       std::cout << (*test_sp) << std::endl,
                       std::runtime_error);
  }

  problem_t pb (f);
  argument_t x (2);
  x.setZero ();
  pb.startingPoint () = x;

  typename constantFunction_t::names_t names (2);
  names[0] = "x₀";
  names[1] = "x₁";
  pb.argumentNames () = names;

  boost::shared_ptr<constantFunction_t>
    cstr = boost::make_shared<constantFunction_t>  (v);
  intervals_t intervals (2);
  scales_t scales (2, 1);
  for (size_t i = 0; i < intervals.size (); ++i)
    intervals[i] = Function::makeInfiniteInterval ();
  pb.addConstraint (cstr, intervals, scales);

  // Check null ptr
  BOOST_CHECK_THROW (boost::shared_ptr<constantFunction_t> null_ptr;
                     pb.addConstraint (null_ptr, intervals, scales),
                     std::runtime_error);

  // Check invalid input size
  BOOST_CHECK_THROW (typename constantFunction_t::vector_t v_size (4);
                     boost::shared_ptr<constantFunction_t>
                     cstr_size = boost::make_shared<constantFunction_t> (v_size);
                     pb.addConstraint (cstr_size, intervals, scales),
                     std::runtime_error);

  // Check invalid interval size
  BOOST_CHECK_THROW (intervals_t intervals_size (6);
                     pb.addConstraint (cstr, intervals_size, scales),
                     std::runtime_error);

  // Check invalid scale vector size
  BOOST_CHECK_THROW (scales_t scales_size (6);
                     pb.addConstraint (cstr, intervals, scales_size),
                     std::runtime_error);

  std::cout << pb << std::endl;

  // Test a problem with multiple types of constraints.
  typedef Problem<GenericDifferentiableFunction<T>,
		  boost::mpl::vector<GenericLinearFunction<T>,
				     GenericDifferentiableFunction<T> > > mixedProblem_t;
  mixedProblem_t mixedPb (f);
  mixedPb.startingPoint () = x;
  mixedPb.argumentNames () = names;

  // First constraint: ConstantFunction automatically converted to LinearFunction
  mixedPb.addConstraint (cstr, intervals, scales);
  // Second constraint: ConstantFunction converted to DifferentiableFunction
  mixedPb.addConstraint (boost::static_pointer_cast<GenericDifferentiableFunction<T> > (cstr),
                         intervals, scales);

  // First constraint: LinearFunction
  BOOST_CHECK (mixedPb.constraints() [0].which () == 0);
  // Second constraint: DifferentiableFunction
  BOOST_CHECK (mixedPb.constraints() [1].which () == 1);

  std::cout << mixedPb << std::endl;
}

BOOST_AUTO_TEST_SUITE_END ()
