/* Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 *                     Peter Gjøl Jensen <root@petergjoel.dk>
 *                     Rasmus Tollund <rtollu18@student.aau.dk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VERIFYPN_PUSHNEGATION_H
#define VERIFYPN_PUSHNEGATION_H

#include <stack>
#include "MutatingVisitor.h"

namespace PetriEngine::PQL {

    Condition_ptr initialMarkingRW(const std::function<Condition_ptr()>& func, negstat_t& stats, const EvaluationContext& context, bool _nested, bool _negated, bool initrw);

    // Uses the visitor below but abstracts away the messiness with instantiating, visiting and grabbing return_value
    Condition_ptr pushNegation(Condition_ptr cond, negstat_t& stats, const EvaluationContext& context, bool nested, bool negated, bool initrw);

    class PushNegationVisitor : public MutatingVisitor {
    public:
        PushNegationVisitor(negstat_t& stats, const EvaluationContext& context, bool nested, bool negated, bool initrw)
            : stats(stats), context(context), initrw(initrw), nested(nested), negated(negated) {}

        Condition_ptr return_value;

    protected:
        bool nested;
        bool negated;

        // This method visits the condition, restores the current arguments and returns the value
        // from the visit.
        inline Condition_ptr subvisit(Condition_ptr condition, bool _nested, bool _negated)
        { return subvisit(&*condition, _nested, _negated); }
        inline Condition_ptr subvisit(Condition* condition, bool _nested, bool _negated) {
            bool old_nested = nested;
            bool old_negated = negated;
            nested = _nested;
            negated = _negated;

            condition->visit(*this);
#ifndef NDEBUG
            assert(has_returned); // Subvisit should return value
            has_returned = false;
#endif

            nested = old_nested;
            negated = old_negated;

            return return_value;
        }

#ifndef NDEBUG
        bool has_returned = false;
#endif

        const EvaluationContext& context;
        negstat_t& stats;
        bool initrw;

        Condition_ptr pushAnd(const std::vector<Condition_ptr> &_conds, bool _nested, bool negate_children);
        Condition_ptr pushOr(const std::vector<Condition_ptr> &_conds, bool nested, bool negate_children);

        template<typename T>
        Condition_ptr pushFireableNegation(const std::string &name, const Condition_ptr &compiled);

        Condition_ptr pushEqual(CompareCondition *org, bool _negated, bool noteq);

        void _accept(NotCondition *element) override;

        void _accept(AndCondition *element) override;

        void _accept(OrCondition *element) override;

        void _accept(LessThanCondition *element) override;

        void _accept(LessThanOrEqualCondition *element) override;

        void _accept(EqualCondition *element) override;

        void _accept(NotEqualCondition *element) override;

        void _accept(DeadlockCondition *element) override;

        void _accept(CompareConjunction *element) override;

        void _accept(UnfoldedUpperBoundsCondition *element) override;

        void _accept(EFCondition *condition) override;

        void _accept(EGCondition *condition) override;

        void _accept(AGCondition *condition) override;

        void _accept(AFCondition *condition) override;

        void _accept(EXCondition *condition) override;

        void _accept(AXCondition *condition) override;

        void _accept(EUCondition *condition) override;

        void _accept(AUCondition *condition) override;

        void _accept(ACondition *condition) override;

        void _accept(ECondition *condition) override;

        void _accept(GCondition *condition) override;

        void _accept(FCondition *condition) override;

        void _accept(XCondition *condition) override;

        void _accept(UntilCondition *condition) override;

        void _accept(UnfoldedFireableCondition *element) override;

        void _accept(BooleanCondition *element) override;

        void _accept(FireableCondition* element) override;

        void _accept(ShallowCondition *element);
    };
}



#endif //VERIFYPN_PUSHNEGATION_H
