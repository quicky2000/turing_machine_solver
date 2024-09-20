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
#include <iostream>

using namespace turing_machine_solver;

//------------------------------------------------------------------------------
int main(int argc,char ** argv)
{
    try
    {
        std::cout << "How many checkers ?" << std::endl;
        int nb_checkers;
        std::cin >> nb_checkers;
        std::cout << "You define " << nb_checkers << " checkers" << std::endl;
        turing_machine_solver::solver l_solver(nb_checkers);

        do
        {
            unsigned int l_candidate_num;
            std::cout << "Propose a candidate ?" << std::endl;
            std::cin >> l_candidate_num;
            candidate l_candidate{l_candidate_num};
            std::string l_checker = l_solver.get_related_checker(l_candidate);
            int l_checker_index;
            unsigned int l_remaining_check = 3;
            do
            {
                std::cout << "Checker index ? ( -1 to propose a new candidate)" << std::endl;
                std::cin >> l_checker_index;
                if(l_checker_index != -1)
                {
                    std::cout << "Checker result ?" << std::endl;
                    bool l_result;
                    std::cin >> l_result;
                    std::cout << "You entered result " << l_result << std::endl;
                    --l_remaining_check;
                    l_solver.analyze_result(l_checker, static_cast<unsigned int>(l_checker_index), l_result);
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
