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

#include <tsppd/solver/gecode/gecode_tsppd_solver.h>
#include <tsppd/solver/gecode/gecode_tsppd_space.h>
#include <tsppd/util/exception.h>

using namespace TSPPD::Data;
using namespace TSPPD::IO;
using namespace TSPPD::Solver;
using namespace TSPPD::Util;
using namespace std;

GecodeTSPPDSolver::GecodeTSPPDSolver(
    const TSPPDProblem& problem,
    const map<string, string> options,
    TSPSolutionWriter& writer) :
    GecodeTSPSolver(problem, options, writer) {

    initialize_tsppd_options();
}

void GecodeTSPPDSolver::initialize_tsppd_options() {
    // Precedence propagation
    if (options["precede"] == "" || options["precede"] == "set")
        precede_type = PRECEDE_SET;
    else if (options["precede"] == "cost")
        precede_type = PRECEDE_COST;
    else if (options["precede"] == "all")
        precede_type = PRECEDE_ALL;
    else
        throw TSPPDException("precede can be either set, cost, or all");

    // Assignment Problem reduced cost fixing
    if (options["ap"] == "" || options["ap"] == "off")
        ap = false;
    else if (options["ap"] == "on")
        ap = true;
    else
        throw TSPPDException("ap can be either on or off");

    // Order Matching Constraint propagation
    if (options["omc"] == "" || options["omc"] == "off")
        omc = false;
    else if (options["omc"] == "on")
        omc = true;
    else
        throw TSPPDException("omc can be either on or off");
}

shared_ptr<GecodeTSPSpace> GecodeTSPPDSolver::build_space() {
    auto space = make_shared<GecodeTSPPDSpace>(problem);
    space->initialize_precedence_propagators(precede_type);
    if (ap)
        space->initialize_assignment_propagator();
    if (omc)
        space->initialize_omc_constraints();
    return space;
}