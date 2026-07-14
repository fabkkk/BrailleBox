console.log("BrailleBox iniciado!");

let selectedLetter = null;
let statusTimer = null;


// Elementos da tela

const letterDisplay = document.getElementById("letter");
const messageDisplay = document.getElementById("message");
const statusDisplay = document.getElementById("status");

const dots = document.querySelectorAll(".dot");

const shortcut = document.getElementById("shortcut");



// Estado inicial

messageDisplay.textContent =
"Selecione uma letra do teclado.";

statusDisplay.textContent = "";



// Tabela Braille

const brailleMap = {

    A:[1],
    B:[1,3],
    C:[1,2],
    D:[1,2,4],
    E:[1,4],

    F:[1,2,3],
    G:[1,2,3,4],
    H:[1,3,4],
    I:[2,3],
    J:[2,3,4],

    K:[1,5],
    L:[1,3,5],
    M:[1,2,5],
    N:[1,2,4,5],
    O:[1,4,5],

    P:[1,2,3,5],
    Q:[1,2,3,4,5],
    R:[1,3,4,5],
    S:[2,3,5],
    T:[2,3,4,5],

    U:[1,5,6],
    V:[1,3,5,6],
    W:[2,3,4,6],
    X:[1,2,5,6],
    Y:[1,2,4,5,6],
    Z:[1,4,5,6]

};



// Limpar pontos

function clearDots(){

    dots.forEach(dot => {

        dot.classList.remove("active");

    });

}



// Mostrar Braille

function showBraille(letter){

    clearDots();


    const points = brailleMap[letter];


    if(!points) return;


    points.forEach(point => {

        dots[point - 1].classList.add("active");

    });

}



// Toast

function showToast(message){

    const toast = document.getElementById("toast");


    toast.textContent = message;


    toast.classList.add("show");


    setTimeout(()=>{

        toast.classList.remove("show");

    },2000);

}



// Selecionar letra

function selectLetter(letter){


    if(statusTimer){

        clearTimeout(statusTimer);

    }


    selectedLetter = letter;


    letterDisplay.textContent = letter;


    messageDisplay.textContent =
    `Letra ${letter} selecionada.`;



    statusDisplay.textContent = "";



    showBraille(letter);



    shortcut.classList.add("show");


}



// Teclado

document.addEventListener("keydown", (event)=>{


    const key = event.key.toUpperCase();



    // Letras A-Z

    if(/^[A-Z]$/.test(key)){


        selectLetter(key);


        return;

    }



    // Espaço confirma

    if(event.code === "Space"){


        event.preventDefault();



        if(selectedLetter){


            const confirmedLetter = selectedLetter;



            messageDisplay.textContent =
`Letra ${confirmedLetter} confirmada.`;

shortcut.classList.remove("show");

selectedLetter = null;


statusTimer = setTimeout(()=>{

    messageDisplay.textContent =
    "Aguardando nova letra.";

},1500);


        }


        else{


            showToast(
                "Nenhuma letra selecionada."
            );


        }


        return;

    }



    showToast(
        "Isso não é uma letra."
    );


});