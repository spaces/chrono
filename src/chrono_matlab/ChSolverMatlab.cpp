// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Alessandro Tasora, Radu Serban
// =============================================================================

#include "chrono_matlab/ChSolverMatlab.h"

namespace chrono {

// Register into the object factory, to enable run-time dynamic creation and persistence
CH_FACTORY_REGISTER(ChSolverMatlab)

ChSolverMatlab::ChSolverMatlab(ChMatlabEngine& me) {
    mengine = &me;
}

ChSolverMatlab::ChSolverMatlab() {
    mengine = 0;
}

// Solve using the Matlab default direct solver (as in x=A\b)
double ChSolverMatlab::Solve(ChSystemDescriptor& sysd) {
    ChSparseMatrix mdM;
    ChSparseMatrix mdCq;
    ChSparseMatrix mdE;
    ChVectorDynamic<double> mdf;
    ChVectorDynamic<double> mdb;
    ChVectorDynamic<double> mdfric;
    sysd.ConvertToMatrixForm(&mdCq, &mdM, &mdE, &mdf, &mdb, &mdfric);

    mengine->PutSparseMatrix(mdM, "mdM");
    mengine->PutSparseMatrix(mdCq, "mdCq");
    mengine->PutSparseMatrix(mdE, "mdE");
    mengine->PutVariable(mdf, "mdf");
    mengine->PutVariable(mdb, "mdb");
    mengine->PutVariable(mdfric, "mdfric");

    mengine->Eval("mdZ = [mdM, mdCq'; mdCq, -mdE]; mdd=[mdf;-mdb];");

    mengine->Eval("mdx = mldivide(mdZ , mdd);");

    ChMatrixDynamic<> mx;
    if (!mengine->GetVariable(mx, "mdx"))
        std::cerr << "ERROR!! cannot fetch mdx" << std::endl;

    sysd.FromVectorToUnknowns(mx);

    mengine->Eval("resid = norm(mdZ*mdx - mdd);");
    ChMatrixDynamic<> mres;
    mengine->GetVariable(mres, "resid");
    std::cout << " Matlab computed residual:" << mres(0, 0) << std::endl;

    return 0;
}

void ChSolverMatlab::ArchiveOut(ChArchiveOut& archive_out) {
    // version number
    archive_out.VersionWrite<ChSolverMatlab>();
    // serialize parent class
    ChSolver::ArchiveOut(archive_out);
    // serialize all member data:
    archive_out << CHNVP(mengine);
}

void ChSolverMatlab::ArchiveIn(ChArchiveIn& archive_in) {
    // version number
    /*int version =*/archive_in.VersionRead<ChSolverMatlab>();
    // deserialize parent class
    ChSolver::ArchiveIn(archive_in);
    // stream in all member data:
    archive_in >> CHNVP(mengine);
}

}  // end namespace chrono
