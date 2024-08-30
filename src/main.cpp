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
#include "checker_func.h"
#include "checker_base.h"
#include "checker_if.h"
#include "quicky_exception.h"
#include <iostream>

using namespace turing_machine_solver;

//------------------------------------------------------------------------------
int main(int argc,char ** argv)
{
    try
    {
        std::cout << "How many verifiers ?" << std::endl;
        int nb_verifiers;
        std::cin >> nb_verifiers;
        std::cout << "You define " << nb_verifiers << " verifiers" << std::endl;
        turing_machine_solver::solver l_solver(nb_verifiers);
        //std::array l_array;

        checker_base<3> l_checker(4, "Le chiffre du carré jaune comparé à  4",
                                  {checker_func{[](const candidate &p_candidate) -> bool
                                     {return p_candidate.get_yellow_square() < 4;}
                                    ,"jaune < 4"
                                    },
                                    {[](const candidate &p_candidate) -> bool
                                     {return p_candidate.get_yellow_square() == 4;}
                                    , "jaune == 4"
                                    },
                                    {[](const candidate &p_candidate) -> bool
                                     {return p_candidate.get_yellow_square() > 4;}
                                    , "jaune > à 4"
                                    }
                                   }
                                  );
        checker_if * l_checker_if = &l_checker;
        checker_func l_func{[](const candidate & p_candidate) -> bool {return p_candidate.get_purple_circle() == 3;}, "check"};
        candidate l_candidate{1,2,3};
        std::cout << l_checker_if->run(0, l_candidate) << std::endl;
        std::cout << l_checker_if->run(1, l_candidate) << std::endl;
        std::cout << l_checker_if->run(2, l_candidate) << std::endl;
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
