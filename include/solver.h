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

#include "potential_checkers.h"
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
        solver(const std::vector<unsigned int> & p_checkers_id);

        inline static
        std::shared_ptr<checker_if>
        get_checker(unsigned int p_id);


        [[nodiscard]] inline
        unsigned int
        get_remaining_candidates() const;

        inline
        void
        analyze_result(const potential_checkers & p_checkers
                      ,unsigned int p_checker_index
                      ,bool l_result
                      );

        [[nodiscard]] inline
        potential_checkers
        get_related_checkers(const candidate & p_candidate) const;

        inline static
        void
        display_all_checkers();

        inline static
        void
        register_all_checkers();

    private:

        inline
        void
        display_remaining();

        [[nodiscard]] inline
        potential_checkers
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
                                ,const potential_checkers & p_checkers
                                ,std::set<potential_checkers> & p_bad_checkers
                                ,std::set<candidate> & p_candidate_with_bad_checkers
                                );

        std::vector<std::shared_ptr<checker_if>> m_checkers;

        std::set<candidate> m_candidates;

        std::set<std::string> m_potential_checkers;

        std::map<candidate, potential_checkers> m_candidate_to_checkers;

        std::map<potential_checkers, candidate> m_checkers_to_candidate;

        inline static std::map<unsigned int, std::shared_ptr<checker_if>> m_all_checkers;
    };

    //-------------------------------------------------------------------------
    solver::solver(const std::vector<unsigned int> & p_checkers_id)
    {
        if(m_all_checkers.empty())
        {
            throw quicky_exception::quicky_logic_exception("Checkers have not been registered"
                                                          ,__LINE__
                                                          ,__FILE__
                                                          );
        }

        unsigned int l_max_grade = 0;
        for(const auto & l_iter_id: p_checkers_id)
        {
            m_checkers.emplace_back(get_checker(l_iter_id));
            if(l_max_grade < m_checkers.back()->get_grade())
            {
                l_max_grade = m_checkers.back()->get_grade();
            }
        }

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
        std::set<potential_checkers> l_bad_checkers;
        std::set<candidate> l_candidate_with_bad_checkers;
        for(const auto & l_iter: m_candidates)
        {
            auto l_result = get_correct_conditions(l_iter);
            if(l_result.is_valid())
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

        display_remaining();
    }

    //-------------------------------------------------------------------------
    void
    solver::display_all_checkers()
    {
        for(const auto & l_iter: m_all_checkers)
        {
            std::cout << l_iter.first << " " << l_iter.second->get_name() << std::endl;
        }
    }

    //-------------------------------------------------------------------------
    void
    solver::display_remaining()
    {
        std::cout << m_candidate_to_checkers.size() << " candidates remaining" << std::endl;
        std::cout << m_checkers_to_candidate.size() << " checkers combinations remaining" << std::endl;
        if(m_candidate_to_checkers.size() == 1)
        {
            std::cout << "SOLUTION FOUND :";
        }
        for(const auto & l_iter: m_candidate_to_checkers)
        {
            std::cout << l_iter.first << " -> " << l_iter.second << std::endl;
        }
    }

    //-------------------------------------------------------------------------
    potential_checkers
    solver::get_related_checkers(const candidate & p_candidate) const
    {
        auto l_iter = m_candidate_to_checkers.find(p_candidate);
        if(l_iter == m_candidate_to_checkers.end())
        {
            throw quicky_exception::quicky_logic_exception("Bad candidate", __LINE__, __FILE__);
        }
        return l_iter->second;
    }

    //-------------------------------------------------------------------------
    void
    solver::analyze_result(const potential_checkers & p_checkers
                          ,unsigned int p_checker_index
                          ,bool l_result
                          )
    {
        if(p_checker_index > m_checkers.size())
        {
            throw quicky_exception::quicky_logic_exception("Bad checker value " + std::to_string(p_checker_index) + ", should be in range [0," + std::to_string(m_checkers.size() - 1) + ']'
                                                          , __LINE__
                                                          , __FILE__
                                                          );
        }
        std::vector<potential_checkers> l_bad_checkers;
        std::vector<candidate> l_bad_candidates;
        for(const auto & l_iter_candidate: m_candidate_to_checkers)
        {
            if(!l_iter_candidate.second.is_compliant_with(p_checker_index, p_checkers, l_result))
            {
                l_bad_checkers.emplace_back(l_iter_candidate.second);
                l_bad_candidates.emplace_back(l_iter_candidate.first);
            }
        }
        for(const auto & l_iter_candidate: l_bad_candidates)
        {
            m_candidate_to_checkers.erase(l_iter_candidate);
        }
        for(const auto & l_iter_checker: l_bad_checkers)
        {
            m_checkers_to_candidate.erase(l_iter_checker);
        }
        display_remaining();
    }

    //-------------------------------------------------------------------------
    potential_checkers
    solver::get_correct_conditions(const candidate & p_candidate)
    {
        potential_checkers l_result;
        for(const auto & l_iter:m_checkers)
        {
            l_result.add(l_iter->get_correct_conditions(p_candidate));
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
                          (5
                          ,"Triangle bleu est pair ou impair"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return !(p_candidate.get_blue_triangle() % 2);}
                                        ,"Bleu est pair"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return (p_candidate.get_blue_triangle() % 2);}
                            ,"Jaune est impair"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<2>
                          (6
                          ,"Carre jaune est pair ou impair"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return !(p_candidate.get_yellow_square() % 2);}
                                        ,"Jaune est pair"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return (p_candidate.get_yellow_square() % 2);}
                            ,"Jaune est impair"
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
                          (8
                          ,"Le nombre de chiffre 1 dans le code"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return 0 == (p_candidate.get_yellow_square() == 1)
					            + (p_candidate.get_purple_circle() == 1)
						    + (p_candidate.get_blue_triangle() == 1);
					 }
                                        ,"pas de 1"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 1 == (p_candidate.get_yellow_square() == 1)
                                        + (p_candidate.get_purple_circle() == 1)
                                        + (p_candidate.get_blue_triangle() == 1);
                             }
                            ,"un 1"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 2 == (p_candidate.get_yellow_square() == 1)
                                        + (p_candidate.get_purple_circle() == 1)
                                        + (p_candidate.get_blue_triangle() == 1);
                             }
                             , "deux 1"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 3 == (p_candidate.get_yellow_square() == 1)
                                        + (p_candidate.get_purple_circle() == 1)
                                        + (p_candidate.get_blue_triangle() == 1);
                             }
                            ,"trois 1"
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
                         {new checker_base<4>
                          (10
                          ,"Le nombre de chiffre 4 dans le code"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return 0 == (p_candidate.get_yellow_square() == 4)
					            + (p_candidate.get_purple_circle() == 4)
						    + (p_candidate.get_blue_triangle() == 4);
					 }
                                        ,"pas de 4"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 1 == (p_candidate.get_yellow_square() == 4)
                                        + (p_candidate.get_purple_circle() == 4)
                                        + (p_candidate.get_blue_triangle() == 4);
                             }
                            ,"un 4"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 2 == (p_candidate.get_yellow_square() == 4)
                                        + (p_candidate.get_purple_circle() == 4)
                                        + (p_candidate.get_blue_triangle() == 4);
                             }
                             , "deux 4"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 3 == (p_candidate.get_yellow_square() == 4)
                                        + (p_candidate.get_purple_circle() == 4)
                                        + (p_candidate.get_blue_triangle() == 4);
                             }
                            ,"trois 4"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (11
                          ,"Le chiffre du triangle bleu comparé au carre jaune"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_blue_triangle() < p_candidate.get_yellow_square();}
                                        ,"bleu < jaune"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() == p_candidate.get_yellow_square();}
                            ,"bleu == jaune"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() > p_candidate.get_yellow_square();}
                             , "bleu > à jaune"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (12
                          ,"Le chiffre du triangle bleu comparé au cercle violet"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_blue_triangle() < p_candidate.get_purple_circle();}
                                        ,"bleu < violet"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() == p_candidate.get_purple_circle();}
                            ,"bleu == violet"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() > p_candidate.get_purple_circle();}
                             , "bleu > à violet"
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
                             {return p_candidate.get_purple_circle() < p_candidate.get_yellow_square()
                                  && p_candidate.get_purple_circle() < p_candidate.get_blue_triangle();}
                             , "violet < (jaune && bleu)"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (15
                          ,"Quelle couleur a le chiffre plus grand que les autres"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_blue_triangle() > p_candidate.get_yellow_square()
					     &&  p_candidate.get_blue_triangle() > p_candidate.get_purple_circle();}
                                        ,"bleu > (jaune && violet)"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() > p_candidate.get_purple_circle()
			          && p_candidate.get_yellow_square() > p_candidate.get_blue_triangle();}
                            ,"jaune > (bleu && violeti)"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_purple_circle() > p_candidate.get_yellow_square()
                                  && p_candidate.get_purple_circle() > p_candidate.get_blue_triangle();}
                             , "violet > (jaune && bleu)"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<2>
                          (16
                          ,"Le nombre de chiffres pairs compare au nombre de chiffres impairs"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {unsigned int l_nb_impair = (p_candidate.get_purple_circle() % 2) +
					                             (p_candidate.get_yellow_square() % 2) +
								     (p_candidate.get_blue_triangle() % 2);
                                          return 3 - l_nb_impair > l_nb_impair;
					 }
                                        ,"Pair > Impair"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {unsigned int l_nb_impair = (p_candidate.get_purple_circle() % 2) +
			                                 (p_candidate.get_yellow_square() % 2) +
			                                 (p_candidate.get_blue_triangle() % 2);
                              return 3 - l_nb_impair < l_nb_impair;
			     }
                            ,"Pair < impair"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<4>
                          (17
                          ,"Le nombre de chiffre pair dans le code"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return 3 == (p_candidate.get_yellow_square() % 2)
					            + (p_candidate.get_purple_circle() % 2)
						    + (p_candidate.get_blue_triangle() % 2);
					 }
                                        ,"aucun chiffre pair"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 2 == (p_candidate.get_yellow_square() % 2)
                                        + (p_candidate.get_purple_circle() % 2)
                                        + (p_candidate.get_blue_triangle() % 2);
                             }
                            ,"un chiffres pair"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 1 == (p_candidate.get_yellow_square() % 2)
                                        + (p_candidate.get_purple_circle() % 2)
                                        + (p_candidate.get_blue_triangle() % 2);
                             }
                             ,"deux chiffre pair"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 0 == (p_candidate.get_yellow_square() % 2)
                                        + (p_candidate.get_purple_circle() % 2)
                                        + (p_candidate.get_blue_triangle() % 2);
                             }
                            ,"trois chiffres pair"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<2>
                          (18
                          ,"La somme de tous les chiffres est paire ou impaire"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return !((p_candidate.get_yellow_square()
					          + p_candidate.get_purple_circle()
						  + p_candidate.get_blue_triangle()
                                                   ) % 2
                                                  );
					 }
                                        ,"La somme des chiffres est paire"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return ((p_candidate.get_yellow_square()
                                     + p_candidate.get_purple_circle()
                                     + p_candidate.get_blue_triangle()
				      ) % 2
				     );
                             }
                            ,"La somme des chiffres est impaire"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (19
                          ,"La somme du triangle bleu et du carre jaune comparee a 6"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return (p_candidate.get_blue_triangle() + p_candidate.get_yellow_square()) < 6;}
                                        ,"bleu + jaune < 6"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return (p_candidate.get_blue_triangle() + p_candidate.get_yellow_square()) == 6;}
                            ,"bleu + jaune = 6"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return (p_candidate.get_blue_triangle() + p_candidate.get_yellow_square()) > 6;}
                             , "bleu + jaune > 6"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (20
                          ,"Un chiffre se repete dans le code"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return (p_candidate.get_blue_triangle() == p_candidate.get_yellow_square() && p_candidate.get_blue_triangle() == p_candidate.get_purple_circle());}
                                        ,"un chiffre triple"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return 2 == ((p_candidate.get_blue_triangle() == p_candidate.get_yellow_square()) +
					   (p_candidate.get_blue_triangle() == p_candidate.get_purple_circle()) +
			                   (p_candidate.get_yellow_square() == p_candidate.get_purple_circle())
                                          );
			     }
                            ,"un chiffre double"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return (p_candidate.get_blue_triangle() != p_candidate.get_yellow_square() &&
                                      p_candidate.get_blue_triangle() != p_candidate.get_purple_circle() &&
				      p_candidate.get_yellow_square() != p_candidate.get_purple_circle()
                                     );
                             }
                             , "pas de repetition"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<2>
                          (21
                          ,"Un chiffre est prsent exactement 2 fois dans le code"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {unsigned int l_repeat[5] = {0,0,0,0,0};
                                          l_repeat[p_candidate.get_blue_triangle()]++;
                                          l_repeat[p_candidate.get_yellow_square()]++;
                                          l_repeat[p_candidate.get_purple_circle()]++;
                                         return l_repeat[p_candidate.get_blue_triangle()] != 2 && l_repeat[p_candidate.get_yellow_square()] != 2 && l_repeat[p_candidate.get_purple_circle()] != 2;
                                         }
                                        ,"Pas de paire"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {unsigned int l_repeat[5] = {0,0,0,0,0};
                              l_repeat[p_candidate.get_blue_triangle()]++;
                              l_repeat[p_candidate.get_yellow_square()]++;
                              l_repeat[p_candidate.get_purple_circle()]++;
                              return l_repeat[p_candidate.get_blue_triangle()] == 2 || l_repeat[p_candidate.get_yellow_square()] == 2 || l_repeat[p_candidate.get_purple_circle()] == 2;
                             }
                            ,"Une paire"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (23
                          ,"La somme de tous les chiffres comparee a 6"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return (p_candidate.get_blue_triangle() + p_candidate.get_yellow_square() + p_candidate.get_purple_circle()) < 6;}
                                        ,"La somme est plus petite que 6"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return (p_candidate.get_blue_triangle() + p_candidate.get_yellow_square() + p_candidate.get_purple_circle()) == 6;}
                            ,"La omme est egale a 6"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return (p_candidate.get_blue_triangle() + p_candidate.get_yellow_square() + p_candidate.get_purple_circle()) > 6;}
                             , "La somme est superieure a 6"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (24
                          ,"Il y a une suite croissante de chiffres consecutifs"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return ((p_candidate.get_blue_triangle() == (p_candidate.get_yellow_square() - 1))
                                               && (p_candidate.get_yellow_square() == (p_candidate.get_purple_circle() - 1))
                                                 );
                                         }
                                        ,"3 chiffres en ordre croissant consecutifs"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return ((p_candidate.get_blue_triangle() == (p_candidate.get_yellow_square() - 1)) 
                                    ^ (p_candidate.get_yellow_square() == (p_candidate.get_purple_circle() - 1))
                                     );
                                     ;
			     }
                            ,"2 chiffres en ordre croissant consecutif"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return ((p_candidate.get_blue_triangle() != (p_candidate.get_yellow_square() - 1)) 
                                   && (p_candidate.get_yellow_square() != (p_candidate.get_purple_circle() - 1))
                                     );
                             }
                             ,"Pas de chiffres en ordre croissant consecutif"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (25
                          ,"Il y a une suite croissante ou decroissante de chiffres"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return !((p_candidate.get_blue_triangle() < p_candidate.get_yellow_square() && p_candidate.get_yellow_square() < p_candidate.get_purple_circle()) ||
                                                   (p_candidate.get_blue_triangle() > p_candidate.get_yellow_square() && p_candidate.get_yellow_square() > p_candidate.get_purple_circle())
                                                  );
                                         }
                                        ,"Pas de suite croissante ou decroissante de chiffre"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return ((p_candidate.get_blue_triangle() < p_candidate.get_yellow_square()) && !(p_candidate.get_yellow_square() < p_candidate.get_purple_circle())) ||
                                     (!(p_candidate.get_blue_triangle() < p_candidate.get_yellow_square()) && (p_candidate.get_yellow_square() < p_candidate.get_purple_circle())) ||
                                     ((p_candidate.get_blue_triangle() > p_candidate.get_yellow_square()) && !(p_candidate.get_yellow_square() > p_candidate.get_purple_circle())) ||
                                     (!(p_candidate.get_blue_triangle() > p_candidate.get_yellow_square()) && (p_candidate.get_yellow_square() > p_candidate.get_purple_circle()))
                                     ;
			     }
                            ,"2 chiffres en ordre croissant ou decroissant"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return ((p_candidate.get_blue_triangle() < p_candidate.get_yellow_square() && p_candidate.get_yellow_square() < p_candidate.get_purple_circle()) ||
                                      (p_candidate.get_blue_triangle() > p_candidate.get_yellow_square() && p_candidate.get_yellow_square() > p_candidate.get_purple_circle())
                                     );
                             }
                             , "3 chiffres en ordre croissant ou decroissant"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (28
                          ,"Une couleur specifique est egale a 1"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_blue_triangle() == 1;}
                                        ,"Bleu egal a 1"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() == 1;}
                            ,"Jaune egal a 1"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_purple_circle() == 1;}
                             , "Violet est egal a 1"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<6>
                          (33
                          ,"Une couleur specifique est paire ou impaire"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return !(p_candidate.get_blue_triangle() % 2);}
                                        ,"bleu est pair"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return !(p_candidate.get_yellow_square() % 2);}
                            ,"jaune est pair"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return !(p_candidate.get_purple_circle() % 2);}
                             , "violet est pair"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return (p_candidate.get_blue_triangle() % 2);}
                            ,"bleu est impair"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return (p_candidate.get_yellow_square() % 2);}
                            ,"jaune est impair"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return (p_candidate.get_purple_circle() % 2);}
                             , "violet est impair"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (34
                          ,"Quelle couleur a le chiffre plus petit ( ou a egalite avec le chiffre le plus petit )"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_blue_triangle() <= p_candidate.get_yellow_square()
                                             &&  p_candidate.get_blue_triangle() <= p_candidate.get_purple_circle();
                                         }
                                        ,"bleu <= (jaune && violet)"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() <= p_candidate.get_purple_circle()
                                  && p_candidate.get_yellow_square() <= p_candidate.get_blue_triangle();
                             }
                            ,"jaune <= (bleu && violeti)"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_purple_circle() <= p_candidate.get_yellow_square()
                                  && p_candidate.get_purple_circle() <= p_candidate.get_blue_triangle();}
                             , "violet <= (jaune && bleu)"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<3>
                          (35
                          ,"Quelle couleur a le chiffre plus grand ( ou a egalite avec le chiffre le plus grand )"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_blue_triangle() >= p_candidate.get_yellow_square()
                                             &&  p_candidate.get_blue_triangle() >= p_candidate.get_purple_circle();
                                         }
                                        ,"bleu >= (jaune && violet)"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() >= p_candidate.get_purple_circle()
                                  && p_candidate.get_yellow_square() >= p_candidate.get_blue_triangle();
                             }
                            ,"jaune >= (bleu && violeti)"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_purple_circle() >= p_candidate.get_yellow_square()
                                  && p_candidate.get_purple_circle() >= p_candidate.get_blue_triangle();}
                             , "violet >= (jaune && bleu)"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<6>
                          (42
                          ,"Quelle couleur est le plus petit ou le plus grand"
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
                             {return p_candidate.get_purple_circle() < p_candidate.get_yellow_square()
                                  && p_candidate.get_purple_circle() < p_candidate.get_blue_triangle();}
                             , "violet < (jaune && bleu)"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() > p_candidate.get_yellow_square()
                                 &&  p_candidate.get_blue_triangle() > p_candidate.get_purple_circle();}
                             ,"bleu > (jaune && violet)"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() > p_candidate.get_purple_circle()
                                  && p_candidate.get_yellow_square() > p_candidate.get_blue_triangle();}
                            ,"jaune > (bleu && violeti)"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_purple_circle() > p_candidate.get_yellow_square()
                                  && p_candidate.get_purple_circle() > p_candidate.get_blue_triangle();}
                             , "violet > (jaune && bleu)"
                            }
                           }
                          )
                         }
                        );
        register_checker(std::shared_ptr<checker_if>
                         {new checker_base<9>
                          (48
                          ,"Une couleur specifique comparee a une autre couleur specifique"
                          ,{checker_func{[](const candidate &p_candidate) -> bool
                                         {return p_candidate.get_blue_triangle() < p_candidate.get_yellow_square();}
                                        ,"bleu < jaune"
                                        }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() < p_candidate.get_purple_circle();}
                            ,"bleu < violet"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() < p_candidate.get_purple_circle();}
                            , "jaune < violet"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() == p_candidate.get_yellow_square();}
                            ,"jaune = violet"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() == p_candidate.get_purple_circle();}
                            ,"bleu = violet"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() == p_candidate.get_purple_circle();}
                            , "jaune = violet"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() > p_candidate.get_yellow_square();}
                            ,"jaune > violet"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_blue_triangle() > p_candidate.get_purple_circle();}
                            ,"bleu > violet"
                            }
                           ,{[](const candidate &p_candidate) -> bool
                             {return p_candidate.get_yellow_square() > p_candidate.get_purple_circle();}
                            , "jaune > violet"
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
                                    ,const potential_checkers & p_checkers
                                    ,std::set<potential_checkers> & p_bad_checkers
                                    ,std::set<candidate> & p_candidate_with_bad_checkers
                                    )
    {
        assert(!m_candidate_to_checkers.contains(p_candidate));
        auto l_iter = m_checkers_to_candidate.find(p_checkers);
        if(m_checkers_to_candidate.end() == l_iter)
        {
            if(!p_bad_checkers.contains(p_checkers))
            {
                m_candidate_to_checkers.insert(std::make_pair(p_candidate, p_checkers));
                m_checkers_to_candidate.insert(std::make_pair(p_checkers, p_candidate));
            }
        }
        else
        {
            p_candidate_with_bad_checkers.insert(l_iter->second);
            m_checkers_to_candidate.erase(l_iter);
            p_bad_checkers.insert(p_checkers);
        }
    }

    //-------------------------------------------------------------------------
    unsigned int
    solver::get_remaining_candidates() const
    {
        return m_candidate_to_checkers.size();
    }



}
#endif //TURING_MACHINE_SOLVER_SOLVER_H
// EOF
