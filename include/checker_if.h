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

#ifndef TURING_MACHINE_SOLVER_CHECKER_IF_H
#define TURING_MACHINE_SOLVER_CHECKER_IF_H

namespace turing_machine_solver
{
    class candidate;

    class checker_if
    {
    public:
        [[nodiscard]]
        virtual
        unsigned int
        get_id() const = 0;

        [[nodiscard]]
        virtual
        unsigned int
        get_grade() const = 0;

        [[nodiscard]]
        virtual
        bool
        run(unsigned int p_grade, const candidate & p_candidate) const = 0;
    private:
    };
}
#endif //TURING_MACHINE_SOLVER_CHECKER_IF_H
// EOF