#include <algorithm>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <map>
#include <regex>
#include <future>

#include "LDE-Matrix/pattern-matrix.hpp"
#include "LDE-Matrix/zmatrix.hpp"
#include "LDE-Matrix/data/patterns928.hpp"
#include "LDE-Matrix/pattern-deduper.hpp"

std::string TFC_OUT_DIR = "user-output";

std::map<int, std::vector<char>> caseSubcases = {
    {1, {'-'}},
    {2, {'-'}},
    {3, {'a', 'b', 'c'}},
    {4, {'a', 'b'}},
    {5, {'a', 'b'}},
    {6, {'a', 'b', 'c'}},
    {7, {'-'}},
    {8, {'a', 'b'}},
};

std::string matchedCasesDirectory = "tfc-output";
std::string matchedSubcasesDirectory = "tfc-output";
bool useNewEncoding = false;

std::string replace_all(
    const std::string & str ,   // where to work
    const std::string & find ,  // substitute 'find'
    const std::string & replace //      by 'replace'
) {
    using namespace std;
    string result;
    size_t find_len = find.size();
    size_t pos,from=0;
    while ( string::npos != ( pos=str.find(find,from) ) ) {
        result.append( str, from, pos-from );
        result.append( replace );
        from = pos + find_len;
    }
    result.append( str, from , string::npos );
    return result;
}

std::vector<patternMatrix> loadPatterns(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file:" << filename << std::endl;
        return {}; // Indicate error
    }
    std::vector<patternMatrix> patterns;
    std::string line;

    int lineNumber = 0;
    while (std::getline(file, line)) {
        // ignore comments
        if (line[0] == '#') {
            std::cout << "Ignoring comment: " << line << std::endl;
            continue;
        }
        patterns.push_back(patternMatrix(++lineNumber, line, useNewEncoding));
    }
    file.close();
    return patterns;
}

std::vector<patternMatrix> loadPatternsExtraBrackets(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file:" << filename << std::endl;
        return {}; // Indicate error
    }
    std::vector<patternMatrix> patterns;
    std::string line;

    int lineNumber = 0;
    while (std::getline(file, line)) {
        // ignore comments
        if (line[0] == '#') {
            std::cout << "Ignoring comment: " << line << std::endl;
            continue;
        }
        // Remove the leading and trailing brackets
        line = line.substr(1, line.size() - 2);
        // Replace spaces with commas
        std::replace(line.begin(), line.end(), ' ', ',');
        // Replace all instances of "],[" with "]["
        line = replace_all(line, "],[", "][");
        patterns.push_back(patternMatrix(++lineNumber, line, false));
        if (lineNumber % 1000 == 0) {
            std::cout << "Loaded " << lineNumber << " patterns" << std::endl;
        }
    }
    file.close();
    return patterns;
}

void case352AllPossible(std::string fileSuffix, bool optimized) {
    std::string tfcOutFile = TFC_OUT_DIR + "/tfc-output" + fileSuffix + ".txt";
    std::string tfcUniquesFile = TFC_OUT_DIR + "/tfc-uniques" + fileSuffix + ".txt";
    std::filesystem::create_directory(TFC_OUT_DIR);
    std::ofstream tfcout = std::ofstream(tfcOutFile);
    std::ofstream tfcUniques = std::ofstream(tfcUniquesFile);

    if (!tfcout.is_open()) {
        std::cerr << "Error opening file:" << tfcOutFile << std::endl;
        return;
    }

    if (!tfcUniques.is_open()) {
        std::cerr << "Error opening file:" << tfcUniquesFile << std::endl;
        return;
    }

    // do this on 702 xT12
    patternMatrix test = patternMatrix(3);
    test.multilineOutput = true;
    test.printDebugInfo = false;

    std::cout << test.id << std::endl;
    std::cout << "Before T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
    std::cout << test << std::endl;

    // p352 gates
    //test.rightTGateMultiply(1,4);
    //test.rightTGateMultiply(2,3);
    // p702 gate
    test.rightTGateMultiply(1,2);

    std::cout << "After T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
    std::cout << test << std::endl;

    // If you want to reduce and see the LDEs then see possible values, uncomment this block
    test.ldeReductionOnPattern(1);
    // p702 extra reduction
    test.ldeReductionOnPattern(0);
    std::cout << "LDEs:" << std::endl;
    test.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    test.printPossibleValues(std::cout);
    std::cout << "Max of possible values: " << test.getMaxOfPossibleValues() << std::endl;
    
    std::chrono::seconds dura(5);
    std::this_thread::sleep_for(dura);

    std::cout << "Starting to generate all possible patterns" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    // This generates all possible value patterns and stores them in the old encoding scheme
    if (optimized) {
        std::cout << "doing the optimized version" << std::endl;
        test.optimizedGenerateAllPossibleValuePatterns();
    } else {
        test.generateAllPossibleValuePatterns();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "Time to generate all possible patterns: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds" << std::endl;
    std::cout << "Time to generate 1 possible pattern: " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / test.allPossibleValuePatterns.size() << " microseconds" << std::endl;
    std::cout << "Number of possible value patterns: " << test.allPossibleValuePatterns.size() << std::endl;
    tfcout << "Time to generate all possible patterns: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds" << std::endl;
    tfcout << "Time to generate 1 possible pattern: " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / test.allPossibleValuePatterns.size() << " microseconds" << std::endl;
    tfcout << "Number of possible value patterns: " << test.allPossibleValuePatterns.size() << std::endl;
    tfcout << "Deduping:" << std::endl;
    patternDeduper pd = patternDeduper();
    int newPatternID = 3000000;
    std::map<int, int> dupCount;
    for (auto pm : test.allPossibleValuePatterns) {
        int duplicateID = -1;
        // By default, these are in the old encoding but this could change :(
        patternMatrix pmCopy = patternMatrix(++newPatternID, pm.first);
        if (pd.isDuplicate(pmCopy, duplicateID, true)) {
            std::cout << pmCopy.id << " is a duplicate of " << duplicateID << std::endl;
            tfcout << pmCopy.id << " is a duplicate of " << duplicateID << std::endl;
            dupCount[duplicateID]++;
        } else {
            std::cout << pmCopy.id << " is unique" << std::endl;
            tfcout << pmCopy.id << " is unique" << std::endl;
            tfcUniques << pmCopy << std::endl;
        }
    }
    tfcout << "Duplicate Counts:" << std::endl;
    tfcUniques << "Duplicate Counts:" << std::endl;
    for (auto const& [id, count] : dupCount) {
        std::cout << "Duplicate ID: " << id << " Count: " << count << std::endl;
        tfcout << "Duplicate ID: " << id << " Count: " << count << std::endl;
        tfcUniques << "Duplicate ID: " << id << " Count: " << count << std::endl;
    }
    tfcout.close();
    tfcUniques.close();
    std::cout << "Done" << std::endl;
}

void case352SubCaseMatching() {
    std::filesystem::create_directory(matchedSubcasesDirectory);
    std::map<int, std::string> patternFiles = {
        // {1, matchedCasesDirectory + "/case1-matches-patterns928.txt"},  // No subcases
        // {2, matchedCasesDirectory + "/case2-matches-patterns928.txt"},  // No subcases
        //{3, matchedCasesDirectory + "/case3-matches-patterns928.txt"},
        {3, matchedCasesDirectory + "/case3-patterns-tfc.txt"},
        //{4, matchedCasesDirectory + "/case4-matches-patterns928.txt"},
        //{5, matchedCasesDirectory + "/case5-matches-patterns928.txt"},
        //{6, matchedCasesDirectory + "/case6-matches-patterns928.txt"},
        // {7, matchedCasesDirectory + "/case7-matches-patterns928.txt"},  // No subcases
        //{8, matchedCasesDirectory + "/case8-matches-patterns928.txt"},
        };
    for (auto const& pair : patternFiles) {
        int caseNumber = pair.first;
        std::string patternFile = pair.second;
        std::vector<patternMatrix> patterns = loadPatterns(patternFile);
        std::vector<std::ofstream> matchedCasesFiles;
        std::vector<std::ofstream> matchedCasesFilesHumanReadable;
        matchedCasesFiles.push_back(std::ofstream(matchedSubcasesDirectory+ "/case" + std::to_string(caseNumber) + "-no-subcase-match.txt"));
        matchedCasesFilesHumanReadable.push_back(std::ofstream(matchedSubcasesDirectory+ "/case" + std::to_string(caseNumber) + "-no-subcase-matches-human-readable.txt"));
        for (char subcase : caseSubcases[caseNumber]) {
            std::string filename = matchedSubcasesDirectory+ "/case" + std::to_string(caseNumber) + subcase + "-matches.txt";
            matchedCasesFiles.push_back(std::ofstream(filename));
            if (!matchedCasesFiles[matchedCasesFiles.size()-1].is_open()) {
                std::cerr << "Error opening file:" << filename << std::endl;
            }
            matchedCasesFiles[matchedCasesFiles.size()-1] << "# Using the new encoding: 2y + x" << std::endl;

            std::string filenameHumanReadable = matchedSubcasesDirectory+ "/case" + std::to_string(caseNumber) + subcase + "-matches-human-readable.txt";
            matchedCasesFilesHumanReadable.push_back(std::ofstream(filenameHumanReadable));
            if (!matchedCasesFilesHumanReadable[matchedCasesFilesHumanReadable.size()-1].is_open()) {
                std::cerr << "Error opening file:" << filenameHumanReadable << std::endl;
            }
            matchedCasesFilesHumanReadable[matchedCasesFilesHumanReadable.size()-1] << "# Using the new encoding: 2y + x" << std::endl;
        }

        for (patternMatrix pm : patterns) {
            std::cout << "Pattern " << pm.id << std::endl;
            pm.singleCaseRearrangement = true;
            pm.rearrangeMatrix();
            patternMatrix pm2 = patternMatrix(pm.id, pm.getFirstCaseRearrangement());
            pm2.updatePairCounts();
            if(!pm2.matchesCase(caseNumber)) {
                std::cout << "Pattern " << pm2.id << " does not match case " << caseNumber << " and is in the wrong file" << std::endl;
                std::cout << pm2 << std::endl;
                continue;
            }
            pm2.caseMatch = caseNumber;
            pm2.printID = true;
            
            if(pm2.determineSubCase()) {
                int index = pm2.subCaseMatch - 'a' + 1;
                matchedCasesFiles[index] << pm2 << std::endl;
                pm2.multilineOutput = true;
                matchedCasesFilesHumanReadable[index] << pm2 << std::endl;
            } else {
                std::cout << "Pattern " << pm2.id << " has no subcase matches." << std::endl;
                matchedCasesFiles[0] << pm2 << std::endl;
                pm2.multilineOutput = true;
                matchedCasesFilesHumanReadable[0] << pm2 << std::endl;
            
            }
        }
        for (std::ofstream& matchedCasesFile : matchedCasesFiles) matchedCasesFile.close();
        for (std::ofstream& matchedCasesFile : matchedCasesFilesHumanReadable) matchedCasesFile.close();
    }
}

void test352Possible() {
    patternMatrix test = patternMatrix(67,"[1,1,1,1,0,0][1,1,3,3,0,0][0,0,0,0,2,2][0,0,0,0,2,2][1,3,1,3,0,0][1,3,3,1,0,0]", true);
    test.multilineOutput = true;

    test.rearrangeMatrix();
    patternMatrix test2 = patternMatrix(67, test.getFirstCaseRearrangement());
    test2.multilineOutput = true;

    test2.printPairCounts(std::cout);
    
    
    std::cout << "Before T-Gate multiplication " << test2.printTGateOperations() << ":" << std::endl;
    std::cout << test2 << std::endl;

    // Here, you can mulitply the pattern with T-Gates.  Feel free to update as desired.
    //  You can use the following functions to multiply the pattern with T-Gates:
    //    test.rightTGateMultiply(column1, column2) - Multiply the pattern with T-Gate on the right
    //    test.leftTGateMultiply(row1, row2) - Multiply the pattern with T-Gate on the left
    test2.rightTGateMultiply(1,4);
    test2.rightTGateMultiply(2,3);

    std::cout << "After T-Gate multiplication " << test2.printTGateOperations() << ":" << std::endl;
    std::cout << test2 << std::endl;

    // If you want to reduce and see the LDEs then see possible values, uncomment this block
    test2.ldeReductionOnPattern(1);
    std::cout << "LDEs:" << std::endl;
    test2.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    test2.printPossibleValues(std::cout);
    std::cout << "Max of possible values: " << test.getMaxOfPossibleValues() << std::endl;
}

void generateCaseSumMaps() {
    // int1 = case number, int2 = sum, int3 = id, string = pattern
    std::map <int, std::map <int, std::map <int, std::string>>> caseSumMap;
    for (int i = 1; i <=928; i++) {
        patternMatrix pm = patternMatrix(i);
        std::cout << pm.id << " " << pm << std::endl;
        pm.matchOnCases();
        caseSumMap[pm.caseMatch][pm.p.zSum][pm.id] = pm.toString();
    }
    
    std::filesystem::create_directory(TFC_OUT_DIR);
    std::ofstream tfcout = std::ofstream(TFC_OUT_DIR + "/case-sum-map.txt");
    if (!tfcout.is_open()) {
        std::cerr << "Error opening file:" << TFC_OUT_DIR + "/case-sum-map.txt" << std::endl;
        return;
    }
    for (auto const& [caseNumber, sumMap] : caseSumMap) {
        tfcout << "    {" << caseNumber << ", {" << std::endl;
        std::cout << "Case: " << caseNumber << std::endl;
        for (auto const& [sum, idMap] : sumMap) {
            tfcout << "        {" << sum << ", {" << std::endl;
            std::cout << "Sum: " << sum << std::endl;
            for (auto const& [id, pattern] : idMap) {
                tfcout << "            {" << id << ", \"" << pattern << "\"}," << std::endl;
            }
            tfcout << "        }}," << std::endl;
        }
        tfcout << "    }}," << std::endl;
    }
}

void outputCaseSumMap() {
    for (auto const& [caseNumber, sumMap] : CASE_SUM_MAP_PATTERNS_928) {
        std::cout << "Case: " << caseNumber << std::endl;
        for (auto const& [sum, idMap] : sumMap) {
            std::cout << "Sum: " << sum << std::endl;
            for (auto const& [id, pattern] : idMap) {
                std::cout << "ID: " << id << " Pattern: " << pattern << std::endl;
            }
        }
    }
}

void patternDeduperTest() {
    patternDeduper pd = patternDeduper();
    patternMatrix pm = patternMatrix(182);
    pm.multilineOutput = true;
    int duplicateID = 0;
    bool isDuplicate = pd.isDuplicate(pm, duplicateID, true);
    std::cout << "Is duplicate: " << isDuplicate << " Duplicate ID: " << duplicateID << std::endl;
}

void dedupeP352() {
    std::filesystem::create_directory(TFC_OUT_DIR);
    std::ofstream tfcout = std::ofstream(TFC_OUT_DIR + "/tfc-output.txt");
    std::ofstream tfcUniques = std::ofstream(TFC_OUT_DIR + "/tfc-uniques.txt");

    std::vector<patternMatrix> patterns = loadPatterns(TFC_OUT_DIR + "/p352-xT14-xT23-all-possible.txt");

    std::vector<patternMatrix> p928Patterns;
    for (int i = 0; i <= 928; i++) {
        if (i == 0) {
            p928Patterns.push_back(patternMatrix());
            continue;
        }
        p928Patterns.push_back(patternMatrix(i));
    }

    patternDeduper pd = patternDeduper();
    int newPatternID = 352000000;
    std::map<int, int> dupCount;
    for (auto pm : patterns) {
        int duplicateID = -1;
        pm.id = ++newPatternID;
        std::cout << "Pattern " << pm.id << std::endl;
        if (pd.isDuplicate(pm, duplicateID, true)) {
            std::cout << pm.id << " " << pm << " is a duplicate of " << duplicateID << " " << p928Patterns[duplicateID] << std::endl;
            tfcout << pm.id << " " << pm << " is a duplicate of " << duplicateID << " " << p928Patterns[duplicateID] << std::endl;
            dupCount[duplicateID]++;
        } else {
            std::cout << pm.id << " is unique" << std::endl;
            tfcout << pm.id << " is unique" << std::endl;
            tfcUniques << pm.id << " " << pm << std::endl;
        }
    }
    tfcout << "Duplicate Counts:" << std::endl;
    tfcUniques << "Duplicate Counts:" << std::endl;
    int totalDupes = 0;
    for (auto const& [id, count] : dupCount) {
        std::cout << "Duplicate ID: " << id << " Count: " << count << std::endl;
        tfcout << "Duplicate ID: " << id << " Count: " << count << std::endl;
        tfcUniques << "Duplicate ID: " << id << " Count: " << count << std::endl;
        totalDupes += count;
    }
    std::cout << "Total duplicates: " << totalDupes << std::endl;
    tfcout << "Total duplicates: " << totalDupes << std::endl;
    tfcUniques << "Total duplicates: " << totalDupes << std::endl;

    tfcout.close();
    tfcUniques.close();
    std::cout << "Done" << std::endl;
}

bool dedupTest(int caseNumber) {
    if (caseNumber < 1 || caseNumber > 8) {
        std::cerr << "Invalid case number" << std::endl;
        return false;
    }
    std::string caseString = "Case: " + std::to_string(caseNumber);
    std::filesystem::create_directory("temp");
    int newPatternID = 1000000 * caseNumber;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    std::cout << "Case " << caseNumber << std::endl;
    std::ofstream dedupeOut = std::ofstream("temp/case-" + std::to_string(caseNumber) + "-dedupe-out.txt");
    std::ofstream uniquesOut = std::ofstream("temp/case-" + std::to_string(caseNumber) + "-uniques-out.txt");
    if (!dedupeOut.is_open()) {
        std::cerr << "Error opening file:" << "temp/case-" + std::to_string(caseNumber) + "-dedupe-out.txt" << std::endl;
        return false;
    }
    if (!uniquesOut.is_open()) {
        std::cerr << "Error opening file:" << "temp/case-" + std::to_string(caseNumber) + "-uniques-out.txt" << std::endl;
        return false;
    }
    std::string filename = "temp/case-" + std::to_string(caseNumber) + ".txt";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file:" << filename << std::endl;
        return false;
    }
    std::ifstream lineCount(filename);
    int totalLines = std::count(std::istreambuf_iterator<char>(lineCount), std::istreambuf_iterator<char>(), '\n');
    std::cout << caseString << " - Total lines: " << totalLines << std::endl;
    std::cout << caseString << " - Files opened" << std::endl;
    std::cout << caseString << " - Iterating through patterns" << std::endl;
    lineCount.close();
    
    patternDeduper pd = patternDeduper();
    std::cout << caseString << " - Deduper Loaded" << std::endl;
    std::map<int, int> dupCount;
    std::cout << caseString << " - Starting dedupe" << std::endl;
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        // ignore comments
        if (line[0] == '#') {
            std::cout << "Ignoring comment: " << line << std::endl;
            continue;
        }
        //std::cout << caseString << " - Line " << lineNumber << " size: " << line.size() << std::endl;
        // Silly me for putting in the LDE1only after the pattern as this was causing the pattern loads to fail :(
        // [[3 3 2 2 0 0] [3 3 2 2 0 0] [2 2 1 1 0 1] [2 2 1 1 0 1] [1 1 1 1 0 0] [1 1 0 0 0 0]] LDE1only
        if (line.size() > 85) {
            std::cout << caseString << " - Line " << lineNumber << " is too long: " << line.size() << std::endl;
            line = line.substr(0, 85);
        }
        // Remove the leading and trailing brackets
        line = line.substr(1, line.size() - 2);
        // Replace spaces with commas
        std::replace(line.begin(), line.end(), ' ', ',');
        // Replace all instances of "],[" with "]["
        line = replace_all(line, "],[", "][");
        patternMatrix pm = patternMatrix(++lineNumber, line, false);
        //std::cout << pm.id << " " << pm << std::endl;
        pm.matchOnCases();
        // There might be some case 2 patterns that hit this as new rules have been added.
        if (pm.caseMatch != caseNumber) {
            // probably should kick this over to the error bucket
            std::cout << caseString << " - Pattern: " << pm.id << " Case Match: " << pm.caseMatch << " does not match: " << caseNumber << std::endl;
            continue;
        }
        int duplicateID = -1;
        if (pd.isDuplicate(pm, duplicateID, true)) {
            //std::cout << pm.id << " is a duplicate of " << duplicateID << std::endl;
            dupCount[duplicateID]++;
        } else {
            pm.id = ++newPatternID;
            //std::cout << pm.id << " is unique" << std::endl;
            uniquesOut << pm.id << " " << pm << std::endl;
            dedupeOut << pm.id << " " << pm << std::endl;
        }
        if (lineNumber % 10000 == 0) {
            std::cout << caseString << " - Deduped " << lineNumber << " patterns" << std::endl;
            std::cout << caseString << " - Deduping 1 pattern: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time).count() / lineNumber << " microseconds" << std::endl;
            std::cout << caseString << " - Percent done: " << floorf(lineNumber * 100) / totalLines << "%" << std::endl;
            std::cout << caseString << " - Unique count: " << pd.getUniqueCaseCount(caseNumber) << std::endl;
        }
    }
    file.close();
    std::cout << std::endl;
    std::cout << caseString << " - Duplicate Counts:" << std::endl;
    dedupeOut << "\nDuplicate Counts:" << std::endl;
    for (auto const& [id, count] : dupCount) {
        std::cout << caseString << " - Duplicate ID: " << id << " Count: " << count << std::endl;
        dedupeOut << "Duplicate ID: " << id << " Count: " << count << std::endl;
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << caseString << " - Time to dedupe: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds" << std::endl;
    std::cout << caseString << " - Time to dedupe 1 pattern: " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / lineNumber << " microseconds" << std::endl;
    dedupeOut << "Time to dedupe: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds" << std::endl;
    dedupeOut << "Time to dedupe 1 pattern: " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / lineNumber << " microseconds" << std::endl;
    dedupeOut.close();
    uniquesOut.close();
    return true;
}


void flowTesting() {
    patternMatrix test = patternMatrix(352);
    test.multilineOutput = true;
    test.printDebugInfo = true;
    patternMatrix test2 = patternMatrix(352);
    test2.multilineOutput = true;
    test2.printDebugInfo = true;

    //std::cout << "Before T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
    //std::cout << test << std::endl;

    //test.tGateAutoMultiply();
    test2.rightTGateMultiply(1,4);
    test2.rightTGateMultiply(2,3);

    //std::cout << "After T-Gate multiplication " << test.printTGateOperations() << ":" << std::endl;
    //std::cout << test << std::endl;

    test.ldeReductionOnPattern(1);
    test2.ldeReductionOnPattern(1);
    //std::cout << "LDEs:" << std::endl;
    //test.printLDEs(std::cout);
    std::cout << "Possible values:" << std::endl;
    std::cout << "Test 1 - Auto" << std::endl;
    test.printPossibleValues(std::cout);
    std::cout << "Test 2 - Manual" << std::endl;
    test2.printPossibleValues(std::cout);
    std::cout << "Max of possible values: " << std::endl;
    std::cout << test.getMaxOfPossibleValues() << std::endl;
    std::cout << test2.getMaxOfPossibleValues() << std::endl;
}

void subcaseMatchFiles() {
    std::filesystem::create_directory("matched-subcases");
    std::vector<std::vector<std::ofstream>> matchedCasesFiles;
    std::vector<std::vector<std::ofstream>> matchedCasesFilesHumanReadable;
    // Since cases are numbered starting at 1, it's safe to put the no-matches file at index 0 which then aligns the index with the case numbers
    matchedCasesFiles.resize(9);
    matchedCasesFiles[0].push_back(std::ofstream("matched-subcases/no-matches.txt"));
    matchedCasesFilesHumanReadable.resize(9);
    matchedCasesFilesHumanReadable[0].push_back(std::ofstream("matched-subcases/no-matches-human-readable.txt"));
    for (int i = 1; i <= 8; i++) {
        // Set up the no-matches file
        if (caseSubcases[i].size() > 1) {
            std::string filename = "matched-subcases/case" + std::to_string(i) + "-no-subcase-matches.txt";
            std::string filenameHumanReadable = "matched-subcases/case" + std::to_string(i) + "-no-subcase-matches-human-readable.txt";
            matchedCasesFiles[i].push_back(std::ofstream(filename));
            if (!matchedCasesFiles[i][0].is_open()) {
                std::cerr << "Error opening file:" << filename << std::endl;
            }
            matchedCasesFiles[i][0] << "# Using the new encoding: 2y + x" << std::endl;

            matchedCasesFilesHumanReadable[i].push_back(std::ofstream(filenameHumanReadable));
            if (!matchedCasesFilesHumanReadable[i][0].is_open()) {
                std::cerr << "Error opening file:" << filenameHumanReadable << std::endl;
            }
            matchedCasesFilesHumanReadable[i][0] << "# Using the new encoding: 2y + x" << std::endl;
        }
        // setup the subcase match files
        for(int j = 0; j < caseSubcases[i].size(); j++) {
            std::cout << "Case: " << i << " Subcase: " << caseSubcases[i][j] << std::endl;
            std::string filename = "matched-subcases/case" + std::to_string(i) + "-matches.txt";
            std::string filenameHumanReadable = "matched-subcases/case" + std::to_string(i) + "-matches-human-readable.txt";
            if (caseSubcases[i][j] != '-') {
                filename = "matched-subcases/case" + std::to_string(i) + caseSubcases[i][j] + "-matches.txt";
                filenameHumanReadable = "matched-subcases/case" + std::to_string(i) + caseSubcases[i][j] + "-matches-human-readable.txt";
            }
            matchedCasesFiles[i].push_back(std::ofstream(filename));
            if (!matchedCasesFiles[i][matchedCasesFiles[i].size()-1].is_open()) {
                std::cerr << "Error opening file:" << filename << std::endl;
            }
            matchedCasesFiles[i][matchedCasesFiles[i].size()-1] << "# Using the new encoding: 2y + x" << std::endl;

            matchedCasesFilesHumanReadable[i].push_back(std::ofstream(filenameHumanReadable));
            if (!matchedCasesFilesHumanReadable[i][matchedCasesFilesHumanReadable[i].size()-1].is_open()) {
                std::cerr << "Error opening file:" << filenameHumanReadable << std::endl;
            }
            matchedCasesFilesHumanReadable[i][matchedCasesFilesHumanReadable[i].size()-1] << "# Using the new encoding: 2y + x" << std::endl;
        }
    }
    for (int i=1; i <= 928; i++) {
        //if (i == 240) continue;
        std::cout << "Pattern " << i << std::endl;
        patternMatrix pm = patternMatrix(i);
        pm.printID = true;
        pm.determineSubCase();
        int subCaseIndex = pm.subCaseMatch - 'a' + 1;
        if (pm.subCaseMatch == '-') {
            subCaseIndex = 0;
        }
        int caseNumber = pm.caseMatch;
        if (caseNumber < 0) caseNumber = 0;
        matchedCasesFiles[caseNumber][subCaseIndex] << pm << std::endl;
        pm.multilineOutput = true;
        matchedCasesFilesHumanReadable[caseNumber][subCaseIndex] << pm << std::endl;
    }
    for (int i = 1; i <= 8; i++) {
        for(int j = 0; j < matchedCasesFiles[i].size(); j++) {
            matchedCasesFiles[i][j].close();
            matchedCasesFilesHumanReadable[i][j].close();
        }
    }
}

int main(int argc, char **argv) {
    // NEXT UP -> Need to take any pattern, apply the T-Gates, and then dedupe it to form the overall map
    //dedupeP352();

    //flowTesting();
    //case3b testing w/ optimal selection() -> case 2;
    //update the subcase listings in matched-subcases;

    case352AllPossible("-3-optimized-02-ortho", true);

    /*
    std::vector<std::future<void>> futures;
    auto res1 = std::async(std::launch::async, case352AllPossible, "-352-optimized-O3", true);
    futures.push_back(std::move(res1));
    std::chrono::seconds dura(15);
    std::this_thread::sleep_for(dura);
    auto res2 = std::async(std::launch::async, case352AllPossible, "-746-standard-O3", false);
    futures.push_back(std::move(res2));
    for (auto &f : futures) {
        f.wait();
    }
    */

    // if a multi-threaded version doesn't work
    //case352AllPossible("-352-optimized", true);
    //case352AllPossible("-352-standard", false);
    
    // subcaseMatchFiles();
    
    /*
    // argv version for a per case run
    if (argc < 2) {
        std::cerr << "Usage: tfc-testing <case#>" << std::endl;
        return 1;
    }
    int caseNumber = std::stoi(argv[1]);
    auto start_time = std::chrono::high_resolution_clock::now();
    bool result = dedupTest(caseNumber);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "Done!" << std::endl;
    std::cout << "Time to dedupe: " << std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count() << " seconds" << std::endl;
    std::cout << "Results are:\n";
    std::cout << "Case " << std::to_string(caseNumber)  << "Result: " << result << std::endl;
    */
    /*
    // trying a multi-threaded version
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<std::future<bool>> futures;
     for (int i = 1; i <= 8; i++) {
        auto res = std::async(std::launch::async, dedupTest, i);
        futures.push_back(std::move(res));
    }
    for (auto &f : futures) {
        f.wait();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "Done!" << std::endl;
    std::cout << "Time to dedupe: " << std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count() << " seconds" << std::endl;
    std::cout << "Results are:\n";
    int caseNumber = 1;
    for (auto &f : futures) {
        bool result = f.get();
        std::cout << "Case " << std::to_string(caseNumber)  << "Result: " << result << std::endl;
        caseNumber++;
    }
    */
    return 0;
}