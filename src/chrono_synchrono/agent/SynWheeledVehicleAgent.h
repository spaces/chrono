#ifndef SYN_WHEELED_VEHICLE_AGENT_H
#define SYN_WHEELED_VEHICLE_AGENT_H

#include "chrono_synchrono/SynApi.h"
#include "chrono_synchrono/agent/SynVehicleAgent.h"
#include "chrono_synchrono/vehicle/SynWheeledVehicle.h"

using namespace chrono;
using namespace chrono::vehicle;

namespace chrono {
namespace synchrono {

class SYN_API SynWheeledVehicleAgent : public SynVehicleAgent {
  public:
    ///@brief Construct a vehicle agent with the specified rank and system
    /// Underlying agent is set to a vehicle type automatically
    ///
    ///@param rank the rank of this agent
    ///@param system an optional argument of the ChSystem to build the vehicle with
    SynWheeledVehicleAgent(unsigned int rank, ChSystem* system = 0);

    ///@brief Construct a wheeled vehicle agent with a json specification file
    ///
    ///@param rank the rank of this agent
    ///@param coord_sys the initial position and orientation of the vehicle
    ///@param filename the json specification file
    ///@param system the system to build the vehicle with
    SynWheeledVehicleAgent(unsigned int rank, ChCoordsys<> coord_sys, const std::string& filename, ChSystem* system);

    ///@brief Construct a wheeled vehicle agent with a json specification file
    ///
    ///@param rank the rank of this agent
    ///@param coord_sys the initial position and orientation of the vehicle
    ///@param filename the json specification file
    ///@param system the contact method used to build the vehicle
    SynWheeledVehicleAgent(unsigned int rank,
                           ChCoordsys<> coord_sys,
                           const std::string& filename,
                           ChContactMethod contact_method);

    ///@brief Construct a zombie vehicle agent from the specified json configuration and the ChSystem
    ///
    ///@param rank the rank of the zombie agent
    ///@param filename the json specification file
    SynWheeledVehicleAgent(unsigned int rank, const std::string& filename);

    ///@brief Destroy the SynWheeledVehicleAgent
    virtual ~SynWheeledVehicleAgent() {}

    ///@brief Synchronize the underlying vehicle
    ///
    ///@param time the time to synchronize to
    ///@param driver_inputs the driver inputs (i.e. throttle, braking, steering)
    virtual void Synchronize(double step, ChDriver::Inputs driver_inputs) override;

    ///@brief Get the state of this agent
    ///
    ///@return std::shared_ptr<SynMessageState>
    virtual std::shared_ptr<SynMessageState> GetState() override;

    ///@brief Get the this agent's message to be pass to other ranks
    ///
    ///@return std::shared_ptr<SynMessageState>
    virtual std::shared_ptr<SynAgentMessage> GetMessage() override { return m_msg; };

    ///@brief Generates messages to be sent to other ranks
    /// Will create or get messages and pass them into the referenced message vector
    ///
    ///@param messages a referenced vector containing messages to be distributed from this rank
    virtual void GenerateMessagesToSend(std::vector<SynMessage*>& messages) override;

    /// Get/Set this agent's vehicle
    virtual std::shared_ptr<SynVehicle> GetVehicle() override { return m_wheeled_vehicle; }
    std::shared_ptr<SynWheeledVehicle> GetWheeledVehicle() { return m_wheeled_vehicle; }
    void SetVehicle(std::shared_ptr<SynWheeledVehicle> wheeled_vehicle);

  private:
    ///@brief Construct a zombie WheeledVehicleAgent from a json speficiation file
    ///
    ///@param filename the json specification file
    void VehicleAgentFromJSON(const std::string& filename);

    ///@brief Construct a VehicleAgent with a json specification file and a ChSystem
    ///
    ///@param coord_sys the initial position and orientation of the vehicle
    ///@param filename the json specification file
    ///@param system the ChSystem to build the vehicle and the agent with
    void VehicleAgentFromJSON(ChCoordsys<> coord_sys, const std::string& filename, ChSystem* system);

    ///@brief Construct a VehicleAgent with a json specification file and a ChSystem
    ///
    ///@param coord_sys the initial position and orientation of the vehicle
    ///@param filename the json specification file
    ///@param contact_method the contact method that should be used in this simulation
    void VehicleAgentFromJSON(ChCoordsys<> coord_sys, const std::string& filename, ChContactMethod contact_method);

  private:
    std::shared_ptr<SynWheeledVehicleMessage> m_msg;       ///< handle to the message that will be passed between ranks
    std::shared_ptr<SynWheeledVehicle> m_wheeled_vehicle;  ///< handle to this agent's vehicle
};

}  // namespace synchrono
}  // namespace chrono

#endif
