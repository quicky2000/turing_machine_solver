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

#include "checker_base.h"
#include "enumerator.h"
#include "quicky_exception.h"
#include <map>
#include <memory>
#include <iostream>
#include <set>

namespace turing_machine_solver
{
    class solver
    {
    public:
        inline explicit
        solver(unsigned int p_nb_checkers);

        inline static
        std::shared_ptr<checker_if>
        get_checker(unsigned int p_id);

    private:

        [[nodiscard]] inline
        std::string
        get_correct_conditions(const candidate & p_candidate);

        /**
         * Compute all checker combinations depending on their respective grade
         * @param p_max_grade
         */
        inline
        void
        compute_potential_checkers(unsigned int p_max_grade);

        inline static
        void
        register_all_checkers();

        inline static
        void
        register_checker(const std::shared_ptr<checker_if> & p_checker);

        /**
         * Record relation between candidate and checker
         * @param p_candidate
         * @param p_checkers
         * @param p_bad_checkers list of eliminated checkers
         * @param p_candidate_with_bad_checkers list of candidates related to
         * eliminated checkers
         */
        inline
        void
        relate_candidate_checker(const candidate & p_candidate
                                ,const std::string & p_checkers
                                ,std::set<std::string> & p_bad_checkers
                                ,std::set<candidate> & p_candidate_with_bad_checkers
                                );

        std::vector<std::shared_ptr<checker_if>> m_checkers;

        std::set<candidate> m_candidates;

        std::set<std::string> m_potential_checkers;

        std::map<candidate, std::string> m_candidate_to_checkers;

        std::map<std::string, candidate> m_checkers_to_candidate;

        inline static std::map<unsigned int, std::shared_ptr<checker_if>> m_all_checkers;
    };

    //-------------------------------------------------------------------------
    solver::solver(unsigned int p_nb_checkers)
    {
        if(m_all_checkers.empty())
        {
            register_all_checkers();
        }

        unsigned int l_max_grade = 0;
        do
        {
            for(const auto & l_iter: m_all_checkers)
            {
                std::cout << l_iter.first << " " << l_iter.second->get_name() << std::endl;
            }
            unsigned int l_id;
            std::cin >> l_id;
            m_checkers.emplace_back(get_checker(l_id));
            if(l_max_grade < m_checkers.back()->get_grade())
            {
                l_max_grade = m_checkers.back()->get_grade();
            }
        } while (m_checkers.size() < p_nb_checkers);

        std::vector<combinatorics::symbol> l_symbols{{1,5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}};
        combinatorics::enumerator l_enumerator{l_symbols, 3};
        while(l_enumerator.generate())
        {
            l_enumerator.display_word();
            m_candidates.insert({l_enumerator.get_word_item(0), l_enumerator.get_word_item(1), l_enumerator.get_word_item(2)});
        }

        compute_potential_checkers(l_max_grade);

        // Test every candidate with all checkers to restrain candidates
        std::cout << "Candidates matching with checkers:" << std::endl;
        std::vector<candidate> l_bad_candidates;
        std::set<std::string> l_bad_checkers;
        std::set<candidate> l_candidate_with_bad_checkers;
        for(const auto & l_iter: m_candidates)
        {
            std::string l_result = get_correct_conditions(l_iter);
            if(l_result.find('-') == std::string::npos)
            {
                std::cout << l_iter << "->" << l_result << std::endl;
                relate_candidate_checker(l_iter, l_result, l_bad_checkers, l_candidate_with_bad_checkers);
            }
            else
            {
                l_bad_candidates.emplace_back(l_iter);
            }
        }
        std::cout << l_bad_candidates.size() << " candidates not compliant with potential checkers" << std::endl;
        std::cout << l_bad_checkers.size() << " checkers associated with several candidates" << std::endl;
        std::cout << l_candidate_with_bad_checkers.size() << " candidates associated with bad checkers" << std::endl;

        // Remove candidates that have been associated with bad checkers
        // This is done after because removing them on the fly would not allow
        // if several potential checkers match 1 candidate
        for(const auto & l_iter:l_candidate_with_bad_checkers)
        {
            m_candidate_to_checkers.erase(l_iter);
        }

        std::cout << m_candidate_to_checkers.size() << " candidates remaining" << std::endl;
        std::cout << m_checkers_to_candidate.size() << " checkers combinations remaining" << std::endl;
        for(const auto & l_iter: m_candidate_to_checkers)
        {
            std::cout << l_iter.first << " -> " << l_iter.second << std::endl;
        }
    }

    //-------------------------------------------------------------------------
    std::string
    solver::get_correct_conditions(const candidate & p_candidate)
    {
        std::string l_result;
        for(const auto & l_iter:m_checkers)
        {
            auto l_single_result{l_iter->get_correct_condition(p_candidate)};
            l_result += l_single_result ? static_cast<char>(static_cast<unsigned int>('0') + *l_single_result) : '-';
        }
        return l_result;
    }

    //-------------------------------------------------------------------------
    void
    solver::compute_potential_checkers(unsigned int p_max_grade)
    {
        std::vector<combinatorics::symbol> l_symbols;
        for(unsigned int l_grade = 0; l_grade < p_max_grade; ++l_grade)
        {
            l_symbols.emplace_back(l_grade + 1, p_max_grade);
        }
        combinatorics::enumerator l_enumerator{l_symbols, static_cast<unsigned int>(m_checkers.size())};
        while(l_enumerator.generate())
        {
            bool l_ok = true;
            std::string l_str;
            for(unsigned int l_index = 0; l_index < m_checkers.size(); ++l_index)
            {
                unsigned int l_func_index = l_enumerator.get_word_item(l_index) - 1;
                if(l_func_index < m_checkers[l_index]->get_grade())
                {
                    l_str += static_cast<char>('0' + l_func_index);
                }
                else
                {
                    l_ok = false;
                    break;
                }
            }
            if(l_ok)
            {
                std::cout << "Potential checker combination: " << l_str << std::endl;
                m_potential_checkers.insert(l_str);
            }
        }
        std::cout << m_potential_checkers.size() << " checker combinations possible" << std::endl;
    }

    //-------------------------------------------------------------------------
    void
    solver::register_all_checkers()
    {
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (2
                          ,"Le chiffre du triangle bleu comparé à 3"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_blue_triangle() < 3;}
                                        ,"bleu < 3"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() == 3;}
                            ,"bleu == 3"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() > 3;}
                            , "bleu > à 3"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (3
                          ,"Le chiffre du carre jaune comparé à 3"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_yellow_square() < 3;}
                                        ,"jaune < 3"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() == 3;}
                            ,"jaune == 3"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() > 3;}
                             , "jaune > à 3"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (4
                          ,"Le chiffre du carré jaune comparé à  4"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_yellow_square() < 4;}
                                        ,"jaune < 4"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() == 4;}
                            ,"jaune == 4"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() > 4;}
                             , "jaune > à 4"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<2>
                          (7
                          ,"cercle violet pair ou impair"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return !(p_candidate.get_purple_circle() % 2);}
                                        ,"violet est pair"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return (p_candidate.get_purple_circle() % 2);}
                            ,"violet est impair"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<4>
                          (9
                          ,"Le nombre de chiffre 3 dans le code"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return 0 == (p_candidate.get_yellow_square() == 3)
					            + (p_candidate.get_purple_circle() == 3)
						    + (p_candidate.get_blue_triangle() == 3);
					 }
                                        ,"pas de 3"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 1 == (p_candidate.get_yellow_square() == 3)
                                        + (p_candidate.get_purple_circle() == 3)
                                        + (p_candidate.get_blue_triangle() == 3);
                             }
                            ,"un 3"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 2 == (p_candidate.get_yellow_square() == 3)
                                        + (p_candidate.get_purple_circle() == 3)
                                        + (p_candidate.get_blue_triangle() == 3);
                             }
                             , "deux 3"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 3 == (p_candidate.get_yellow_square() == 3)
                                        + (p_candidate.get_purple_circle() == 3)
                                        + (p_candidate.get_blue_triangle() == 3);
                             }
                            ,"trois 3"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (13
                          ,"Le chiffre du carré jaune comparé au cercle violet"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_yellow_square() < p_candidate.get_purple_circle();}
                                        ,"jaune < violet"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() == p_candidate.get_purple_circle();}
                            ,"jaune == violet"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() > p_candidate.get_purple_circle();}
                             , "jaune > à violet"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (14
                          ,"Quelle couleur a le chiffre plus petit que les autres"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_blue_triangle() < p_candidate.get_yellow_square()
					     &&  p_candidate.get_blue_triangle() < p_candidate.get_purple_circle();}
                                        ,"bleu < (jaune && violet)"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() < p_candidate.get_purple_circle()
			          && p_candidate.get_yellow_square() < p_candidate.get_blue_triangle();}
                            ,"jaune < (bleu && violeti)"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_purple_circle() > p_candidate.get_yellow_square()
                                  && p_candidate.get_purple_circle() > p_candidate.get_blue_triangle();}
                             , "violet < (jaune && bleu)"
                            }
                           }
                          )
                         }
                        );
    }

    //-------------------------------------------------------------------------
    std::shared_ptr<checker_if>
    solver::get_checker(unsigned int p_id)
    {
        auto l_iter = m_all_checkers.find(p_id);
        if(m_all_checkers.end() == l_iter)
        {
            throw quicky_exception::quicky_logic_exception("No checker with ID " + std::to_string(p_id), __LINE__, __FILE__);
        }
        return l_iter->second;
    }

    //-------------------------------------------------------------------------
    void
    solver::register_checker(const std::shared_ptr<checker_if> & p_checker)
    {
        assert(!m_all_checkers.contains(p_checker->get_id()));
        m_all_checkers.insert(make_pair(p_checker->get_id(), p_checker));
    }

    //-------------------------------------------------------------------------
    void
    solver::relate_candidate_checker(const candidate & p_candidate
                                    ,const std::string & p_checkers
                                    ,std::set<std::string> & p_bad_checkers
                                    ,std::set<candidate> & p_candidate_with_bad_checkers
                                    )
    {
        assert(!m_candidate_to_checkers.contains(p_candidate));
        auto l_iter = m_checkers_to_candidate.find(p_checkers);
        if(m_checkers_to_candidate.end() == l_iter)
        {
            if(!p_bad_checkers.contains(p_checkers))
            {
                m_candidate_to_checkers.insert(make_pair(p_candidate, p_checkers));
                m_checkers_to_candidate.insert(make_pair(p_checkers, p_candidate));
            }
        }
        else
        {
            p_candidate_with_bad_checkers.insert(l_iter->second);
            m_checkers_to_candidate.erase(l_iter);
            p_bad_checkers.insert(p_checkers);
        }
    }


}
#endif //TURING_MACHINE_SOLVER_SOLVER_H
// EOF
