/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_ECOINPUT_H__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_ECOINPUT_H__

#include <memory>

#include <antares/array/matrix.h>
#include <antares/series/series.h>

#include "../../fwd.h"
#include "defines.h"

namespace Antares::Data
{
/*!
** \brief Thermal
*/
class EconomicInputData
{
public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit EconomicInputData();
    //@}

    bool forceReload(bool reload) const;

    void markAsModified() const;

    /*!
    ** \brief Reset all values to their default ones
    */
    void reset();

    //! Copy data from another struct
    void copyFrom(const EconomicInputData& rhs);

    /*!
    ** \brief Load settings for the thermal prepro from a folder
    **
    ** \param folder The source folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool loadFromFolder(Study& study, const std::string& folder);

    /*!
    ** \brief Save settings used by the thermal prepro to a folder
    **
    ** \param folder The targer folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool saveToFolder(const AnyString& folder) const;

    /*!
    ** \brief Get the amount of memory used by the class
    */
    uint64_t memoryUsage() const;

    //! All {FO,PO}{Duration,Rate} annual values
    // max x DAYS_PER_YEAR
    TimeSeries::TS fuelcost;
    TimeSeries::TS co2cost;

}; // class EconomicInputData

} // namespace Antares::Data
#endif // __ANTARES_LIBS_STUDY_PARTS_THERMAL_ECOINPUT_H__
