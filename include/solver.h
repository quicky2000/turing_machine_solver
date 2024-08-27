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

#ifndef TURING_MACHINE_SOLVER_SOLVER_H
#define TURING_MACHINE_SOLVER_SOLVER_H

namespace turing_machine_solver
{
    class solver
    {
    public:
        inline
        solver(unsigned int p_nb_verifiers);

    private:
        unsigned int m_nb_verifiers;
    };

    //-------------------------------------------------------------------------
    solver::solver(unsigned int p_nb_verifiers)
    :m_nb_verifiers{p_nb_verifiers}
    {

    }
}
#endif //TURING_MACHINE_SOLVER_SOLVER_H
// EOF