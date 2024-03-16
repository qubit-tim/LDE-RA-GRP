# LDE-RA-GRP
LDE Reduction Algorithms-GRP

Code written to take pattern files, match them to cases, and then work through the transitions from case 8b -> 6 -> 3 -> [ ].  It also contains other random things associated with this work.

# How to run the code and what the run does
**Note - all of these are in the makefile**
* case-matching - Loads cases from ```cases/``` and matches patterns against those cases.  Outputs matches into ```matched-cases/``` by the case matched and the pattern file it was loaded from.
  * make case-matching && ./case-matching.out
* dupe-checks - Loads patterns from ```patterns/``` and dedups them against the other files.  ```patterns928.txt``` is considered the primary list.
  * make dupe-checks && ./dupe-checks.out
* convert-patterns - Converts patterns from "X X" format to numerical (0-3) format.
  * make convert-patterns && ./convert-patterns.out
* **Deprecated** pattern-file-checks - This was used to do a basic check of pattern duplicates between files but this is just as easily done with cmd line operations.  Also, this is handled with dupe-checks.  
  * make pattern-file-checks && ./pattern-file-checks.out
* case-8b - Loads up the ```matches/case8-matches-patterns928.txt``` file and performs case 8b matches by rearranging the matrices to match 8b then outputs the rearranged patterns in both a short and detailed form.
  * make case-8b && ./case-8b.out
* case-3 - Loads up the ```matches/case3-matches-patterns928.txt``` file and rearranges the cases to match the case 3 variants on page 12 of the paper.  Then it tries to perform multiplcation to get to another case.  As of 16 Mar 2024, the multiplication isn't fully working as sqrt(2) removal needs to be handled.
  * make case-3 && ./case-3.out
* case-traversal - WIP
* ztest - A quick, incomplete, test of the zmatrix class
  * make ztest && ./ztest.out

# Comments
28 Feb 2024 - This code is a bit messy and could use some clean up / refactoring.  I'd also like to have this as a pipeline where a pattern can go through all the steps in one go.  Perhaps one day when I have more time.

16 Mar 2024 - The case matching should be moved into the pattern matrix class along with the concept of case traversal.  Oh, and adding tests would be nice.
