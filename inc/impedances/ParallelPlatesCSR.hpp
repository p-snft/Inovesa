/******************************************************************************
 * Inovesa - Inovesa Numerical Optimized Vlasov-Equation Solver Algorithms   *
 * Copyright (c) 2014-2016: Patrik Schönfeldt                                 *
 *                                                                            *
 * This file is part of Inovesa.                                              *
 * Inovesa is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * Inovesa is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with Inovesa.  If not, see <http://www.gnu.org/licenses/>.           *
 ******************************************************************************/

#ifndef PARALLELPLATESCSR_HPP
#define PARALLELPLATESCSR_HPP

#include "impedances/Impedance.hpp"

namespace vfps
{

class ParallelPlatesCSR : public Impedance
{
public:
    ParallelPlatesCSR(const size_t n,
                      const frequency_t f_rev,
                      const frequency_t f_max,
                      const double h);

private:
    static std::vector<vfps::impedance_t>
    __calcImpedance(const size_t n,
                    const frequency_t f_rev,
                    const frequency_t f_max,
                    const double h);
};

} // namespace VFPS

#endif // PARALLELPLATESCSR_HPP
