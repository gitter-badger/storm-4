#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/FormulaParser.h"
#include "src/logic/FragmentChecker.h"
#include "src/exceptions/WrongFormatException.h"

TEST(FragmentCheckerTest, Propositional) {
    storm::logic::FragmentChecker checker;
    storm::logic::FragmentSpecification prop = storm::logic::propositional();

    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<const storm::logic::Formula> formula;
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("\"label\""));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, prop));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("true"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, prop));

    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("true | \"label\""));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, prop));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("!true"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, prop));

    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("true"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, prop));

    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F true]"));
    EXPECT_FALSE(checker.conformsToSpecification(*formula, prop));

    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("false | P>0.5 [G \"label\"]"));
    EXPECT_FALSE(checker.conformsToSpecification(*formula, prop));

    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F \"label\"]"));
    EXPECT_FALSE(checker.conformsToSpecification(*formula, prop));
}

TEST(FragmentCheckerTest, Pctl) {
    storm::logic::FragmentChecker checker;
    storm::logic::FragmentSpecification pctl = storm::logic::pctl();
    
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<const storm::logic::Formula> formula;
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("\"label\""));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, pctl));

    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F \"label\"]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, pctl));

    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F P=? [F \"label\"]]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, pctl));

    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("R=? [F \"label\"]"));
    EXPECT_FALSE(checker.conformsToSpecification(*formula, pctl));
}

TEST(FragmentCheckerTest, Prctl) {
    storm::logic::FragmentChecker checker;
    storm::logic::FragmentSpecification prctl = storm::logic::prctl();
    
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<const storm::logic::Formula> formula;
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("\"label\""));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, prctl));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F \"label\"]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, prctl));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F P=? [F \"label\"]]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, prctl));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("R=? [F \"label\"]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, prctl));

    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("R=? [C<=3]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, prctl));

    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F[0,1] \"label\"]"));
    EXPECT_FALSE(checker.conformsToSpecification(*formula, prctl));
}

TEST(FragmentCheckerTest, Csl) {
    storm::logic::FragmentChecker checker;
    storm::logic::FragmentSpecification csl = storm::logic::csl();
    
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<const storm::logic::Formula> formula;
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("\"label\""));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, csl));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F \"label\"]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, csl));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F P=? [F \"label\"]]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, csl));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("R=? [F \"label\"]"));
    EXPECT_FALSE(checker.conformsToSpecification(*formula, csl));

    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F[0,1] \"label\"]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, csl));
}

TEST(FragmentCheckerTest, Csrl) {
    storm::logic::FragmentChecker checker;
    storm::logic::FragmentSpecification csrl = storm::logic::csrl();
    
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<const storm::logic::Formula> formula;
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("\"label\""));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, csrl));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F \"label\"]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, csrl));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F P=? [F \"label\"]]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, csrl));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("R=? [F \"label\"]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, csrl));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("R=? [C<=3]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, csrl));
    
    ASSERT_NO_THROW(formula = formulaParser.parseSingleFormulaFromString("P=? [F[0,1] \"label\"]"));
    EXPECT_TRUE(checker.conformsToSpecification(*formula, csrl));
}