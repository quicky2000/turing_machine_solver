/*    This file is part of turing_machine_solver
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

#ifndef TURING_MACHINE_SOLVER_CHECKER_BASE_H
#define TURING_MACHINE_SOLVER_CHECKER_BASE_H

#include "candidate.h"
#include "checker_func.h"
#include <string>
#include <utility>
#include <cassert>

namespace turing_machine_solver
{
    template <unsigned int GRADE>
    class checker_base
    {
    public:
        inline
        checker_base(unsigned int p_id
                    ,std::string  p_name
                    ,std::array<checker_func, GRADE> p_funcs);

        [[nodiscard]] inline
        bool
        run(unsigned int p_grade, const candidate & p_candidate) const;

    private:
        unsigned int m_id;

        std::string m_name;

        std::array<checker_func, GRADE> m_funcs;
    };

    //-------------------------------------------------------------------------
    template <unsigned int GRADE>
    checker_base<GRADE>::checker_base(unsigned int p_id
                                     ,std::string  p_name
                                     , std::array<checker_func, GRADE> p_funcs
                                     )
    :m_id{p_id}
    ,m_name{std::move(p_name)}
    ,m_funcs{p_funcs}
    {
    }

    //-------------------------------------------------------------------------
    template <unsigned int GRADE>
    bool
    checker_base<GRADE>::run(unsigned int p_grade
                            ,const candidate & p_candidate
                            )const
    {
        assert(p_grade < GRADE);
        return m_funcs[p_grade].run(p_candidate);
    }
}
#endif //TURING_MACHINE_SOLVER_CHECKER_BASE_H
// EOF