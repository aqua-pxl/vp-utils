# About
This is a tool for doing various kinds of modifications to Virtual Piano sheets.

# Features
- **Transposing** text sheets
- Automatically finding the **easiest transposition to play** (least sharps/flats/missing notes)
- Compressing a sheet to only three octaves (made for **FFXIV** bards)
- Handling out-of-bounds characters after transposition

> [!warning]
> - This is NOT a MIDI to VP conversion tool. I recommend looking elsewhere for that.

# Sample Runs
**Automatic transposition:**
```
What would you like to do?
[1] Transpose VP sheet file
[2] Compress VP sheet to 3-octave range
Enter number: 1

Enter file name/path: sheet.txt

How would you like to transpose the piece?
[1] Enter manual amount
[2] Automatically find transposition with least accidentals/missing notes
Enter number: 2

The following transpositions were found to have the least problematic notes (14):
-3

Your sheet has been written into a new file (output.txt) located in the same directory as this program. (The original file is untouched.)
```

**Octave compression:**
```
What would you like to do?
[1] Transpose VP sheet file
[2] Compress VP sheet to 3-octave range
Enter number: 2

Enter file name/path: sample.txt

Choose desired octave range:
[1] (1-s) - 110 notes lost
[2] (8-l) - 51 notes lost
[3] (t-m) - 192 notes lost
Enter number: 2

This transposition places notes outside of the playable range.
How would you like to handle this?
[1] Remove them
[2] Transpose them to the nearest octave
[3] Indicate them with '?'
Enter number: 2

Your sheet has been written into a new file (output.txt) located in the same directory as this program. (The original file is untouched.)
```

