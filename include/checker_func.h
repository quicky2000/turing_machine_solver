/*    This file is part of solver
      Copyright (C) 2024  Julien Thevenon ( julien_thevenon at yahoo.fr )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef TURING_MACHINE_SOLVER_CHECKER_FUNC_H
#define TURING_MACHINE_SOLVER_CHECKER_FUNC_H

#include "candidate.h"
#include <functional>
#include <utility>

namespace turing_machine_solver
{
    class checker_func
    {
    public:
        checker_func(std::function<bool(const candidate &)> p_func
                    ,std::string p_description);

        [[nodiscard]]
        bool
        run(const candidate & p_candidate) const;

    private:
        std::function<bool(const candidate &)> m_func;

        std::string m_description;
    };

    //-------------------------------------------------------------------------
    checker_func::checker_func(std::function<bool(const candidate &)> p_func
                              ,std::string p_description
                              )
    :m_func{std::move(p_func)}
    ,m_description{std::move(p_description)}
    {

    }

    //-------------------------------------------------------------------------
    bool
    checker_func::run(const turing_machine_solver::candidate & p_candidate) const
    {
        return m_func(p_candidate);
    }
}
#endif //TURING_MACHINE_SOLVER_CHECKER_FUNC_H
// EOF