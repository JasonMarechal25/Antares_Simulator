/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once
#include <filesystem>

#include <antares/study-loader/IStudyLoader.h>
#include "antares/api/SimulationResults.h"

namespace Antares::Data
{
class Study;
}

namespace Antares::API
{

/**
 * @class APIInternal
 * @brief The APIInternal class is used to run simulations.
 */
class APIInternal
{
public:
    /**
     * @brief The run method is used to run the simulation.
     * @param study_loader A pointer to an IStudyLoader object. The IStudyLoader object is used to
     * load the study that will be simulated.
     * @return SimulationResults object which contains the results of the simulation.
     */
    SimulationResults run(const IStudyLoader& study_loader);

private:
    std::shared_ptr<Antares::Data::Study> study_;
    SimulationResults execute() const;
};

} // namespace Antares::API