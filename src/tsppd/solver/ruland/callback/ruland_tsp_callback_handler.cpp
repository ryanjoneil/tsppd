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

#include <cmath>
#include <iostream>

#include <tsppd/data/tsppd_search_statistics.h>
#include <tsppd/data/tsppd_solution.h>
#include <tsppd/solver/ruland/callback/ruland_tsp_callback_handler.h>

using namespace TSPPD::Data;
using namespace TSPPD::IO;
using namespace TSPPD::Solver;
using namespace std;

RulandTSPCallbackHandler::RulandTSPCallbackHandler(
    const TSPSolver* solver,
    const TSPPDProblem& problem,
    std::map<std::pair<unsigned int, unsigned int>, GRBVar> arcs,
    vector<shared_ptr<RulandTSPCallback>> callbacks,
    TSPSolutionWriter& writer) :
    solver(solver),
    problem(problem),
    arcs(arcs),
    callbacks(callbacks),
    writer(writer),
    subtour_finder(problem),
    primal(-1) { }

double RulandTSPCallbackHandler::get_solution(GRBVar v) {
    return getSolution(v);
}

double* RulandTSPCallbackHandler::get_solution(const GRBVar* xvars, int len) {
    return getSolution(xvars, len);
}

void RulandTSPCallbackHandler::add_lazy(const GRBTempConstr& tc) {
    addLazy(tc);
}

void RulandTSPCallbackHandler::add_lazy(const GRBLinExpr& expr, char sense, double rhs) {
    addLazy(expr, sense, rhs);
}

void RulandTSPCallbackHandler::callback() {
    if (where == GRB_CB_MIP) {
        TSPPDSearchStatistics stats;

        // For some reason this moodel produces invalid primal bounds during search sometimes,
        // so we instead get primal bounds directly off of MIPSOL callbacks.
        if (primal >= 0)
            stats.primal = primal;
        else
            stats.primal = getDoubleInfo(GRB_CB_MIP_OBJBST);
        stats.dual = max(0.0, getDoubleInfo(GRB_CB_MIP_OBJBND));


        writer.write(stats);

    } else if (where == GRB_CB_MIPSOL) {
        // Convert arc variables into boolean values for subtour finder.
        map<pair<unsigned int, unsigned int>, bool> arc_values{};
        for (auto pair : arcs)
            arc_values[pair.first] = *(get_solution(&(pair.second), 1)) > 0.5;

        auto subtours = subtour_finder.subtours(arc_values);

        // Write out current solution.
        if (subtours.size() == 1) {
            TSPPDSolution solution(problem, subtours[0]);
            if (solution.feasible()) {
                TSPPDSearchStatistics stats(solution);
                primal = solution.cost;
                stats.dual = max(0.0, ceil(getDoubleInfo(GRB_CB_MIPSOL_OBJBND)));
                writer.write(stats);
            }
        }

        auto grb_callback = static_cast<GRBCallback*>(this);
        for (auto callback : callbacks)
            callback->callback(grb_callback, subtours);
    }
}
