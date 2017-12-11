/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*  This file is part of the tsppd program and library for solving           */
/*  Traveling Salesman Problems with Pickup and Delivery. tsppd requires     */
/*  other commercial and open source software to build. tsppd is decribed    */
/*  in the paper "Exact Methods for Solving Traveling Salesman Problems      */
/*  with Pickup and Delivery in Real Time".                                  */
/*                                                                           */
/*  Copyright (C) 2017 Ryan J. O'Neil <roneil1@gmu.edu>                      */
/*                                                                           */
/*  tsppd is distributed under the terms of the ZIB Academic License.        */
/*  You should have received a copy of the ZIB Academic License along with   */
/*  tsppd. See the file LICENSE. If not, email roneil1@gmu.edu.              */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef TSPPD_SOLVER_GECODE_TSP_SPACE_H
#define TSPPD_SOLVER_GECODE_TSP_SPACE_H

#include <ostream>
#include <vector>

#include <gecode/int.hh>
#include <gecode/minimodel.hh>

#include <tsppd/data/tsppd_problem.h>
#include <tsppd/solver/gecode/brancher/gecode_tsp_brancher.h>
#include <tsppd/solver/gecode/dual/gecode_tsp_dual.h>

namespace TSPPD {
    namespace Solver {
        class GecodeTSPSpace : public Gecode::IntMinimizeSpace {
        public:
            GecodeTSPSpace(const TSPPD::Data::TSPPDProblem& problem);

            // Search & optimization support
            GecodeTSPSpace(bool share, GecodeTSPSpace& rs);
            virtual Gecode::Space* copy(bool share);
            virtual Gecode::IntVar cost() const;
            virtual Gecode::IntVar dual() const;
            virtual void constrain(const Gecode::Space& _best);

            virtual void print(std::ostream& out = std::cout) const;

            virtual void initialize_constraints();
            virtual void initialize_brancher(const GecodeTSPBrancherType brancher_type);
            virtual void initialize_dual(const GecodeTSPDualType dual_type);

            virtual std::vector<std::string> solution() const;

        protected:
            Gecode::IntArgs build_arc_costs() const;
            void print_int_array(std::ostream& out, Gecode::IntVarArray array, std::string label) const;

            const TSPPD::Data::TSPPDProblem& problem;

            // Decision variables
            Gecode::IntVarArray next;
            Gecode::IntVar length;
            Gecode::IntVar dual_bound;
        };
    }
}

#endif
