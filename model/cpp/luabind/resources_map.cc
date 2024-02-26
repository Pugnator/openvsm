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
        //{"LuaUnit", {reinterpret_cast<const char *>(lua_LuaUnitData), lua_LuaUnitSize}},
};