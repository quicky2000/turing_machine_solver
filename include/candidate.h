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

#ifndef TURING_MACHINE_SOLVER_CANDIDATE_H
#define TURING_MACHINE_SOLVER_CANDIDATE_H

#include "quicky_exception.h"
#include <tuple>
#include <ostream>

namespace turing_machine_solver
{
    class candidate
    {
        friend std::ostream & operator<<(std::ostream &, const candidate & );
    public:

        explicit inline
        candidate(unsigned int p_num);

        inline
        candidate(unsigned int p_blue_triangle
                 ,unsigned int p_yellow_square
                 ,unsigned int p_purple_circle
                 );

        inline
        bool
        operator<(const candidate &) const;

        [[nodiscard]]
        unsigned int
        get_blue_triangle() const;

        [[nodiscard]]
        unsigned int
        get_yellow_square() const;

        [[nodiscard]]
        unsigned int
        get_purple_circle() const;

        inline static
        void
        check_number(unsigned int p_value, const std::string & p_name);

    private:
        std::tuple<unsigned int, unsigned int, unsigned int> m_content;
    };

    //-------------------------------------------------------------------------
    candidate::candidate(unsigned int p_blue_triangle
                        ,unsigned int p_yellow_square
                        ,unsigned int p_purple_circle
                        )
    :m_content(p_blue_triangle, p_yellow_square, p_purple_circle)
    {
        check_number(p_blue_triangle, "Blue triangle");
        check_number(p_yellow_square, "Yellow square");
        check_number(p_purple_circle, "Purple circle");
    }

    //-------------------------------------------------------------------------
    candidate::candidate(unsigned int p_num)
    :candidate(p_num / 100
              ,(p_num - (100 * (p_num / 100))) / 10
              , p_num - 10 * ( p_num / 10)
              )
    {

    }

    //-------------------------------------------------------------------------
    void
    candidate::check_number(unsigned int p_value, const std::string & p_name)
    {
        if(p_value < 1 || p_value > 5)
        {
            throw quicky_exception::quicky_logic_exception(p_name + " should be in [1-5] : " + std::to_string(p_value)
                                                          , __LINE__
                                                          , __FILE__
                                                          );
        }
    }

    //-------------------------------------------------------------------------
    unsigned int
    candidate::get_blue_triangle() const
    {
        return std::get<0>(m_content);
    }

    //-------------------------------------------------------------------------
    unsigned int
    candidate::get_yellow_square() const
    {
        return std::get<1>(m_content);
    }

    //-------------------------------------------------------------------------
    unsigned int
    candidate::get_purple_circle() const
    {
        return std::get<2>(m_content);
    }

    //-------------------------------------------------------------------------
    bool
    candidate::operator<(const candidate & p_candidate) const
    {
        return m_content < p_candidate.m_content;
    }

    //-------------------------------------------------------------------------
    std::ostream & operator<<(std::ostream & p_stream, const candidate & p_candidate)
    {
        p_stream << "(" << p_candidate.get_blue_triangle() << " " << p_candidate.get_yellow_square() << " "
        << p_candidate.get_purple_circle() << ")";
        return p_stream;
    }

}
#endif //TURING_MACHINE_SOLVER_CANDIDATE_H
// EOF