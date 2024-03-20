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
// Authors: Alessandro Tasora
// =============================================================================

#include <vector>

#include "chrono_cosimulation/ChCosimulation.h"

using namespace chrono::utils;

namespace chrono {
namespace cosimul {

ChCosimulation::ChCosimulation(ChSocketFramework& mframework,  // socket framework
                               int n_in_values,                // number of scalar variables to receive each timestep
                               int n_out_values                // number of scalar variables to send each timestep
) {
    this->myServer = 0;
    this->myClient = 0;
    this->in_n = n_in_values;
    this->out_n = n_out_values;
    this->nport = 0;
}

ChCosimulation::~ChCosimulation() {
    if (this->myServer)
        delete this->myServer;
    this->myServer = 0;
    if (this->myClient)
        delete this->myClient;
    this->myClient = 0;
}

bool ChCosimulation::WaitConnection(int aport) {
    this->nport = aport;

    // a server is created, that could listen at a given port
    this->myServer = new ChSocketTCP(aport);

    // bind socket to server
    this->myServer->bindSocket();

    // wait for a client to connect (this might put the program in
    // a long waiting state... a timeout can be useful then)
    this->myServer->listenToClient(1);

    std::string clientHostName;
    this->myClient = this->myServer->acceptClient(clientHostName);  // pick up the call!

    if (!this->myClient)
        throw std::runtime_error("Server failed in getting the client socket");

    return true;
}

bool ChCosimulation::SendData(double mtime, ChVectorConstRef out_data) {
    if (out_data.size() != this->out_n)
        throw std::runtime_error("ERROR: Sent data must be a vector of size N.");
    if (!myClient)
        throw std::runtime_error("ERROR: Attempted 'SendData' with no connected client.");

    std::vector<char> mbuffer;
    mbuffer.resize((out_data.size() + 1) * sizeof(double));
    // Serialize data (little endian)...
    // time:
    for (size_t ds = 0; ds < sizeof(double); ++ds) {
        mbuffer[ds] = (reinterpret_cast<char*>(&mtime))[ds];
    }

    // variables:
    for (int i = 0; i < out_data.size(); i++) {
        for (size_t ds = 0; ds < sizeof(double); ++ds) {
            mbuffer[(i + 1) * sizeof(double) + ds] =
                reinterpret_cast<char*>(const_cast<double*>(&out_data.data()[i]))[ds];
        }
    }

    // -----> SEND!!!
    this->myClient->SendBuffer(mbuffer);

    return true;
}

bool ChCosimulation::ReceiveData(double& mtime, ChVectorRef in_data) {
    if (in_data.size() != this->in_n)
        throw std::runtime_error("ERROR: Received data must be a vector of size N.");
    if (!myClient)
        throw std::runtime_error("ERROR: Attempted 'ReceiveData' with no connected client.");

    // Receive from the client
    int nbytes = sizeof(double) * (this->in_n + 1);
    std::vector<char> rbuffer;
    rbuffer.resize(nbytes);

    // -----> RECEIVE!!!
    /*int numBytes =*/this->myClient->ReceiveBuffer(rbuffer, nbytes);

    // Deserialize data (little endian)...
    // retrieve time
    for (size_t ds = 0; ds < sizeof(double); ++ds) {
        reinterpret_cast<char*>(&mtime)[ds] = rbuffer[ds];
    }

    // retrieve variables:
    for (size_t i = 0; i < in_data.size(); ++i) {
        for (size_t ds = 0; ds < sizeof(double); ++ds) {
            reinterpret_cast<char*>(&(in_data.data()[i]))[ds] = rbuffer[(i + 1) * sizeof(double) + ds];
        }
    }

    return true;
}

}  // end namespace cosimul
}  // end namespace chrono
