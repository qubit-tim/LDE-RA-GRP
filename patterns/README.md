About the pattern files...
* patterns2704.txt - contains all the patterns
* patterns928.txt - all of these patterns are contained in patterns2704.txt
* patterns785.txt - all of these patterns are contained in patterns785.txt

To verify the above statements use the following:
```
comm -13 <(sort patterns2704.txt) <(sort patterns928.txt)
# Expect to see no output here
comm -13 <(sort patterns928.txt) <(sort patterns785.txt)
# Expect to see no output here

#Other ways:
comm -1 <(sort patterns2704.txt) <(sort patterns928.txt) | wc -l
# Expect to see a 928 here
comm -1 <(sort patterns928.txt) <(sort patterns785.txt) | wc -l
# Expect to see a 785 here
```

Command Explanation: All of this is in the ```man``` page for ```comm``` but for simplicity, the ```-13``` instructs ```comm``` not to display any lines that are unique to the first file and not to display any lines that are in common.  The other ways does a verification by displaying lines that are unique to the second file and common between both and doing a line count; we expect this line count to match the number of patterns in the second file.

A bit more about the pattern files that still needs to be verified:
* patterns928.txt - This should be patterns2704.txt with all transpose duplicates removed (example - if M1^T == M2, then M2 is removed)
* patterns785.txt -  This should be patterns928.txt with all 2/3 swap duplicates removed (example - if you swap all 2s for 3s and 3s for 2s in M1 and M1^(2/3 swap) == M2, then M2 is removed)