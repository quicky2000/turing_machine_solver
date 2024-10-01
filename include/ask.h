/*    This file is part of turing_mahcine_solver
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

#ifndef TURING_MACHINE_SOLVER_ASK_H
#define TURING_MACHINE_SOLVER_ASK_H

#include "quicky_exception.h"
#include <string>
#include <queue>
#include <charconv>
#include <cassert>
#include <sstream>
#include <fstream>

namespace turing_machine_solver
{
    class ask
    {
    public:
        inline
        ask(const std::string & p_string);

        inline
        ~ask();

        template <typename T>
        [[nodiscard]] inline
        T
        next();

    private:
        std::queue<std::string> m_fifo;
        std::ofstream m_log_file;
        bool m_on_going;
    };

    //-------------------------------------------------------------------------
    ask::ask(const std::string & p_string)
    : m_log_file("turing.log")
    , m_on_going{false}
    {
        std::string l_token;
        std::stringstream l_stream;
        l_stream << p_string;
        while(std::getline(l_stream, l_token, ','))
        {
            m_fifo.push(l_token);
        }
    }

    //-------------------------------------------------------------------------
    ask::~ask()
    {
        m_log_file << std::endl;
        m_log_file.close();
    }

    //-------------------------------------------------------------------------
    template <typename T>
    T
    ask::next()
    {
        T l_result;
        if(m_fifo.empty())
        {
            std::cin >> l_result;
        }
        else
        {
            std::string l_value = m_fifo.front();
            m_fifo.pop();
            auto [ptr, l_status] = std::from_chars(l_value.data(), l_value.data() + l_value.size(), l_result);
            if (l_status == std::errc())
            {
                std::cout << l_result << std::endl;
            } else if (l_status == std::errc::invalid_argument)
            {
                throw quicky_exception::quicky_logic_exception("Invalid ask argument " + l_value
                                                              , __LINE__
                                                              , __FILE__
                                                              );
            } else if (l_status == std::errc::result_out_of_range) {
                throw quicky_exception::quicky_logic_exception("Out of range argument " + l_value
                                                              , __LINE__
                                                              , __FILE__
                                                              );
            }
            else
            {
                throw quicky_exception::quicky_logic_exception("Unknown ask exception"
                                                              , __LINE__
                                                              , __FILE__
                                                              );
            }
        }
        if(m_on_going)
        {
            m_log_file << ",";
        }
        else
        {
            m_on_going = true;
        }
        m_log_file << l_result;
        return l_result;
    }
}
#endif //TURING_MACHINE_SOLVER_ASK_H
// EOF