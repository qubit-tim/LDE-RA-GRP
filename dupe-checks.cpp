#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>

#include "pattern-matrix.hpp"
#include "case-matrix.hpp"

std::vector<patternMatrix> loadPatterns(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return {}; // Indicate error
    }
    std::vector<patternMatrix> patterns;
    std::string line;

    int patternNumber = 1;
    while (std::getline(file, line)) {
        patterns.push_back(patternMatrix(patternNumber++, line));
    }
    file.close();
    return patterns;
}

bool comparePatternIDs(patternMatrix a, patternMatrix b) {
    return a.id < b.id;
}

std::vector<patternMatrix> removeTransposeDupes(std::string patternSet, std::vector<patternMatrix> patterns) {
    std::vector<patternMatrix> noDupes;
    std::map<int, std::vector<patternMatrix>> patternBuckets;
    // Sort patterns into buckets based on the sum of the pattern matrix
    for (patternMatrix pm : patterns) {
        patternBuckets[pm.p.zSum].push_back(pm);
    }
    // Remove patterns that are duplicates of a transpose of another pattern
    std::ofstream ptRemovals("deduping/" + patternSet + "-s1-r1-T-dupes.txt");
    std::ofstream ptRemovalsDetailed("deduping/" + patternSet + "-s1-r1-T-dupes-detailed.txt");
    for (auto& [psum, bucket] : patternBuckets) {
        //std::cout << "Pattern Sum for this Bucket: " << psum << std::endl;
        //std::cout << "Number of Patterns in this Bucket: " << bucket.size() << std::endl;
        // Skip bucket with only one pattern
        if (bucket.size() < 2) {
            continue;
        }
        // Compare each pattern to each other pattern in the bucket
        for (int i = 0; i < bucket.size(); ++i) {
            for (int j = i + 1; j < bucket.size(); ++j) {
                // Don't compare to yourself
                if (bucket[i].id == bucket[j].id) {
                    //std::cout << "SKIP - Attempted to compare transpose of pattern " << bucket[i].id << " to itself" << std::endl;
                    continue;
                }
                if (bucket[i].isTranspose(bucket[j])) {
                    ptRemovalsDetailed << bucket[i] << "; Transposed = " << bucket[i].pT << "; Matches = " << bucket[j] << std::endl;
                    ptRemovals << bucket[j] << std::endl;
                    bucket.erase(bucket.begin() + j);
                    --j;
                }
            }
        }
    }
    ptRemovals.close();
    ptRemovalsDetailed.close();
    
    for (auto& [psum, bucket] : patternBuckets) {
        for (patternMatrix pm : bucket) {
            noDupes.push_back(pm);
        }
    }
    // Sort the patterns by ID for file output purposes
    std::sort(noDupes.begin(), noDupes.end(), comparePatternIDs);

    // Write out the patterns with no duplicates
    std::ofstream ptNoDupesWithID("deduping/" + patternSet + "-s1-g1-no-T-dupes-with-id.txt");
    std::ofstream ptNoDupes("deduping/" + patternSet + "-s1-g1-no-T-dupes.txt");
    for(patternMatrix pm : noDupes) {
        ptNoDupesWithID << pm << std::endl;
        ptNoDupes << pm.p << std::endl;
    }
    ptNoDupesWithID.close();
    ptNoDupes.close();
    return noDupes;
}

std::vector<patternMatrix> remove23Swaps(std::string patternSet, std::vector<patternMatrix> patterns) {
    std::vector<patternMatrix> no23Swaps = patterns;
    std::map<int, std::vector<patternMatrix>> patternBuckets;
    // Sort patterns into buckets based on the sum of the pattern matrix
    for (patternMatrix pm : patterns) {
        patternBuckets[pm.p.zSum].push_back(pm);
    }
    // Remove patterns that are duplicates of a 2/3 swap of another pattern
    std::ofstream swap23Removals("deduping/" + patternSet + "-s2-r2-23-swap-dupes.txt");
    std::ofstream swap23RemovalsDetailed("deduping/" + patternSet + "-s2-r2-23-swap-dupes-detailed.txt");
    for (int i = no23Swaps.size() - 1; i >= 0; --i) {
        //std::cout << "Checking pattern " << no23Swaps[i].id << std::endl;
        for (patternMatrix pm : patternBuckets[no23Swaps[i].swap23.zSum]) {
            // Don't compare to yourself
            if (no23Swaps[i].id == pm.id) {
                //std::cout << "SKIP - Attempted to compare 2/3 swap of pattern " << no23Swaps[i].id << " to itself" << std::endl;
                continue;
            }
            if (pm.is23Swap(no23Swaps[i])) {
                swap23RemovalsDetailed << pm << "; 2/3 Swapped = " << pm.swap23 << "; Matches = " << no23Swaps[i] << std::endl;
                swap23Removals << no23Swaps[i] << std::endl;
                no23Swaps.erase(no23Swaps.begin() + i);
                // --i;
                break;
            }
        }
    }
    swap23Removals.close();
    swap23RemovalsDetailed.close();
    // Sort the patterns by ID for file output purposes
    std::sort(no23Swaps.begin(), no23Swaps.end(), comparePatternIDs);

    // Write out the patterns with no duplicates
    std::ofstream ptNoDupesWithID("deduping/" + patternSet + "-s2-g2-no-23-swap-dupes-with-id.txt");
    std::ofstream ptNoDupes("deduping/" + patternSet + "-s2-g2-no-23-swap-dupes.txt");
    for(patternMatrix pm : no23Swaps) {
        ptNoDupesWithID << pm << std::endl;
        ptNoDupes << pm.p << std::endl;
    }
    ptNoDupesWithID.close();
    ptNoDupes.close();
    return no23Swaps;
}

// This could be part of the remove23Swaps function, but it's separated so we can track removals per type of dupe
std::vector<patternMatrix> remove23SwapTransposes(std::string patternSet, std::vector<patternMatrix> patterns) {
    std::vector<patternMatrix> no23SwapsTransposes = patterns;
    std::map<int, std::vector<patternMatrix>> patternBuckets;
    // Sort patterns into buckets based on the sum of the pattern matrix
    for (patternMatrix pm : patterns) {
        patternBuckets[pm.p.zSum].push_back(pm);
    }
    // Remove patterns that are duplicates of a 2/3 swap of another pattern
    std::ofstream swap23TRemovals("deduping/" + patternSet + "-s3-r3-23-swap-T-dupes.txt");
    std::ofstream swap23TRemovalsDetailed("deduping/" + patternSet + "-s3-r3-23-swap-T-dupes-detailed.txt");
    for (int i = no23SwapsTransposes.size() - 1; i >= 0; --i) {
        //std::cout << "Checking pattern " << no23SwapsTransposes[i].id << std::endl;
        for (patternMatrix pm : patternBuckets[no23SwapsTransposes[i].swap23.zSum]) {
            // Don't compare to yourself
            if (no23SwapsTransposes[i].id == pm.id) {
                //std::cout << "SKIP - Attempted to compare 2/3 swap transpose of pattern " << no23SwapsTransposes[i].id << " to itself" << std::endl;
                continue;
            }
            if (pm.is23SwapT(no23SwapsTransposes[i])) {
                swap23TRemovalsDetailed << pm << "; 2/3 Swapped Transposed = " << pm.swap23T << "; Matches = " << no23SwapsTransposes[i] << std::endl;
                swap23TRemovals << pm << std::endl;
                no23SwapsTransposes.erase(no23SwapsTransposes.begin() + i);
                break;
            }
        }
    }
    swap23TRemovals.close();
    swap23TRemovalsDetailed.close();
    // Sort the patterns by ID for file output purposes
    std::sort(no23SwapsTransposes.begin(), no23SwapsTransposes.end(), comparePatternIDs);

    // Write out the patterns with no duplicates
    std::ofstream ptNoDupesWithID("deduping/" + patternSet + "-s3-g3-no-23-swap-T-dupes-with-id.txt");
    std::ofstream ptNoDupes("deduping/" + patternSet + "-s3-g3-no-23-swap-T-dupes.txt");
    for(patternMatrix pm : no23SwapsTransposes) {
        ptNoDupesWithID << pm << std::endl;
        ptNoDupes << pm.p << std::endl;
    }
    ptNoDupesWithID.close();
    ptNoDupes.close();
    return no23SwapsTransposes;
}

int main(int argc, char **argv) {
    // std::string patternFile = "patterns2704";
    // Since we didn't get what was expected from the 2704 pattern set, we'll try them all
    //std::vector<std::string> patternFiles = {"patterns785", "patterns928", "patterns2704"};
    std::vector<std::string> patternFiles = {"patterns2704"};
    for (std::string patternFile : patternFiles) {
        std::cout << "==== Start processing " << patternFile << " ====" << std::endl;
        std::vector<patternMatrix> patterns = loadPatterns("patterns/" + patternFile + ".txt");
        std::ofstream step0("deduping/" + patternFile + "-s0-g0-original-patterns.txt");
        for (patternMatrix pm : patterns) {
            pm.printAllIDs = true;  // This could be done while loading the patterns, but we'll do it here
            step0 << pm.p << std::endl;
        }
        step0.close();
        std::vector<patternMatrix> transposesRemoved = removeTransposeDupes(patternFile, patterns);
        std::vector<patternMatrix> no23swaps = remove23Swaps(patternFile, transposesRemoved);
        std::vector<patternMatrix> no23swapTransposes = remove23SwapTransposes(patternFile, no23swaps);
        std::cout << "Findings for " << patternFile << ":" << std::endl;
        std::cout << "Started with " << patterns.size() << " patterns" << std::endl;
        std::cout << "Removed " << patterns.size() - transposesRemoved.size() << " transpose duplicates" << std::endl;
        std::cout << "Removed " << transposesRemoved.size() - no23swaps.size() << " 2/3 swap duplicates" << std::endl;
        std::cout << "Removed " << no23swaps.size() - no23swapTransposes.size() << " 2/3 swap transpose duplicates" << std::endl;
        std::cout << "Ended with " << no23swapTransposes.size() << " patterns" << std::endl;
        std::cout << "==== End processing " << patternFile << " ====" << std::endl;
    }
    return 0;
}
