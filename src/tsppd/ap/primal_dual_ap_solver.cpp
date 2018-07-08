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

#include <iomanip>
#include <limits>
#include <numeric>

#include <tsppd/ap/primal_dual_ap_solver.h>

using namespace TSPPD::AP;
using namespace std;

const int M = 10 * 1000 * 1000;

PrimalDualAPSolver::PrimalDualAPSolver(const unsigned int size) :
    APSolver(size),
    a(vector<vector<int>>(size, vector<int>(size, 0))),
    lb(vector<vector<bool>>(size, vector<bool>(size, false))),
    ub(vector<vector<bool>>(size, vector<bool>(size, true))),
    u(vector<int>(size, 0)),
    v(vector<int>(size, 0)),
    f(vector<int>(size, -1)),
    f_bar(vector<int>(size, -1)),
    p(vector<int>(size, -1)),
    LR(),
    UC(),
    c(vector<int>(size, -1)),
    pi(vector<int>(size, -1)),
    initialized(false) { }

bool PrimalDualAPSolver::solve() {
    if (!initialized)
        initialize();

    for (int i = 0; i < (int) size; ++i) {
        if (f[i] < 0) {
            auto j = path(i);
            if (j >= 0)
                increase(i, j);
        }
    }

    z = accumulate(u.begin(), u.end(), 0) + accumulate(v.begin(), v.end(), 0);
    return z >= 0 && z < M;
}

void PrimalDualAPSolver::set_obj(APIndex index, int _obj) {
    a[index.first][index.second] = _obj;
}

void PrimalDualAPSolver::set_bounds(APIndex index, bool _lb, bool _ub) {
    auto row = index.first;
    auto col = index.second;

    lb[row][col] = _lb;
    ub[row][col] = _ub;

    if (!_ub) {
        if (f[row] == (int) col) {
            f[row] = -1;
            f_bar[col] = -1;
        }

        a[row][col] = M;
    }
}

int PrimalDualAPSolver::get_z() const {
    return z;
}

bool PrimalDualAPSolver::get_x(APIndex index) {
    return f[index.first] == (int) index.second;
}

int PrimalDualAPSolver::get_rc(APIndex index) {
    auto row = index.first;
    auto col = index.second;
    return a[row][col] - u[row] - v[col];
}

bool PrimalDualAPSolver::get_lb(APIndex index) {
    return lb[index.first][index.second];
}

bool PrimalDualAPSolver::get_ub(APIndex index) {
    return ub[index.first][index.second];
}

int PrimalDualAPSolver::get_u(unsigned int row) {
    return u[row];
}

int PrimalDualAPSolver::get_v(unsigned int col) {
    return v[col];
}

void PrimalDualAPSolver::initialize() {
    initialize_phase_1();
    initialize_phase_2();

    initialized = true;
}

int PrimalDualAPSolver::path(int i) {
    LR = {i};

    for (int j = 0; j < (int) size; ++j)
        UC.insert(j);

    pi.assign(size, numeric_limits<int>::max());

    while (true) {
        auto r = LR[LR.size() - 1];

        if (r >= (int) size)
            break;

        for (auto j : UC) {
            auto val = a[r][j] - u[r] - v[j];
            if (val < pi[j]) {
                pi[j] = val;
                c[j] = r;
            }
        }

        bool found = false;
        for (auto j : UC) {
            if (pi[j] == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            // d = min { pi[j] | j in UC }
            bool first = true;
            int d = 0;
            for (auto j : UC) {
                if (first || pi[j] < d) {
                    first = false;
                    d = pi[j];
                }
            }

            for (auto h : LR)
                u[h] += d;

            for (int j = 0; j < (int) size; ++j)
                if (pi[j] == 0)
                    v[j] -= d;
                else
                    pi[j] -= d;
        }

        // j = first column in { k in UC | pi[k] = 0 }
        int j = -1;
        for (auto k : UC) {
            if (pi[k] == 0) {
                j = k;
                break;
            }
        }

        if (j >= 0 && f_bar[j] >= 0) {
            LR.push_back(f_bar[j]);
            UC.erase(j);
        }

        if (j >= 0 && f_bar[j] < 0)
            return j;
    }

    return -1;
}

void PrimalDualAPSolver::increase(int i, int j) {
    while (true) {
        auto l = c[j];
        f_bar[j] = l;
        auto k = f[l];
        f[l] = j;
        j = k;

        if (l == i)
            break;
    }
}

void PrimalDualAPSolver::initialize_phase_1() {
    for (int j = 0; j < (int) size; ++j) {
        auto r = min_row(j);
        v[j] = a[r][j];
        if (f[r] < 0) {
            f_bar[j] = r;
            f[r] = j;
            u[r] = 0;
            p[r] = j + 1;
        }
    }
}

void PrimalDualAPSolver::initialize_phase_2() {
    for (int i = 0; i < (int) size; ++i) {
        if (f[i] >= 0)
            continue;

        // Determine value of u[i].
        auto j = min_col(i);
        u[i] = a[i][j] - v[j];

        bool assign = (f_bar[j] < 0);
        while (!assign && j < (int) size) {
            if (a[i][j] - u[i] - v[j] == 0) {
                auto r = f_bar[j];
                if (r < 0)
					break;

                auto k = p[r];

                while (!assign && k < (int) size) {
                    if (r >= 0 && k >= 0 && f_bar[k] < 0 && a[r][k] - u[r] - v[k] == 0) {
                        assign = true;
                        f[r] = k;
                        f_bar[k] = r;
                    } else {
                        ++k;
                    }
                }

                // Note: The paper sets p[r] = k +1, but the FORTRAN code
                //       sets p[r] = n. We use the latter as our standard.
                p[r] = size;
            }

            if (!assign)
                ++j;
        }

        if (assign) {
            f[i] = j;
            f_bar[j] = i;
            p[i] = j + 1;
        }
    }
}

int PrimalDualAPSolver::min_row(int j) {
    bool first = true;
    int min_val = 0;
    int min_row = 0;

    for (int i = 0; i < (int) size; ++i) {
        if (lb[i][j]) {
            return i;
        } else if (ub[i][j]) {
            if (first || a[i][j] < min_val || (a[i][j] == min_val && f[i] < 0)) {
                first = false;
                min_val = a[i][j];
                min_row = i;
            }
        }
    }

    return min_row;
}

int PrimalDualAPSolver::min_col(int i) {
    bool first = true;
    int min_val = 0;
    int min_col = 0;

    for (int j = 0; j < (int) size; ++j) {
        if (lb[i][j]) {
            return j;
        } else if (ub[i][j]) {
            if (first || a[i][j] - v[j] < min_val || (a[i][j] == min_val && f_bar[j] < 0)) {
                first = false;
                min_val = a[i][j] - v[j];
                min_col = j;
            }
        }
    }

    return min_col;
}
