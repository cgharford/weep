For grading, you can run 'weep' with this command:
  $ ./weep practice.cap

Here's my basic thoughts and algorithm about the searching algorithm.
For ( n = 0; n = 0 < 255; n += interval) {
  ​1. Take the first n columns and use N-Queen Algorithm to generate all possible combinations (with consideration of interval).
  ​2. Eliminate the combinations that have been tested before.
  3. Test the new combination.
  4. Record the tested combination
}  

But I found that recording and eliminating the tested combinations actually took longer. I think the reason is that checking whether the key has been tested before might take up to O(n^2) time which significantlly slow down my algorithm.

Just as I talk you during Monday's office hour. I understand that the Union doesn't work but I couldn't come up with a way to find out which keys have been tested without recording down previously tested keys. 

I tried to proceed the search by interval, which will save me some time and always look for the most possible combinations.




To re-compile the program and time it, run the following program:
$ bash auto.sh
The output is pasted as follow:

--- Executing weep. Trying practice.cap ---
Storing IV and Keystream.
Processed 30792 packets.
Printing the table!!!!
1F (155)  4E (149)  9D (146)  73 (145)  36 (145)  5C (145)  14 (145)  C3 (145)  
53 (152)  AE (151)  54 (149)  FF (147)  47 (145)  65 (142)  46 (141)  A3 (141)  
5D (183)  6E (151)  81 (148)  79 (145)  38 (143)  03 (142)  A9 (142)  AD (142)  
7C (160)  D8 (151)  72 (149)  5C (148)  BB (147)  4F (145)  66 (140)  1B (139)  
9B (154)  FB (147)  6F (144)  13 (143)  FD (142)  F6 (142)  83 (141)  2E (141)  
Looking at total of 0 columns
Generated new candidates #0
Looking at total of 8 columns
Generated new candidates #32768
Looking at total of 16 columns
Generated new candidates #1048576
Found the key here: 0x1f1f1f1f1f !!!
The correct key is: 1f1f1f1f1f
Success!

real  0m0.303s
user  0m0.303s
sys 0m0.000s
--- Execution ended ---
--- Executing weep. Trying cs-test-wep.cap ---
Storing IV and Keystream.
Processed 84800 packets.
Printing the table!!!!
93 (133)  ED (125)  66 (124)  5E (120)  B8 (118)  6E (117)  23 (117)  BF (115)  
A2 (125)  5B (122)  B4 (120)  CE (116)  22 (116)  28 (115)  1A (115)  2A (114)  
C2 (155)  4D (122)  CD (121)  73 (119)  EB (118)  FF (117)  71 (117)  F4 (116)  
F3 (144)  82 (127)  8D (119)  9B (117)  E7 (116)  8F (116)  61 (115)  6B (115)  
6D (120)  C0 (117)  92 (116)  03 (115)  58 (115)  69 (115)  8B (114)  21 (114)  
Looking at total of 0 columns
Generated new candidates #0
Looking at total of 8 columns
Generated new candidates #32768
Looking at total of 16 columns
Generated new candidates #1048576
Found the key here: 0x938fa0c08f !!!
The correct key is: 938fa0c08f
Success!

real  0m0.207s
user  0m0.202s
sys 0m0.004s
--- Execution ended ---

----------------------- Auto run file End -----------------------
