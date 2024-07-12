# LDE-RA-GRP
LDE Reduction Algorithms-GRP

Code written to take pattern files, match them to cases, and then work through the transitions from case.  It also contains other random things associated with this work.

# Running the code
## Requirements
 * C++
 * Bazel - https://bazel.build/install
 * [Optional] - Buildifier

## Run Commands
 * On MacOS - `bazel run lde-main`
 * On Windows - `bazel run lde-main`

After executing the above command, the output will be in the `bazel-LDE-RA-GRP` directory.  

If you want the output to land directly into the workspace do the following:
 1. `bazel build lde-main`
 1. `bazel-bin/lde-main`

### Notes on changing input files
`main.cpp` defaults to using the `pattern928` file in the `patterns` directory.  To change to another set of patterns in the same directory, change the ` std::vector<std::string> patternFiles` to match the desired file.  At some point in the future input / output will be made more user friendly.

## Run Tests
 * `bazel test [Test Pattern]`
   * Test Pattern should be replaced with the test desired, like `//LDE-Matrix:pattern-matrix_test` or `//...` for all tests

# Comments
28 Feb 2024 - This code is a bit messy and could use some clean up / refactoring.  I'd also like to have this as a pipeline where a pattern can go through all the steps in one go.  Perhaps one day when I have more time.

16 Mar 2024 - The case matching should be moved into the pattern matrix class along with the concept of case traversal.  Oh, and adding tests would be nice.

14 Apr 2024 - Need to add some verbiage for Bazel and running tests.  Will do that after more tests are done.  Also things could be moved around and cleaned up to adhere to https://google.github.io/styleguide/cppguide.html.

29 Jun 2024 - Tests cover most of what is needed but there are a few items that could be improved with regards to rearrangements and orthonormality.  Orthonormality still needs to be verified and then output cleaned up in both main.cpp and pattern-matrix.cpp so that output goes to a consistent location.  Finally, the pattern generator is still in the works and will need more threads to be viable but is also waiting on orthonormality.
