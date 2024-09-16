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
        std::cout << "How many verifiers ?" << std::endl;
        int nb_verifiers;
        std::cin >> nb_verifiers;
        std::cout << "You define " << nb_verifiers << " verifiers" << std::endl;
        turing_machine_solver::solver l_solver(nb_verifiers);
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
