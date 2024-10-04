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

#include "quicky_exception.h"
#include <cassert>
#include <ostream>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>

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

        [[nodiscard]] inline
        bool
        is_valid() const;

        [[nodiscard]] inline
        bool
        is_compliant_with(unsigned int p_index
                         ,const potential_checkers & p_checkers
                         ,bool l_checker_result
                         ) const;

        inline
        bool
        operator<(const potential_checkers & p_checkers) const;

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
    bool
    potential_checkers::is_valid() const
    {
        return std::all_of(m_content.begin()
                          ,m_content.end()
                          ,[](const std::set<unsigned int> & p_item)
                             {return !p_item.empty();}
                          );
    }

    //-------------------------------------------------------------------------
    bool
    potential_checkers::is_compliant_with(unsigned int p_index
                                         ,const potential_checkers & p_checkers
                                         ,bool p_checker_result
                                         ) const
    {
        assert(m_content.size() == p_checkers.m_content.size());
        if(p_index < m_content.size())
        {
            if(m_content[p_index] == p_checkers.m_content[p_index])
            {
                return p_checker_result;
            }
            else
            {
                std::vector<unsigned int> l_intersection;
                std::set_intersection(m_content[p_index].begin()
                                     ,m_content[p_index].end()
                                     ,p_checkers.m_content[p_index].begin()
                                     ,p_checkers.m_content[p_index].end()
                                     ,std::back_inserter(l_intersection)
                                     );
                if(l_intersection.empty())
                {
                    return !p_checker_result;
                }
                else
                {
                    return true;
                }
            }
        }
        throw quicky_exception::quicky_logic_exception("Bad index " + std::to_string(p_index)
                                                      ,__LINE__
                                                      ,__FILE__
                                                      );
    }

    //-------------------------------------------------------------------------
    bool
    potential_checkers::operator<(const potential_checkers & p_checkers) const
    {
        assert(m_content.size() == p_checkers.m_content.size());
        for(unsigned int l_index = 0; l_index < m_content.size(); l_index++)
        {
            if(!std::equal(m_content[l_index].begin()
                          ,m_content[l_index].end()
                          ,p_checkers.m_content[l_index].begin()
                          ,p_checkers.m_content[l_index].end()
                          )
              )
            {
                return std::lexicographical_compare(m_content[l_index].begin()
                                                   ,m_content[l_index].end()
                                                   ,p_checkers.m_content[l_index].begin()
                                                   ,p_checkers.m_content[l_index].end()
                                                   );
            }

        }
        return false;
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