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
#ifndef __SOLVER_VARIABLE_ECONOMY_ProductionByDispatchablePlant_H__
#define __SOLVER_VARIABLE_ECONOMY_ProductionByDispatchablePlant_H__

#include "antares/solver/variable/variable.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
struct VCardProductionByDispatchablePlant
{
    //! Caption
    static std::string Caption()
    {
        return "DTG by plant";
    }
    //! Unit
    static std::string Unit()
    {
        return "MWh";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Energy generated by all the clusters";
    }

    //! The expecte results
    typedef Results<R::AllYears::Average< // The average values throughout all years
      >>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardProductionByDispatchablePlant VCardForSpatialAggregate;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & (Category::de),
        //! Precision (views)
        precision = Category::all,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
        //! Decimal precision
        decimal = 0,
        //! Number of columns used by the variable
        columnCount = Category::dynamicColumns,
        //! The Spatial aggregation
        spatialAggregate = Category::spatialAggregateSum,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,
        //! Intermediate values
        hasIntermediateValues = 1,
        //! Can this variable be non applicable (0 : no, 1 : yes)
        isPossiblyNonApplicable = 0,
    };

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef IntermediateValues* IntermediateValuesBaseType;
    typedef IntermediateValuesBaseType* IntermediateValuesType;

}; // class VCard

/*!
** \brief Energy generated by all thermal dispatchable clusters
*/
template<class NextT = Container::EndOfList>
class ProductionByDispatchablePlant
 : public Variable::
     IVariable<ProductionByDispatchablePlant<NextT>, NextT, VCardProductionByDispatchablePlant>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardProductionByDispatchablePlant VCardType;
    //! Ancestor
    typedef Variable::IVariable<ProductionByDispatchablePlant<NextT>, NextT, VCardType>
      AncestorType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

    enum
    {
        //! How many items have we got
        count = 1 + NextT::count,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count
            = ((VCardType::categoryDataLevel & CDataLevel && VCardType::categoryFileLevel & CFile)
                 ? (NextType::template Statistics<CDataLevel, CFile>::count
                    + VCardType::columnCount * ResultsType::count)
                 : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    ProductionByDispatchablePlant() :
     pValuesForTheCurrentYear(nullptr), pminOfTheClusterForYear(nullptr), pSize(0)
    {
    }

    ~ProductionByDispatchablePlant()
    {
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            delete[] pValuesForTheCurrentYear[numSpace];
        delete[] pValuesForTheCurrentYear;

        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            delete[] pminOfTheClusterForYear[numSpace];
        delete[] pminOfTheClusterForYear;
    }

    void initializeFromStudy(Data::Study& study)
    {
        // Next
        NextType::initializeFromStudy(study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        // Get the number of years in parallel
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
        pminOfTheClusterForYear = new double*[pNbYearsParallel];

        // Get the area
        pSize = area->thermal.list.enabledCount();
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                pValuesForTheCurrentYear[numSpace]
                  = new VCardType::IntermediateValuesDeepType[pSize];

            // Minimum power values of the cluster for the whole year - from the solver in the
            // accurate mode not to be displayed in the output \todo think of a better place like
            // the DispatchableMarginForAllAreas done at the beginning of the year

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                pminOfTheClusterForYear[numSpace] = new double[pSize * maxHoursInAYear];

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                for (unsigned int i = 0; i != pSize; ++i)
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);

            for (unsigned int i = 0; i != pSize; ++i)
            {
                AncestorType::pResults[i].initializeFromStudy(*study);
                AncestorType::pResults[i].reset();
            }
        }
        else
        {
            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace] = nullptr;
                pminOfTheClusterForYear[numSpace] = nullptr;
            }

            AncestorType::pResults.clear();
        }
        // Next
        NextType::initializeFromArea(study, area);
    }

    size_t getMaxNumberColumns() const
    {
        return pSize * ResultsType::count;
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        // Next
        NextType::initializeFromAreaLink(study, link);
    }

    void simulationBegin()
    {
        // Next
        NextType::simulationBegin();
    }

    void simulationEnd()
    {
        NextType::simulationEnd();
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        // Reset the values for the current year
        for (unsigned int i = 0; i != pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();

            for (unsigned int j = 0; j != maxHoursInAYear; ++j)
            {
                pminOfTheClusterForYear[numSpace][i * maxHoursInAYear + j] = 0;
            }
        }
        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuildPrepareDataForEachThermalCluster(State& state,
                                                      uint year,
                                                      unsigned int numSpace)
    {
        for (unsigned int i = 0; i <= state.study.runtime->rangeLimits.hour[Data::rangeEnd]; ++i)
        {
            state.thermalClusterProductionForYear[i]
              += pValuesForTheCurrentYear[numSpace][state.thermalCluster->areaWideIndex].hour[i];
            state.thermalClusterPMinOfTheClusterForYear[i]
              += pminOfTheClusterForYear[numSpace]
                                        [(state.thermalCluster->areaWideIndex * maxHoursInAYear)
                                         + i];
        }

        // Next variable
        NextType::yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year)
    {
        // Next variable
        NextType::yearEndBuild(state, year);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        // Merge all results for all thermal clusters
        {
            for (unsigned int i = 0; i < pSize; ++i)
            {
                // Compute all statistics for the current year (daily,weekly,monthly)
                pValuesForTheCurrentYear[numSpace][i].computeStatisticsForTheCurrentYear();
            }
        }
        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            for (unsigned int i = 0; i < pSize; ++i)
            {
                // Merge all those values with the global results
                AncestorType::pResults[i].merge(numSpaceToYear[numSpace],
                                                pValuesForTheCurrentYear[numSpace][i]);
            }
        }

        // Next variable
        NextType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        auto& area = state.area;
        auto& thermal = state.thermal;
        for (auto cluster : area->thermal.list.each_enabled())
        {
            // Production for this hour
            pValuesForTheCurrentYear[numSpace][cluster->areaWideIndex]
              .hour[state.hourInTheYear]
              += thermal[area->index].thermalClustersProductions[cluster->areaWideIndex];

            pminOfTheClusterForYear[numSpace][(cluster->areaWideIndex * maxHoursInAYear)
                                              + state.hourInTheYear]
              = thermal[area->index].PMinOfClusters[cluster->areaWideIndex];
        }

        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    inline void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        // Ask to build the digest to the next variable
        NextType::buildDigest(results, digestLevel, dataLevel);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    inline uint64_t memoryUsage() const
    {
        uint64_t r = (sizeof(IntermediateValues) * pSize + IntermediateValues::MemoryUsage())
                         * pNbYearsParallel;
        r += sizeof(double) * pSize * maxHoursInAYear * pNbYearsParallel;
        r += AncestorType::memoryUsage();
        return r;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        // Initializing external pointer on current variable non applicable status
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0])
        {
            assert(NULL != results.data.area);
            const auto& thermal = results.data.area->thermal;

            // Write the data for the current year
            for (auto cluster : thermal.list.each_enabled())
            {
                // Write the data for the current year
                results.variableCaption = cluster->name(); // VCardType::Caption();
                results.variableUnit = VCardType::Unit();
                pValuesForTheCurrentYear[numSpace][cluster->areaWideIndex].template buildAnnualSurveyReport<VCardType>(
                  results, fileLevel, precision);
            }
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    double** pminOfTheClusterForYear;
    size_t pSize;
    unsigned int pNbYearsParallel;

}; // class ProductionByDispatchablePlant

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_ProductionByDispatchablePlant_H__