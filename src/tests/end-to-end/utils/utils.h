#pragma once
#define WIN32_LEAN_AND_MEAN
#include "antares/study/study.h"
#include "simulation/economy.h"
#include "antares/study/scenario-builder/rules.h"
#include "antares/study/scenario-builder/sets.h"

void prepareStudy(Antares::Data::Study::Ptr pStudy, int nbYears);

Antares::Data::Area* addArea(Antares::Data::Study::Ptr pStudy, const std::string& areaName, int nbTS);

std::shared_ptr<Antares::Data::ThermalCluster> addCluster(Antares::Data::Area* pArea, const std::string& clusterName, double maximumPower, double cost, int nbTS, int unitCount = 1);

std::shared_ptr<Antares::Data::BindingConstraint> addBindingConstraints(Antares::Data::Study::Ptr study, std::string name, std::string group);

void cleanSimulation(Antares::Data::Study::Ptr pStudy, Antares::Solver::Simulation::ISimulation< Antares::Solver::Simulation::Economy >* simulation);

Solver::Simulation::ISimulation< Solver::Simulation::Economy >* runSimulation(Study::Ptr pStudy);

float defineYearsWeight(Study::Ptr pStudy, const std::vector<float>& yearsWeight);

ScenarioBuilder::Rules::Ptr createScenarioRules(Study::Ptr pStudy);


class NoOPResultWriter: public Solver::IResultWriter {
    void addEntryFromBuffer(const std::string &, Clob &) override;

    void addEntryFromBuffer(const std::string &, std::string &) override;

    void addEntryFromFile(const std::string &, const std::string &) override;

    bool needsTheJobQueue() const override;

    void finalize(bool ) override;
};