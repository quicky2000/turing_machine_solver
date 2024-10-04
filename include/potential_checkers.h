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

#ifndef TURING_MACHINE_SOLVER_POTENTIAL_CHECKERS_H
#define TURING_MACHINE_SOLVER_POTENTIAL_CHECKERS_H

#include <ostream>
#include <vector>
#include <set>
#include <sstream>
namespace turing_machine_solver
{
    class potential_checkers
    {
        friend std::ostream & operator<<(std::ostream &, const potential_checkers & );

    public:
        inline explicit
        potential_checkers() = default;

        inline
        void
        add(std::set<unsigned int> && p_value);

    private:

        std::vector<std::set<unsigned int>> m_content;
    };

    //-------------------------------------------------------------------------
    void
    potential_checkers::add(std::set<unsigned int> && p_value)
    {
        m_content.emplace_back(std::move(p_value));
    }

    //-------------------------------------------------------------------------
    std::ostream &
    operator<<(std::ostream & p_stream
              ,const potential_checkers & p_potential_checkers
              )
    {
        for(const auto & l_iter: p_potential_checkers.m_content)
        {
            if(l_iter.size() == 1)
            {
                p_stream << *l_iter.begin();
            }
            else if(l_iter.size() > 1)
            {
                p_stream << "(" ;
                for(const auto & l_iter_value: l_iter)
                {
                    p_stream << l_iter_value;
                }
                p_stream << ")";
            }
            else
            {
                p_stream << "-";
            }
        }
        return p_stream;
    }
}
#endif //TURING_MACHINE_SOLVER_POTENTIAL_CHECKERS_H
// EOF