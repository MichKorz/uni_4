const words = ["programowanie","wisielec","javascript","kwiat","rower","komputer"];
let selectedWord = "";
let guessedLetters = [];
let clickedLetters = [];  // <— nowa tablica do śledzenia wszystkich kliknięć
let wrongGuesses = 0;
const maxWrong = 6;

const wordDiv = document.getElementById("word");
const lettersDiv = document.getElementById("letters");
const messageDiv = document.getElementById("message");
const restartButton = document.getElementById("restart");
const cancelButton = document.getElementById("cancel");
const canvas = document.getElementById("hangman");
const ctx = canvas.getContext("2d");

function saveState() {
    const state = {
        selectedWord,
        guessedLetters,
        clickedLetters,
        wrongGuesses
    };
    localStorage.setItem("hangmanState", JSON.stringify(state));
}

function loadState() {
    const raw = localStorage.getItem("hangmanState");
    if (!raw) return false;
    try {
        const { selectedWord: w, guessedLetters: g, clickedLetters: c, wrongGuesses: x } = JSON.parse(raw);
        if (w && Array.isArray(g) && Array.isArray(c) && typeof x === "number") {
            selectedWord = w;
            guessedLetters = g;
            clickedLetters = c;
            wrongGuesses = x;
            return true;
        }
    } catch (e) { /* parsowanie nieudane */ }
    return false;
}

function clearState() {
    localStorage.removeItem("hangmanState");
}

function drawHangman() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    if (wrongGuesses > 0) {
        ctx.beginPath(); ctx.moveTo(10,190); ctx.lineTo(190,190); ctx.stroke();
    }
    if (wrongGuesses > 1) {
        ctx.beginPath(); ctx.moveTo(50,190); ctx.lineTo(50,20);
        ctx.lineTo(150,20); ctx.lineTo(150,40); ctx.stroke();
    }
    if (wrongGuesses > 2) {
        ctx.beginPath(); ctx.arc(150,55,15,0,Math.PI*2); ctx.stroke();
    }
    if (wrongGuesses > 3) {
        ctx.beginPath(); ctx.moveTo(150,70); ctx.lineTo(150,120); ctx.stroke();
    }
    if (wrongGuesses > 4) {
        ctx.beginPath(); ctx.moveTo(150,80); ctx.lineTo(130,100); ctx.stroke();
    }
    if (wrongGuesses > 5) {
        ctx.beginPath(); ctx.moveTo(150,80); ctx.lineTo(170,100); ctx.stroke();
    }
}

function displayWord() {
    wordDiv.innerHTML = selectedWord
        .split("")
        .map(l => guessedLetters.includes(l) ? l : "_")
        .join(" ");
}

function createLetterButtons() {
    lettersDiv.innerHTML = "";
    const alphabet = "aąbcćdeęfghijklłmnńoóprsśtuwyzźż";
    alphabet.split("").forEach(letter => {
        const btn = document.createElement("div");
        btn.textContent = letter;
        btn.className = "letter";
        // jeśli wczoraj kliknięto tę literę, od razu zaznacz jako "clicked"
        if (clickedLetters.includes(letter)) {
            btn.classList.add("clicked");
        }
        btn.addEventListener("click", () => handleGuess(letter, btn));
        lettersDiv.appendChild(btn);
    });
}

function handleGuess(letter, btn) {
    if (btn.classList.contains("clicked")) return;
    btn.classList.add("clicked");
    clickedLetters.push(letter);

    if (selectedWord.includes(letter)) {
        guessedLetters.push(letter);
        displayWord();
        checkWin();
    } else {
        wrongGuesses++;
        drawHangman();
        checkLose();
    }
    saveState();
}

function checkWin() {
    if (selectedWord.split("").every(l => guessedLetters.includes(l))) {
        messageDiv.textContent = "Gratulacje! Wygrałeś!";
        disableAllButtons();
        clearState();
    }
}

function checkLose() {
    if (wrongGuesses >= maxWrong) {
        messageDiv.textContent = `Przegrałeś! Słowo to: ${selectedWord}`;
        disableAllButtons();
        clearState();
    }
}

function disableAllButtons() {
    document.querySelectorAll(".letter").forEach(btn => btn.classList.add("clicked"));
}

function startGame() {
    if (!loadState()) {
        selectedWord = words[Math.floor(Math.random() * words.length)];
        guessedLetters = [];
        clickedLetters = [];
        wrongGuesses = 0;
        clearState();
    }
    messageDiv.textContent = "";
    drawHangman();
    displayWord();
    createLetterButtons();
}

restartButton.addEventListener("click", () => {
    clearState();
    startGame();
});
cancelButton.addEventListener("click", () => {
    messageDiv.textContent = "Gra anulowana.";
    wordDiv.textContent = "";
    lettersDiv.innerHTML = "";
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    clearState();
});

window.addEventListener("load", startGame);
