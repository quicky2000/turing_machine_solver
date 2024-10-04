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
#include "solver.h"
#include "quicky_exception.h"
#include "ask.h"
#include <iostream>

using namespace turing_machine_solver;

//------------------------------------------------------------------------------
int main(int argc,char ** argv)
{
    try
    {
        std::string l_input_values{argc == 2 ? argv[1] : ""};
        ask l_ask{l_input_values};

        std::cout << "How many checkers ?" << std::endl;
        unsigned int nb_checkers{l_ask.next<unsigned int>()};
        std::cout << "You define " << nb_checkers << " checkers" << std::endl;

        solver::register_all_checkers();
        std::vector<unsigned int> l_checkers_id;
        do
        {
            solver::display_all_checkers();
            unsigned int l_id{l_ask.next<unsigned int>()};
            l_checkers_id.emplace_back(l_id);
        } while (l_checkers_id.size() < nb_checkers);

        turing_machine_solver::solver l_solver(l_checkers_id);

        do
        {
            std::cout << "Propose a candidate ?" << std::endl;
            unsigned int l_candidate_num{l_ask.next<unsigned int>()};
            candidate l_candidate{l_candidate_num};
            potential_checkers l_checkers = l_solver.get_related_checkers(l_candidate);
            int l_checker_index;
            unsigned int l_remaining_check = 3;
            do
            {
                std::cout << "Current candidate " << l_candidate << " -> " << l_checkers << std::endl;
                std::cout << "Checker index ? ( -1 to propose a new candidate)" << std::endl;
                l_checker_index = l_ask.next<int>();
                if(l_checker_index != -1)
                {
                    std::cout << "Checker result ?" << std::endl;
                    bool l_result{static_cast<bool>(l_ask.next<unsigned int>())};
                    std::cout << "You entered result " << l_result << std::endl;
                    --l_remaining_check;
                    l_solver.analyze_result(l_checkers, static_cast<unsigned int>(l_checker_index), l_result);
                }
            } while(l_remaining_check && l_checker_index != -1 && l_solver.get_remaining_candidates() > 1);

        } while(l_solver.get_remaining_candidates() > 1);
    }
    catch(quicky_exception::quicky_runtime_exception & e)
    {
        std::cout << "ERROR : " << e.what() << " at " << e.get_file() << ":" << e.get_line() <<std::endl ;
        return(-1);
    }
    catch(quicky_exception::quicky_logic_exception & e)
    {
        std::cout << "ERROR : " << e.what() << " at " << e.get_file() << ":" << e.get_line() << std::endl ;
        return(-1);
    }
    return 0;
}
//EOF
