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

#include <tsppd/solver/focacci/brancher/focacci_tsp_sequential_closest_neighbor_brancher.h>

using namespace Gecode;
using namespace TSPPD::Data;
using namespace TSPPD::Solver;
using namespace std;

FocacciTSPSequentialClosestNeighborBrancher::FocacciTSPSequentialClosestNeighborBrancher(
    Home home,
    ViewArray<Int::IntView>& next,
    const TSPPDProblem& problem) :
    FocacciTSPBrancher(home, next, problem), current(0) {

    home.notice(*this, AP_DISPOSE);
}

FocacciTSPSequentialClosestNeighborBrancher::FocacciTSPSequentialClosestNeighborBrancher(
    Space& home,
    FocacciTSPSequentialClosestNeighborBrancher& b) :
    FocacciTSPBrancher(home, b), current(b.current) { }

Actor* FocacciTSPSequentialClosestNeighborBrancher::copy(Space& home) {
    return new (home) FocacciTSPSequentialClosestNeighborBrancher(home, *this);
}

size_t FocacciTSPSequentialClosestNeighborBrancher::dispose(Gecode::Space& home) {
    home.ignore(*this, AP_DISPOSE);
    (void) FocacciTSPBrancher::dispose(home);
    return sizeof(*this);
}

Choice* FocacciTSPSequentialClosestNeighborBrancher::choice(Space& home) {
    // Find first unassigned start location along the path.
    while (next[current].assigned())
        current = next[current].val();

    // Branch on the closest arc to it.
    auto arc_index = closest_feasible_arc_index(current, indexes[current]);
    auto arc = problem.arc(current, arc_index);

    indexes[current] = arc_index;

    return new FocacciTSPBranchChoice(*this, arc.from_index, arc.to_index);
}

void FocacciTSPSequentialClosestNeighborBrancher::post(
    Home home,
    ViewArray<Int::IntView>& next,
    const TSPPDProblem& problem) {

    (void) new (home) FocacciTSPSequentialClosestNeighborBrancher(home, next, problem);
}
