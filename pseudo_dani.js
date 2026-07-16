console.log("BrailleBox iniciado!");

let selectedLetter = null;
let statusTimer = null;


const letterDisplay = document.getElementById("letter");
const messageDisplay = document.getElementById("message");
const statusDisplay = document.getElementById("status");

const dots = document.querySelectorAll(".dot");

const shortcut = document.getElementById("shortcut");


messageDisplay.textContent =
"Selecione uma letra do teclado.";

statusDisplay.textContent = "";



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




function clearDots(){

    dots.forEach(dot => {

        dot.classList.remove("active");

    });

}


function showBraille(letter){

    clearDots();


    const points = brailleMap[letter];


    if(!points) return;


    points.forEach(point => {

        dots[point - 1].classList.add("active");

    });

}



function showToast(message){

    const toast = document.getElementById("toast");


    toast.textContent = message;


    toast.classList.add("show");


    setTimeout(()=>{

        toast.classList.remove("show");

    },2000);

}



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

fetch(`/selecionar/${letter}`,{
    method:"POST"
})
.then(()=>{

    shortcut.classList.add("show");

});

}


document.addEventListener("keydown", (event)=>{


    const key = event.key.toUpperCase();



    if(/^[A-Z]$/.test(key)){


        selectLetter(key);


        return;

    }



    if(event.code === "Space"){


        event.preventDefault();



        if(selectedLetter){


            const confirmedLetter = selectedLetter;

            fetch("/confirmar", {

    method:"POST",

    headers:{

        "Content-Type":"application/json"

    },

    body:JSON.stringify({

        letra:confirmedLetter

    })

})

.then(response => response.json())

.then(data => {

    console.log(data);

});



            messageDisplay.textContent =
`Letra ${confirmedLetter} confirmada.`;

shortcut.classList.remove("show");

selectedLetter = null;


statusTimer = setTimeout(()=>{


    messageDisplay.style.opacity = "0";


    setTimeout(()=>{


        messageDisplay.textContent =
        "Aguardando nova letra.";

        fetch("/audio/nova", {
    method:"POST"
});


        messageDisplay.style.opacity = "1";


    },350);



},5000);


        }


        else{


            fetch("/audio/nenhuma", {
    method:"POST"
});

showToast(
    "Nenhuma letra selecionada."
);


        }


        return;

    }



    fetch("/audio/invalida", {
    method:"POST"
});

showToast(
    "Tecla inválida."
);


});
