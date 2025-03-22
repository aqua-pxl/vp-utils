// --------- HTML ELEMENTS --------- //
// Text areas
const inputField = document.getElementById("input");
const outputField = document.getElementById("output");

// Buttons
const transpBtn = document.getElementById("transpose");
const transpNum = document.getElementById("transp-number");
const autoTranspBtn = document.getElementById("autotranspose");

// Auto-transposition result display
const autotransResult = document.getElementById("autotrans-result-div"); // currently 'display: none;'
const autotransNum = document.getElementById("autotrans-result");

// Octave compression buttons
const compressButtons = [
    document.getElementById("compress1s"),
    document.getElementById("compress8l"),
    document.getElementById("compresstm")
];

// Out-of-bounds setting
const oobSelect = document.getElementById("oobSelect");

// --------------------------------- //

const VP_CHARS = "1!2@34$5%6^78*9(0qQwWeErtTyYuiIoOpPasSdDfgGhHjJklLzZxcCvVbBnm";
const ACCIDENTALS = /[!@$%^*(QWETYIOPSDGHJLZCVB]/g;

// Track how out-of-bounds notes should be handled
let oobHandling = oobSelect.value;
function updateOobHandling() {
    oobHandling = oobSelect.value;
}

// Transpose given string by noteAmount;
// minKey and maxKey are indices for VP_CHARS
function transposeString(str, noteAmount, minKey = 0, maxKey = VP_CHARS.length - 1) {
    let result = "";
    inputStr = str.split(/\r?\n/)

    // Iterate through lines
    for (let lineIndex = 0; lineIndex < inputStr.length; lineIndex++) {
        // Iterate through characters
        for (let charIndex = 0; charIndex < inputStr[lineIndex].length; charIndex++) {
            // Find position of key on piano keyboard
            let indexInPiano = VP_CHARS.indexOf(inputStr[lineIndex][charIndex])

            // If the character is not in VP_CHARS, skip and add to result
            if (indexInPiano == -1) {
                result += inputStr[lineIndex][charIndex];
                continue;
            }

            // If transposed index is valid, add to index and reassign character
            let newIndex = indexInPiano + parseInt(noteAmount);
            if (newIndex >= minKey && newIndex <= maxKey) {
                result += VP_CHARS[newIndex];
            }
            else { // OOB handling
                switch (oobHandling) {
                    case "transpose":
                        while (newIndex > maxKey) {
                            newIndex -= 12;
                        }
                        while (newIndex < minKey) {
                            newIndex += 12;
                        }
                        result += VP_CHARS[newIndex];
                        break;
                    case "question":
                        result += "?";
                        break;
                    case "delete":
                        break;
                    case "octswitch":
                        // Currently not implemented
                        break;
                }
            }
        }
        if (lineIndex < inputStr.length - 1) {
            result += '\n';
        }
    }
    return result;
}

// Transpose a sheet and also push it to the output field
function transposeToOutput(str, noteAmount, minKey = 0, maxKey = VP_CHARS.length - 1) {
    outputField.value = "";
    if (transpNum.value != 0) {
        outputField.value += "Transpose " + (-noteAmount) + "\n\n"
    }
    outputField.value += transposeString(str, noteAmount, minKey, maxKey);
}

// Returns the number of out-of-bounds notes in a string
function sumOutOfBounds(str) {
    let prevOob = oobHandling;
    oobHandling = "question";

    let sum = 0;
    for (let i = 0; i < str.length; i++) {
        if (str[i] == '?') {
            sum++;
        }
    }
    oobHandling = prevOob;

    return sum;
}

// Sums the number of accidentals (sharps/flats) in a string
function sumAccidentals(str) {
    let match = str.match(ACCIDENTALS)
    return ((match != null) ? match.length : 0);
}

// Automatically find the optimal/easiest transposition (least accidentals/missing notes)
// Returns [best transpositions[], how many bad notes they have]
function findBestTranspose(originalSheet) {
    let bestIndices = []
    let bestCount = Number.MAX_SAFE_INTEGER;
    let count;

    for (let transposeAmt = -12; transposeAmt <= 12; transposeAmt++) {
        let newSheet = transposeString(originalSheet, transposeAmt);
        count = sumAccidentals(newSheet) + sumOutOfBounds(newSheet);

        // Check if current transposition is better
        if (count < bestCount) {
            bestIndices = []
            bestIndices.push(transposeAmt);

            bestCount = count;
        }
        // ...or if it's exactly as good
        else if (count == bestCount) {
            bestIndices.push(transposeAmt);
        }
    }

    return [bestIndices, bestCount];
}

// Find best transposition for sheet and push it to HTML output
function autoTransposeOutput(str) {
    let bests = findBestTranspose(str);
    autotransNum.textContent = bests[0].toString();
    autotransResult.style.display = "block";
    document.getElementById("autotrans-count").innerHTML = "(<b>" + bests[1] + "</b> bad notes)";
}
// Hide previous result
function hideAutoTranspose() {
    autotransResult.style.display = "none";
}

// Updates the "bad notes" count on octave compression buttons
function updateOctaveMisses(inputStr) {
    let oobPerOctave = [0, 0, 0];

    for (let i = 0; i < 3; i++) {
        let minKey = 12 * i;
        let maxKey = minKey + 36;
        let tempSheet = transposeString(inputStr, 0, minKey, maxKey);
        oobPerOctave[i] = sumOutOfBounds(tempSheet) + sumAccidentals(tempSheet);
        let id = compressButtons[i].id;
        let idl = id.length;
        compressButtons[i].textContent = id[idl - 2] + "-" + id[idl - 1] + " (" + oobPerOctave[i] + " bad notes)";
    }
}

// Whenever the input field updates,
// hide auto transpose results and update "bad notes" count
function onTextUpdate(inputStr) {
    hideAutoTranspose();
    updateOctaveMisses(inputStr);
}

function outputToInput() {
    inputField.value = outputField.value;
}

// --- EVENT LISTENER ASSIGNMENT --- //
// Input text update
inputField.addEventListener('input', () => { onTextUpdate(inputField.value) });

// Buttons
transpBtn.addEventListener('click', () => { transposeToOutput(inputField.value, transpNum.value) });
autoTranspBtn.addEventListener('click', () => { autoTransposeOutput(inputField.value) });
document.getElementById("transfer").addEventListener('click', outputToInput);

for (let i = 0; i < 3; i++) {
    compressButtons[i].addEventListener('click', () => { transposeToOutput(inputField.value, 0, 12 * i, (12 * i) + 36) });
}

// Config
oobSelect.addEventListener('change', () => { updateOobHandling(); updateOctaveMisses(inputField.value) });