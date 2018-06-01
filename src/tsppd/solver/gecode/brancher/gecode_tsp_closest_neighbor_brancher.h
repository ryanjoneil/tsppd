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

#ifndef TSPPD_SOLVER_GECODE_TSP_CLOSEST_NEIGHBOR_BRANCHER_H
#define TSPPD_SOLVER_GECODE_TSP_CLOSEST_NEIGHBOR_BRANCHER_H

#include <tsppd/solver/gecode/brancher/gecode_tsp_brancher.h>

namespace TSPPD {
    namespace Solver {
        class GecodeTSPClosestNeighborBrancher : public GecodeTSPBrancher {
        public:
            GecodeTSPClosestNeighborBrancher(
                Gecode::Home home,
                Gecode::ViewArray<Gecode::Int::IntView>& next,
                const TSPPD::Data::TSPPDProblem& problem
            );

            GecodeTSPClosestNeighborBrancher(Gecode::Space& home, GecodeTSPClosestNeighborBrancher& b);
            virtual Gecode::Actor* copy(Gecode::Space& home);
            virtual size_t dispose(Gecode::Space& home);

            virtual Gecode::Choice* choice(Gecode::Space& home);

            static void post(
                Gecode::Home home,
                Gecode::ViewArray<Gecode::Int::IntView>& next,
                const TSPPD::Data::TSPPDProblem& problem
            );
        };
    }
}

#endif
