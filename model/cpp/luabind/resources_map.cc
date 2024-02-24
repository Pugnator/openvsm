#include "module_config.hpp"
#define INCBIN_PREFIX lua_
#include "incbin.h"

INCBIN(LuaUnit, "wakadll/lua/luaunit/luaunit.luac");
INCBIN(TableHelpers, "wakadll/lua/training/helpers.luac");

INCBIN(DictManager, "wakadll/lua/dictionary/manager.luac");
INCBIN(SearchRequest, "wakadll/lua/dictionary/requests.luac");
INCBIN(UtfChars, "wakadll/lua/morphology/utf_chars.luac");

INCBIN(Inflector, "wakadll/lua/inflection/main.luac");
INCBIN(IchidanInflector, "wakadll/lua/inflection/ichidan.luac");
INCBIN(GodanInflector, "wakadll/lua/inflection/godan.luac");
INCBIN(IrregularInflector, "wakadll/lua/inflection/irregular.luac");
INCBIN(IAdjectiveInflector, "wakadll/lua/inflection/adjective_i.luac");
INCBIN(NaAdjectiveInflector, "wakadll/lua/inflection/adjective_na.luac");
INCBIN(Vowels, "wakadll/lua/inflection/vowels.luac");

INCBIN(TrainingTranslation, "wakadll/lua/training/training_translation.luac");
INCBIN(TrainingReading, "wakadll/lua/training/training_reading.luac");
INCBIN(TrainingKana, "wakadll/lua/training/training_kana.luac");
INCBIN(TrainingExamples, "wakadll/lua/training/training_examples.luac");
INCBIN(TrainingNumeral, "wakadll/lua/training/training_numeral.luac");
INCBIN(JlptTraining, "wakadll/lua/training/main_logic.luac");

INCBIN(Lexicon, "wakadll/lua/lexicon/main.luac");
INCBIN(LexiconPOS, "wakadll/lua/lexicon/word_pos.luac");
INCBIN(LexiconNumerals, "wakadll/lua/lexicon/numerals.luac");

const std::vector<std::string> modulesToPreload =
    {
        "LuaUnit",
        "DictManager",
        "UtfChars",
        "SearchRequest",
        "Inflector",
        "IchidanInflector",
        "GodanInflector",
        "IrregularInflector",
        "IAdjectiveInflector",
        "NaAdjectiveInflector",
        "Vowels",
        "JlptTraining",
        "TrainingExamples",
        "TrainingTranslation",
        "TrainingReading",
        "TrainingKana",
        "TrainingNumeral",
        "TableHelpers",
        "Lexicon",
        "LexiconPOS",
        "LexiconNumerals",
};

const std::map<std::string, Resource> resourceMap =
    {
        {"LuaUnit", {reinterpret_cast<const char *>(lua_LuaUnitData), lua_LuaUnitSize}},
        {"UtfChars", {reinterpret_cast<const char *>(lua_UtfCharsData), lua_UtfCharsSize}},
        {"DictManager", {reinterpret_cast<const char *>(lua_DictManagerData), lua_DictManagerSize}},
        {"SearchRequest", {reinterpret_cast<const char *>(lua_SearchRequestData), lua_SearchRequestSize}},
        {"Inflector", {reinterpret_cast<const char *>(lua_InflectorData), lua_InflectorSize}},
        {"IchidanInflector", {reinterpret_cast<const char *>(lua_IchidanInflectorData), lua_IchidanInflectorSize}},
        {"GodanInflector", {reinterpret_cast<const char *>(lua_GodanInflectorData), lua_GodanInflectorSize}},
        {"IrregularInflector", {reinterpret_cast<const char *>(lua_IrregularInflectorData), lua_IrregularInflectorSize}},
        {"IAdjectiveInflector", {reinterpret_cast<const char *>(lua_IAdjectiveInflectorData), lua_IAdjectiveInflectorSize}},
        {"NaAdjectiveInflector", {reinterpret_cast<const char *>(lua_NaAdjectiveInflectorData), lua_NaAdjectiveInflectorSize}},
        {"Vowels", {reinterpret_cast<const char *>(lua_VowelsData), lua_VowelsSize}},
        {"JlptTraining", {reinterpret_cast<const char *>(lua_JlptTrainingData), lua_JlptTrainingSize}},
        {"TableHelpers", {reinterpret_cast<const char *>(lua_TableHelpersData), lua_TableHelpersSize}},
        {"TrainingTranslation", {reinterpret_cast<const char *>(lua_TrainingTranslationData), lua_TrainingTranslationSize}},
        {"TrainingReading", {reinterpret_cast<const char *>(lua_TrainingReadingData), lua_TrainingReadingSize}},
        {"TrainingKana", {reinterpret_cast<const char *>(lua_TrainingKanaData), lua_TrainingKanaSize}},
        {"TrainingExamples", {reinterpret_cast<const char *>(lua_TrainingExamplesData), lua_TrainingExamplesSize}},
        {"TrainingNumeral", {reinterpret_cast<const char *>(lua_TrainingNumeralData), lua_TrainingNumeralSize}},
        {"Lexicon", {reinterpret_cast<const char *>(lua_LexiconData), lua_LexiconSize}},
        {"LexiconPOS", {reinterpret_cast<const char *>(lua_LexiconPOSData), lua_LexiconPOSSize}},
        {"LexiconNumerals", {reinterpret_cast<const char *>(lua_LexiconNumeralsData), lua_LexiconNumeralsSize}},

};