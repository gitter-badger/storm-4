#include <storm-pars/modelchecker/region/RegionResultHypothesis.h>
#include "storm-pars/settings/modules/RegionSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalArgumentValueException.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string RegionSettings::moduleName = "region";
            const std::string RegionSettings::regionOptionName = "region";
            const std::string RegionSettings::regionShortOptionName = "reg";
            const std::string RegionSettings::hypothesisOptionName = "hypothesis";
            const std::string RegionSettings::hypothesisShortOptionName = "hyp";
            const std::string RegionSettings::refineOptionName = "refine";
            const std::string RegionSettings::checkEngineOptionName = "engine";
            const std::string RegionSettings::printNoIllustrationOptionName = "noillustration";
            const std::string RegionSettings::printFullResultOptionName = "printfullresult";
            
            RegionSettings::RegionSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, regionOptionName, false, "Sets the region(s) considered for analysis.").setShortName(regionShortOptionName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("regioninput", "The region(s) given in format a<=x<=b,c<=y<=d seperated by ';'. Can also be a file.").build()).build());
                
                std::vector<std::string> hypotheses = {"unknown", "allsat", "allviolated"};
                this->addOption(storm::settings::OptionBuilder(moduleName, hypothesisOptionName, false, "Sets a hypothesis for region analysis. If given, the region(s) are only analyzed w.r.t. that hypothesis.").setShortName(hypothesisShortOptionName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("hypothesis", "The hypothesis.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(hypotheses)).setDefaultValueString("unknown").build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, refineOptionName, false, "Enables region refinement.")
                                .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("coverage-threshold", "Refinement converges if the fraction of unknown area falls below this threshold.").setDefaultValueDouble(0.05).addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleRangeValidatorIncluding(0.0,1.0)).build())
                                .addArgument(storm::settings::ArgumentBuilder::createIntegerArgument("depth-limit", "If given, limits the number of times a region is refined.").setDefaultValueInteger(-1).build()).build());
                
                std::vector<std::string> engines = {"pl", "exactpl", "validatingpl"};
                this->addOption(storm::settings::OptionBuilder(moduleName, checkEngineOptionName, true, "Sets which engine is used for analyzing regions.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the engine to use.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(engines)).setDefaultValueString("pl").build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, printNoIllustrationOptionName, false, "If set, no illustration of the result is printed.").build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, printFullResultOptionName, false, "If set, the full result for every region is printed.").build());
            }
            
            bool RegionSettings::isRegionSet() const {
                return this->getOption(regionOptionName).getHasOptionBeenSet();
            }
            
            std::string RegionSettings::getRegionString() const {
                return this->getOption(regionOptionName).getArgumentByName("regioninput").getValueAsString();
            }
            
            bool RegionSettings::isHypothesisSet() const {
                return this->getOption(hypothesisOptionName).getHasOptionBeenSet();
            }
            
            storm::modelchecker::RegionResultHypothesis RegionSettings::getHypothesis() const {
                std::string hypString = this->getOption(hypothesisOptionName).getArgumentByName("hypothesis").getValueAsString();
                
                storm::modelchecker::RegionResultHypothesis result;
                
                if (hypString == "unknown") {
                    result = storm::modelchecker::RegionResultHypothesis::Unknown;
                } else if (hypString == "allsat") {
                    result = storm::modelchecker::RegionResultHypothesis::AllSat;
                } else if (hypString == "allviolated") {
                    result = storm::modelchecker::RegionResultHypothesis::AllViolated;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Hypothesis " << hypString << " not known.");
                }
                
                return result;
            }
            
            bool RegionSettings::isRefineSet() const {
                return this->getOption(refineOptionName).getHasOptionBeenSet();
            }
            
            double RegionSettings::getCoverageThreshold() const {
                return this->getOption(refineOptionName).getArgumentByName("coverage-threshold").getValueAsDouble();
            }
            
            bool RegionSettings::isDepthLimitSet() const {
                return this->getOption(refineOptionName).getArgumentByName("depth-limit").getHasBeenSet() && this->getOption(refineOptionName).getArgumentByName("depth-limit").getValueAsInteger() >= 0;
            }
            
            uint64_t RegionSettings::getDepthLimit() const {
                int64_t depth = this->getOption(refineOptionName).getArgumentByName("depth-limit").getValueAsInteger();
                STORM_LOG_THROW(depth >= 0, storm::exceptions::InvalidOperationException, "Tried to retrieve the depth limit but it was not set.");
                return (uint64_t) depth;
            }
            
            storm::modelchecker::RegionCheckEngine RegionSettings::getRegionCheckEngine() const {
                std::string engineString = this->getOption(checkEngineOptionName).getArgumentByName("name").getValueAsString();
                
                storm::modelchecker::RegionCheckEngine result;
                if (engineString == "pl") {
                    result = storm::modelchecker::RegionCheckEngine::ParameterLifting;
                } else if (engineString == "exactpl") {
                    result = storm::modelchecker::RegionCheckEngine::ExactParameterLifting;
                } else if (engineString == "validatingpl") {
                    result = storm::modelchecker::RegionCheckEngine::ValidatingParameterLifting;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown region check engine '" << engineString << "'.");
                }
                
                return result;
            }
            
            bool RegionSettings::isPrintNoIllustrationSet() const {
                return this->getOption(printNoIllustrationOptionName).getHasOptionBeenSet();
            }
            
            bool RegionSettings::isPrintFullResultSet() const {
                return this->getOption(printFullResultOptionName).getHasOptionBeenSet();
            }
            

        } // namespace modules
    } // namespace settings
} // namespace storm
