#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//  TODO: - Add octave switching notation as an OOB option after transposition
//        - Remove case sensitivity in "Transpose" line detection

// Global variables
const std::string VP_CHARS = "1!2@34$5%6^78*9(0qQwWeErtTyYuiIoOpPasSdDfgGhHjJklLzZxcCvVbBnm";
const std::string OUTPUT_PATH = "output.txt";
int minKey = 0; // By index, not character
int maxKey = VP_CHARS.size()-1;
int oobHandling = 3;

// Functions
void readInt(int &var, int min, int max);
std::string transposeLine(int noteAmount, std::string *line);
void transposeFile(std::string *sheetPath, int amt);
void oobPrompt(std::string *path, int amt);
int sumOutOfBounds(); // Reads from OUTPUT_PATH
int sumAccidentals(); // ^

// ---------------- Main program ----------------
int main() {
    std::string sheetPath;

    // Prompt user
    int mainChoice;
    std::cout << "What would you like to do?\n"
              << "[1] Transpose VP sheet file\n"
              << "[2] Compress VP sheet to 3-octave range\n"
              << "Enter number: ";
    readInt(mainChoice, 1, 2);

    // ----- ESTABLISH INPUT FILE -----
    std::cout << "\nEnter file name/path: ";
    std::cin >> sheetPath;

    // Ensure file is not output file
    while(sheetPath == OUTPUT_PATH) {
        std::cout << "Input file should not match output path ('" << OUTPUT_PATH << "'). Please re-enter: ";
        std::cin >> sheetPath;
    }

    // Ensure file exists
    std::ifstream sheet(sheetPath);
    while(!sheet) {
        std::cout << "File not found. Please re-enter: ";
        std::cin >> sheetPath;

        sheet.open(sheetPath);
    }
    sheet.close();

    // ----- HANDLE USER CHOICE -----
    std::cout << '\n';
    switch(mainChoice) {
        case 1: {
            // -------------------------- Transpose VP sheet --------------------------

            int transpChoice;
            std::cout << "How would you like to transpose the piece?\n"
                      << "[1] Enter manual amount\n"
                      << "[2] Automatically find transposition with least accidentals/missing notes\n"
                      << "Enter number: ";
            readInt(transpChoice, 1, 2);

            switch(transpChoice) {
                // ----- Manual transposition
                case 1: {
                    std::cout << "\nEnter desired note movement (from -12 through 12): ";
                    int transposeAmt;
                    readInt(transposeAmt, -12, 12);

                    transposeFile(&sheetPath, transposeAmt);
                    oobPrompt(&sheetPath, transposeAmt);

                    break;
                }
                // ----- Automatic transposition
                case 2: {
                    std::vector<int> solutions;

                    int indexOfLowestCount;
                    int lowestCount = INT_MAX;
                    for(int i = -12; i <= 12; i++) {
                        transposeFile(&sheetPath, i);

                        int currentCount = sumOutOfBounds() + sumAccidentals();

                        if(currentCount < lowestCount) {
                            if(solutions.size() > 0) {
                                solutions.erase(solutions.begin()); // Erase previous solution, since new is lower
                            }

                            indexOfLowestCount = i;
                            lowestCount = currentCount;

                            solutions.push_back(i);
                        }
                        else if(currentCount == lowestCount) {
                            solutions.push_back(i); // Since count is equal, just append index
                        }
                    }

                    // Inform user of results if applicable
                    if(indexOfLowestCount != 0) {
                        std::cout << "\nThe following transpositions were found to have the least problematic notes (" << lowestCount << "):\n";
                        for(int i = 0; i < solutions.size(); i++) {
                            if(i < solutions.size()-1)
                                std::cout << solutions[i] << ", ";
                            else
                                std::cout << solutions[i] << ' ';
                        }
                        std::cout << '\n';
                    }
                    else {
                        std::cout << "\nThis sheet is already at the best transposition possible.\n";
                    }


                    // Transpose to best-ish config
                    transposeFile(&sheetPath, indexOfLowestCount);
                    oobPrompt(&sheetPath, indexOfLowestCount);
                }
            }


            break;
        }

        case 2: {
            // ---------------- Compress VP sheet to 3-octave range -------------------

            // Count the number of out-of-range notes
            int oobPerOctave[3] = {0, 0, 0};

            for(int i = 0; i < 3; i++) {
                minKey = 12*i;
                maxKey = minKey + 36;
                transposeFile(&sheetPath, 0);
                oobPerOctave[i] = sumOutOfBounds() + sumAccidentals();
            }

            int chosenRange = 0;
            std::cout << "Choose desired octave range:\n"
                      << "[1] (1-s) - " << oobPerOctave[0] << " notes lost\n"
                      << "[2] (8-l) - " << oobPerOctave[1] << " notes lost\n"
                      << "[3] (t-m) - " << oobPerOctave[2] << " notes lost\n"
                      << "Enter number: ";
            readInt(chosenRange, 1, 3);

            minKey = 12*(chosenRange-1);
            maxKey = minKey+36;

            transposeFile(&sheetPath, 0);
            oobPrompt(&sheetPath, 0);

            break;
        }
    }

    std::cout << "\nYour sheet has been written into a new file ("
              << OUTPUT_PATH
              << ") located in the same directory as this program. (The original file is untouched.)";

    return 0;
}

void readInt(int &var, int min, int max) {
    while(true) {
        std::cin >> var;

        if(std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Please enter a valid number: ";
            continue;
        }

        else if(var < min || var > max) {
            std::cout << "That number is outside of the given range. Please re-enter: ";
        }
        else {
            break;
        }
    }
}

// Given a string, transpose all possible characters by specified amount
std::string transposeLine(int noteAmount, std::string *line) {
    std::string result = "";

    // Ignore line if stating transposition
    if(line->find("Transp") != -1)
        return *line;

    // Iterate through given string
    for(int i = 0; i < line->size(); i++) {
        int indexInPiano = VP_CHARS.find((*line)[i]);

        // If the character is not on the piano, ignore it
        if(indexInPiano == -1) {
            result += (*line)[i];
            continue;
        }

        int newIndex = indexInPiano + noteAmount;
        // If transposed index is valid, add to index and reassign character
        if(newIndex >= minKey && newIndex <= maxKey) {
            result += VP_CHARS[newIndex];
        }
        // Out-of-bounds handling
        else {
            switch(oobHandling) {
                // Case 1 (remove out-of-bounds notes) is handled automatically.

                // 2: Change to nearest octave
                case 2: {
                    while(newIndex >= maxKey) {
                        newIndex -= 12;
                    }
                    while(newIndex < minKey) {
                        newIndex += 12;
                    }
                    result += VP_CHARS[newIndex];
                    break;
                }
                // 3: ? notation
                case 3: {
                    result += '?';
                    break;
                }
            }
        }
    }

    return result;
}

// Transpose all lines of a file
void transposeFile(std::string *sheetPath, int amt) {
    std::ifstream inputFile(*sheetPath);
    std::ofstream outputFile(OUTPUT_PATH);

    // Put transposition line before sheet
    if(amt != 0)
        outputFile << ((-amt > 0)? "+":"") << std::to_string(-amt) << " Transposition\n\n";

    // Transpose sheet line-by-line
    std::string sheetLine;
    while(getline(inputFile, sheetLine)) {
        outputFile << transposeLine(amt, &sheetLine) << '\n';
    }

    inputFile.close();
    outputFile.close();
}

// Sum the number of notes outside of minKey & maxKey range OR is notated with a '?'
int sumOutOfBounds() {
    std::ifstream file(OUTPUT_PATH);
    int sum = 0;

    std::string line;
    while(std::getline(file, line)) {
        for(int i = 0; i < line.size(); i++) {
            if(line[i] == '?')
                sum++;
            // Sum notes that are out of minKey/maxKey bounds
            else if(VP_CHARS.find(line[i]) != -1) {
                if(VP_CHARS.find(line[i]) < minKey || VP_CHARS.find(line[i]) > maxKey)
                    sum++;
            }
        }
    }

    file.close();
    return sum;
}

// Sum the number of accidental notes in a file
// Ignores lines if they contain "Transp", ex. lines like "+5 Transposition"
int sumAccidentals() {
    std::string accidentals = "!@$%^*(QWETYIOPSDGHJLZCVB";

    std::ifstream file(OUTPUT_PATH);
    int sum = 0;

    std::string line;
    while(std::getline(file, line)) {
        for(int i = 0; i < line.size(); i++) {
            if(accidentals.find(line[i]) != -1 && line.find("Transpos") == -1)
                sum++;
        }
    }

    file.close();
    return sum;
}

// Receive user preference for out-of-bounds handling
void oobPrompt(std::string *path, int amt) {
    if(sumOutOfBounds() > 0) {
        std::cout << "\nThis transposition places notes outside of the playable range.\n"
                  << "How would you like to handle this?\n"
                  << "[1] Remove them\n"
                  << "[2] Transpose them to the nearest octave\n"
                  << "[3] Indicate them with '?'\n"
                  << "Enter number: ";
        readInt(oobHandling, 1, 3);
    }

    if(oobHandling != 3)
        transposeFile(path, amt);
}
